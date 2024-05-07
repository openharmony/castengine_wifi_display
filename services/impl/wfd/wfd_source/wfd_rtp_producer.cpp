/*
 * Copyright (c) 2023 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "wfd_rtp_producer.h"
#include <unistd.h>
#include "common/common_macro.h"
#include "common/reflect_registration.h"
#include "configuration/include/config.h"
#include "extend/magic_enum/magic_enum.hpp"
#include "protocol/frame/aac_frame.h"
#include "protocol/frame/h264_frame.h"
#include "utils/utils.h"
#include "wfd_media_def.h"
#include "wfd_session_def.h"

namespace OHOS {
namespace Sharing {

WfdRtpProducer::UdpClient::UdpClient(bool rtcp) : rtcp_(rtcp) {}

WfdRtpProducer::UdpClient::~UdpClient()
{
    Release();
}

void WfdRtpProducer::UdpClient::OnClientClose(int32_t fd)
{
    SHARING_LOGD("trace.");
    (void)fd;
}

void WfdRtpProducer::UdpClient::OnClientWriteable(int32_t fd)
{
    SHARING_LOGD("trace.");
    (void)fd;
}

void WfdRtpProducer::UdpClient::OnClientException(int32_t fd)
{
    SHARING_LOGD("trace.");
    (void)fd;
}

void WfdRtpProducer::UdpClient::OnClientConnect(bool isSuccess)
{
    SHARING_LOGD("trace.");
    (void)isSuccess;
}

void WfdRtpProducer::UdpClient::OnClientReadData(int32_t fd, DataBuffer::Ptr buf)
{
    SHARING_LOGD("trace.");
    auto listener = udpDataListener_.lock();
    if (listener && rtcp_) {
        listener->OnRtcpReadData(fd, buf);
    }
}

void WfdRtpProducer::UdpClient::SetUdpDataListener(std::weak_ptr<WfdRtpProducer> udpDataListener)
{
    SHARING_LOGD("trace.");
    udpDataListener_ = udpDataListener;
}

void WfdRtpProducer::UdpClient::Stop()
{
    SHARING_LOGD("trace.");
    if (networkClientPtr_ != nullptr) {
        networkClientPtr_->Disconnect();
        networkClientPtr_.reset();
    }
}

void WfdRtpProducer::UdpClient::Release()
{
    SHARING_LOGD("trace.");
    Stop();
}

bool WfdRtpProducer::UdpClient::Connect(const std::string &peerIp, uint16_t peerPort, const std::string &localIp,
                                        uint16_t localPort)
{
    SHARING_LOGD("trace.");
    return NetworkFactory::CreateUdpClient(peerIp, peerPort, localIp, localPort, shared_from_this(), networkClientPtr_);
}

bool WfdRtpProducer::UdpClient::SendDataBuffer(const DataBuffer::Ptr &buf)
{
    MEDIA_LOGD("trace.");
    if (networkClientPtr_ && buf) {
        return networkClientPtr_->Send(buf, buf->Size());
    }
    return false;
}

WfdRtpProducer::WfdRtpProducer()
{
    SHARING_LOGD("ctor.");
}

WfdRtpProducer::~WfdRtpProducer()
{
    SHARING_LOGD("dtor producer Id: %{public}u.", GetId());
}

void WfdRtpProducer::UpdateOperation(ProsumerStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);
    SHARING_LOGD("status: %{public}s producerId: %{public}u.",
                 std::string(magic_enum::enum_name(static_cast<ProsumerOptRunningStatus>(statusMsg->status))).c_str(),
                 GetId());
    switch (statusMsg->status) {
        case ProsumerOptRunningStatus::PROSUMER_INIT:
            statusMsg->status = PROSUMER_NOTIFY_INIT_SUCCESS;
            break;
        case PROSUMER_START: {
            isPaused_ = false;
            if (isInit_ && (Connect() == 0)) {
                statusMsg->status = PROSUMER_NOTIFY_START_SUCCESS;
            } else {
                statusMsg->status = PROSUMER_NOTIFY_ERROR;
                statusMsg->errorCode = ERR_PROSUMER_START;
            }
            break;
        }
        case PROSUMER_PAUSE: {
            isPaused_ = true;
            statusMsg->status = PROSUMER_NOTIFY_PAUSE_SUCCESS;
            break;
        }

        case PROSUMER_RESUME: {
            isPaused_ = false;
            statusMsg->status = PROSUMER_NOTIFY_RESUME_SUCCESS;
            break;
        }

        case PROSUMER_STOP: {
            if (Stop() == 0) {
                statusMsg->status = PROSUMER_NOTIFY_STOP_SUCCESS;
            } else {
                statusMsg->status = PROSUMER_NOTIFY_ERROR;
                statusMsg->errorCode = ERR_PROSUMER_STOP;
            }
            break;
        }

        case PROSUMER_DESTROY: {
            Release();
            statusMsg->status = PROSUMER_NOTIFY_DESTROY_SUCCESS;
            break;
        }

        default:
            break;
    }
    Notify(statusMsg);
}

void WfdRtpProducer::PublishOneFrame(const MediaData::Ptr &mediaData)
{
    MEDIA_LOGD("trace.");
    RETURN_IF_NULL(mediaData);
    RETURN_IF_NULL(mediaData->buff);
    RETURN_IF_NULL(mediaData->buff->Peek());
    auto buff = mediaData->buff;
    if (isRunning_ && !isPaused_) { // paused check at this pos or in DispatchMediaData
        if (mediaData->mediaType == MEDIA_TYPE_AUDIO) {
            auto aacFrame = FrameImpl::CreateFrom(std::move(*buff));
            aacFrame->codecId_ = CodecId::CODEC_AAC;
            tsPacker_->InputFrame(aacFrame);
        } else if (mediaData->mediaType == MEDIA_TYPE_VIDEO) {
            MEDIA_LOGD("video frame pts:%{public}" PRId64 ".", mediaData->pts);
            auto h264Frame = std::make_shared<H264Frame>(std::move(*buff));
            h264Frame->dts_ = h264Frame->pts_ = static_cast<uint32_t>(mediaData->pts);
            h264Frame->prefixSize_ = PrefixSize(h264Frame->Peek(), h264Frame->Size());
            h264Frame->codecId_ = CODEC_H264;
            tsPacker_->InputFrame(h264Frame);
        }
    }
}

int32_t WfdRtpProducer::HandleEvent(SharingEvent &event)
{
    RETURN_INVALID_IF_NULL(event.eventMsg);
    SHARING_LOGI("HandleEvent eventType: %{public}d.", event.eventMsg->type);
    switch (event.eventMsg->type) {
        case EventType::EVENT_WFD_MEDIA_INIT: {
            HandleMediaInit(ConvertEventMsg<WfdProducerEventMsg>(event));
            break;
        }
        default:
            break;
    }
    return 0;
}

bool WfdRtpProducer::ProducerInit()
{
    SHARING_LOGD("trace.");
    if (InitTsRtpPacker(ssrc_) != 0) {
        SHARING_LOGE("init rtp packer failed.");
        return false;
    }

    if (InitUdpClients() != 0) {
        SHARING_LOGE("init udp clients failed.");
        return false;
    }

    isInit_ = true;
    return true;
}

int32_t WfdRtpProducer::InitTsRtpPacker(uint32_t ssrc, size_t mtuSize, uint32_t sampleRate, uint8_t pt,
                                        RtpPayloadStream ps)
{
    SHARING_LOGD("trace.");
    tsPacker_ = RtpFactory::CreateRtpPack(ssrc, mtuSize, sampleRate, pt, ps, 0);
    if (tsPacker_ == nullptr) {
        SHARING_LOGE("createRtpPacker failed.");
        return -1;
    }
    DataBuffer::Ptr buffer = std::make_shared<DataBuffer>();
    tsPacker_->SetOnRtpPack([=](const RtpPacket::Ptr &rtp) {
        MEDIA_LOGD("rtp packed seq: %{public}d timestamp: %{public}d size: %{public}d.", rtp->GetSeq(), rtp->GetStamp(),
                   rtp->Size());
        // Reset ori data, data len is 188, maybe use the data ori instead.
        buffer->ReplaceData(rtp->Peek(), rtp->Size());
        SendDataBuffer(buffer);
    });
    return 0;
}

int32_t WfdRtpProducer::InitUdpClients()
{
    SHARING_LOGD("trace.");
    tsUdpClient_ = std::make_shared<UdpClient>(false);
    tsRtcpUdpClient_ = std::make_shared<UdpClient>(true);
    tsRtcpUdpClient_->SetUdpDataListener(shared_from_this());

    SharingValue::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("mediachannel", "rtcpLimit", "timeout", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        values->GetValue<int32_t>(rtcpCheckInterval_);
    }

    if (rtcpCheckInterval_ > 0) {
        rtcpSendContext_ = std::make_shared<RtcpSenderContext>();
    }

    return 0;
}

bool WfdRtpProducer::SendDataBuffer(const DataBuffer::Ptr &buf, bool audio)
{
    MEDIA_LOGD("trace.");
    RETURN_FALSE_IF_NULL(buf);
    (void)audio;
    return tsUdpClient_->SendDataBuffer(buf);
}

int32_t WfdRtpProducer::Connect()
{
    SHARING_LOGD("trace.");
    if (tsPacker_ == nullptr) {
        SHARING_LOGE("connect resource not useable.");
        return -1;
    }

    if (tsUdpClient_ == nullptr || tsRtcpUdpClient_ == nullptr) {
        SHARING_LOGE("udp network need init.");
        return -1;
    }
    // create two udp client
    if (!tsUdpClient_->Connect(primarySinkIp_, primarySinkPort_, localIp_, localPort_) ||
        !tsRtcpUdpClient_->Connect(primarySinkIp_, primarySinkPort_ + 1, localIp_, localPort_ + 1)) {
        SHARING_LOGE("createNetworkClient failed.");
        return -1;
    }

    SHARING_LOGD("createNetworkClient success.");

    isRunning_ = true;
    return 0;
}

int32_t WfdRtpProducer::Stop()
{
    SHARING_LOGD("producerId: %{public}u.", GetId());
    isRunning_ = false;
    if (tsUdpClient_ != nullptr) {
        tsUdpClient_->Stop();
    }

    if (tsRtcpUdpClient_ != nullptr) {
        tsRtcpUdpClient_->Stop();
    }

    dispatching_ = false;
    NotifyReadStop();
    if (dispatchThread_ && dispatchThread_->joinable()) {
        dispatchThread_->join();
        dispatchThread_ = nullptr;
    }
    return 0;
}

int32_t WfdRtpProducer::Release()
{
    SHARING_LOGD("producerId: %{public}u.", GetId());
    if (tsUdpClient_ != nullptr) {
        tsUdpClient_.reset();
    }

    if (tsRtcpUdpClient_ != nullptr) {
        tsRtcpUdpClient_.reset();
    }

    if (tsPacker_ != nullptr) {
        tsPacker_.reset();
    }

    if (rtcpSendContext_ != nullptr) {
        rtcpSendContext_.reset();
    }
    return 0;
}

void WfdRtpProducer::OnRtcpReadData(int32_t fd, DataBuffer::Ptr buf)
{
    MEDIA_LOGD("trace.");
    (void)fd;
    if (buf && (buf->Size() > 0)) {
        MEDIA_LOGD("recv rtcp rsp, producerId: %{public}u.", GetId());
        rtcpOvertimes_ = 0;
    }
}

void WfdRtpProducer::OnRtcpTimeOut()
{
    MEDIA_LOGD("producerId: %{public}u tid: %{public}d.", GetId(), gettid());
    auto listener = listener_.lock();
    if (listener && (rtcpOvertimes_ >= 3)) { // 3 : DEFAULT_RTCP_OVERTIMES
        SHARING_LOGW("rtcp time out,producerId: %{public}u,tid: %{public}d.", GetId(), gettid());
        ProsumerStatusMsg::Ptr pMsg = std::make_shared<ProsumerStatusMsg>();
        pMsg->prosumerId = GetId();
        pMsg->agentId = GetSrcAgentId();
        pMsg->eventMsg = std::make_shared<AgentEventMsg>();
        pMsg->status = ProsumerNotifyStatus::PROSUMER_NOTIFY_ERROR;
        pMsg->eventMsg->type = EVENT_AGENT_PROSUMER_ERROR;
        pMsg->errorCode = ERR_PROSUMER_TIMEOUT;
        listener->OnProducerNotify(pMsg);
        rtcpOvertimes_ = 0;
        return;
    } else {
        rtcpOvertimes_++;
    }
    if (rtcpSendContext_ && tsRtcpUdpClient_) {
        tsRtcpUdpClient_->SendDataBuffer(rtcpSendContext_->CreateRtcpSR(ssrc_));
    }
}

void WfdRtpProducer::StartDispatchThread()
{
    SHARING_LOGD("trace.");
    dispatching_ = true;
    dispatchThread_ = std::make_shared<std::thread>(&WfdRtpProducer::DispatchMediaData, this);
}

void WfdRtpProducer::DispatchMediaData()
{
    SHARING_LOGD("trace.");
    uint32_t rtpCount = 0;
    MediaData::Ptr mediaData = std::make_shared<MediaData>();
    mediaData->buff = std::make_shared<DataBuffer>();
    while (dispatching_) {
        int32_t ret = RequestRead(MEDIA_TYPE_AV, [&mediaData](const MediaData::Ptr &data) {
            mediaData->buff->ReplaceData(data->buff->Peek(), data->buff->Size());
            mediaData->keyFrame = data->keyFrame;
            mediaData->mediaType = data->mediaType;
            mediaData->pts = data->pts;
            mediaData->isRaw = data->isRaw;
            mediaData->ssrc = data->ssrc;
        });
        if (ret != 0) {
            SHARING_LOGE("Request media data err :%{public}d.", ret);
            continue;
        }
        MEDIA_LOGD("receiver: %{public}u producerId: %{public}u received a av: %{public}d data :%{public}d \
                   from dispatcher: %{public}u.",
                   GetReceiverId(), GetId(), (int)mediaData->mediaType, mediaData->buff->Size(), GetDispatcherId());
        if (mediaData->keyFrame) {
            auto sps = GetSPS();
            if (sps != nullptr && sps->buff != nullptr) {
                auto spsFrame = std::make_shared<MediaData>(*sps);
                spsFrame->buff = std::make_shared<DataBuffer>(*(sps->buff));
                spsFrame->pts = mediaData->pts;
                PublishOneFrame(spsFrame);
            }

            auto pps = GetPPS();
            if (pps != nullptr && pps->buff != nullptr) {
                auto ppsFrame = std::make_shared<MediaData>(*pps);
                ppsFrame->buff = std::make_shared<DataBuffer>(*(pps->buff));
                ppsFrame->pts = mediaData->pts;
                PublishOneFrame(ppsFrame);
            }
        }

        PublishOneFrame(mediaData);

        if (rtcpCheckInterval_ > 0) {
            ++rtpCount;
            if ((rtpCount & 0x3ff) == 1023) { // 1023 : rtsp timeout
                OnRtcpTimeOut();
            }
        }
    }
}

void WfdRtpProducer::HandleMediaInit(std::shared_ptr<WfdProducerEventMsg> msg)
{
    if (!msg) {
        SHARING_LOGE("msg is nullptr.");
        return;
    }
    primarySinkPort_ = msg->port;
    primarySinkIp_ = msg->ip;
    localPort_ = msg->localPort;
    localIp_ = msg->localIp;
    SHARING_LOGD("primarySinkIp:%s port:%d localIp:%s localPort:%d.", GetAnonyString(primarySinkIp_).c_str(),
                 primarySinkPort_, GetAnonyString(localIp_).c_str(), localPort_);
    SharingErrorCode errCode = ERR_OK;
    if (!ProducerInit()) {
        errCode = ERR_PROSUMER_INIT;
    }
    auto pPrivateMsg = std::make_shared<WfdSourceSessionEventMsg>();
    pPrivateMsg->errorCode = errCode;
    pPrivateMsg->type = EVENT_WFD_STATE_MEDIA_INIT;
    pPrivateMsg->toMgr = ModuleType::MODULE_CONTEXT;
    pPrivateMsg->fromMgr = ModuleType::MODULE_MEDIACHANNEL;
    pPrivateMsg->srcId = GetId();
    pPrivateMsg->dstId = msg->srcId;
    pPrivateMsg->agentId = GetSrcAgentId();
    pPrivateMsg->prosumerId = GetId();
    NotifyPrivateEvent(pPrivateMsg);
}

REGISTER_CLASS_REFLECTOR(WfdRtpProducer);
} // namespace Sharing
} // namespace OHOS

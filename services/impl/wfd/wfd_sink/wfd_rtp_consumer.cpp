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

#include "wfd_rtp_consumer.h"
#include <chrono>
#include "extend/magic_enum/magic_enum.hpp"
#include "common/reflect_registration.h"
#include "configuration/include/config.h"
#include "event_comm.h"
#include "protocol/frame/h264_frame.h"
#include "wfd_media_def.h"
#include "wfd_session_def.h"

namespace OHOS {
namespace Sharing {

WfdRtpConsumer::WfdRtpConsumer()
{
    SHARING_LOGI("trace.");
}

WfdRtpConsumer::~WfdRtpConsumer()
{
    SHARING_LOGI("wfd rtp consumer Id: %{public}u.", GetId());
    Release();
}

int32_t WfdRtpConsumer::HandleEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(event.eventMsg);

    SHARING_LOGI("eventType: %{public}s, wfd rtp consumerId: %{public}u.",
                 std::string(magic_enum::enum_name(event.eventMsg->type)).c_str(), GetId());
    switch (event.eventMsg->type) {
        case EventType::EVENT_WFD_MEDIA_INIT:
            HandleProsumerInitState(event);
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }

    return 0;
}

void WfdRtpConsumer::HandleProsumerInitState(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(event.eventMsg);

    auto msg = ConvertEventMsg<WfdConsumerEventMsg>(event);
    if (msg) {
        port_ = msg->port;
        if (msg->audioTrack.codecId != CodecId::CODEC_NONE) {
            audioTrack_ = msg->audioTrack;
        }
        if (msg->videoTrack.codecId != CodecId::CODEC_NONE) {
            videoTrack_ = msg->videoTrack;
        }
    }

    isInit_ = true;

    auto pPrivateMsg = std::make_shared<WfdSinkSessionEventMsg>();
    pPrivateMsg->errorCode = ERR_OK;
    pPrivateMsg->type = EVENT_AGENT_STATE_PROSUMER_INIT;
    pPrivateMsg->toMgr = ModuleType::MODULE_CONTEXT;
    pPrivateMsg->fromMgr = ModuleType::MODULE_MEDIACHANNEL;
    pPrivateMsg->srcId = GetId();
    pPrivateMsg->dstId = event.eventMsg->srcId;
    contextId_ = event.eventMsg->srcId;
    pPrivateMsg->agentId = GetSinkAgentId();
    pPrivateMsg->prosumerId = GetId();

    NotifyPrivateEvent(pPrivateMsg);
}

void WfdRtpConsumer::UpdateOperation(ProsumerStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);

    SHARING_LOGD("status: %{public}s consumerId: %{public}u.",
                 std::string(magic_enum::enum_name(static_cast<ProsumerOptRunningStatus>(statusMsg->status))).c_str(),
                 GetId());
    switch (statusMsg->status) {
        case PROSUMER_INIT:
            if (Init()) {
                statusMsg->status = PROSUMER_NOTIFY_INIT_SUCCESS;
            } else {
                statusMsg->status = PROSUMER_NOTIFY_INIT_SUCCESS;
                statusMsg->errorCode = ERR_PROSUMER_CREATE;
            }
            break;
        case PROSUMER_START:
            if (isInit_ && Start()) {
                statusMsg->status = PROSUMER_NOTIFY_START_SUCCESS;
            } else {
                statusMsg->status = PROSUMER_NOTIFY_ERROR;
                statusMsg->errorCode = ERR_PROSUMER_START;
            }
            break;
        case PROSUMER_PAUSE:
            isPaused_ = true;
            mediaTypePaused_ = statusMsg->mediaType;
            statusMsg->status = PROSUMER_NOTIFY_PAUSE_SUCCESS;
            break;
        case PROSUMER_RESUME:
            isPaused_ = false;
            statusMsg->status = PROSUMER_NOTIFY_RESUME_SUCCESS;
            break;
        case PROSUMER_STOP:
            Stop();
            statusMsg->status = PROSUMER_NOTIFY_STOP_SUCCESS;
            break;
        case PROSUMER_DESTROY:
            Release();
            statusMsg->status = PROSUMER_NOTIFY_DESTROY_SUCCESS;
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }

    Notify(statusMsg);
}

bool WfdRtpConsumer::Init()
{
    SHARING_LOGD("trace.");
    return InitRtpUnpacker();
}

bool WfdRtpConsumer::Start()
{
    SHARING_LOGD("trace.");
    if (!StartNetworkServer(port_, rtpServer_.second, rtpServer_.first)) {
        SHARING_LOGE("start rtp server port: %{public}d failed.", port_);
        return false;
    }

    SHARING_LOGD("start reveiver server success.");
    isRunning_ = true;
    return true;
}

bool WfdRtpConsumer::Stop()
{
    SHARING_LOGD("trace.");
    isRunning_ = false;
    if (rtpServer_.second) {
        rtpServer_.second->Stop();
        rtpServer_.second.reset();
    }

    if (rtpUnpacker_) {
        rtpUnpacker_->Release();
        rtpUnpacker_.reset();
    }

    return true;
}

int32_t WfdRtpConsumer::Release()
{
    SHARING_LOGD("WFDRtpConsumerId: %{public}u.", GetId());
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> diff = end - gopInterval_;

    SHARING_LOGD(
        "TEST STATISTIC Miracast:finish, interval:%{public}.0f ms, agent ID:%{public}d, "
        "get video frame, gop:%{public}d, average receiving frames time:%{public}.0f ms.",
        diff.count(), GetSinkAgentId(), frameNums_, diff.count() / frameNums_);

    Stop();
    return 0;
}

bool WfdRtpConsumer::InitRtpUnpacker()
{
    SHARING_LOGD("trace.");
    RtpPlaylodParam arpp = {33, 90000, RtpPayloadStream::MPEG2_TS}; // 33 : ts rtp payload, 90000 : sampe rate
    rtpUnpacker_ = RtpFactory::CreateRtpUnpack(arpp);
    if (rtpUnpacker_ != nullptr) {
        // data callback
        rtpUnpacker_->SetOnRtpUnpack(
            std::bind(&WfdRtpConsumer::OnRtpUnpackCallback, this, std::placeholders::_1, std::placeholders::_2));
        // notify callback
        rtpUnpacker_->SetOnRtpNotify(std::bind(&WfdRtpConsumer::OnRtpUnpackNotify, this, std::placeholders::_1));
    } else {
        SHARING_LOGE("wfd init rtp unpacker failed.");
        return false;
    }

    return true;
}

void WfdRtpConsumer::OnRtpUnpackNotify(int32_t errCode)
{
    SHARING_LOGD("errCode: %{public}d.", errCode);
}

void WfdRtpConsumer::OnRtpUnpackCallback(uint32_t ssrc, const Frame::Ptr &frame)
{
    MEDIA_LOGD("trace.");
    RETURN_IF_NULL(frame);
    auto listener = listener_.lock();
    RETURN_IF_NULL(listener);

    auto dispatcher = listener->GetDispatcher();
    RETURN_IF_NULL(dispatcher);

    MediaData::Ptr mediaData;
    if (frame->GetTrackType() == TRACK_AUDIO) {
        if (isPaused_ && (mediaTypePaused_ == MEDIA_TYPE_AUDIO || mediaTypePaused_ == MEDIA_TYPE_AV)) {
            return;
        }

        mediaData = std::make_shared<MediaData>();
        mediaData->isRaw = false;
        mediaData->keyFrame = false;
        mediaData->mediaType = MEDIA_TYPE_AUDIO;
        mediaData->buff = frame;
        mediaData->pts = frame->Pts();
        MEDIA_LOGD("audio & put it into dispatcher: %{public}u, consumerId: %{public}u.", dispatcher->GetDispatcherId(),
                   GetId());

        dispatcher->InputData(mediaData);
    } else if (frame->GetTrackType() == TRACK_VIDEO) {
        if (isPaused_ && (mediaTypePaused_ == MEDIA_TYPE_VIDEO || mediaTypePaused_ == MEDIA_TYPE_AV)) {
            return;
        }

        auto p = frame->Data();
        p = *(p + 2) == 0x01 ? p + 3 : p + 4; // 2: fix offset, 3: fix offset, 4: fix offset
        if (((p[0]) & 0x1f) == 0x01) {
            mediaData = std::make_shared<MediaData>();
            mediaData->mediaType = MEDIA_TYPE_VIDEO;
            mediaData->isRaw = false;
            mediaData->keyFrame = false;
            mediaData->buff = frame;
            mediaData->pts = frame->Pts();

            dispatcher->InputData(mediaData);
            frameNums_++;
        } else {
            SplitH264((char *)frame->Data(), frame->Size(), 0, [&](const char *buf, size_t len, size_t prefix) {
                if ((*(buf + prefix) & 0x1f) == 0x06) {
                    // discard the SEI data
                    return;
                }

                if ((*(buf + prefix) & 0x1f) == 0x07) {
                    auto spsOld = dispatcher->GetSPS();
                    if (spsOld != nullptr && spsOld->buff != nullptr) {
                        return;
                    }

                    // set sps into buffer dispathcer
                    auto sps = std::make_shared<MediaData>();
                    sps->buff = std::make_shared<DataBuffer>();
                    sps->mediaType = MEDIA_TYPE_VIDEO;
                    sps->buff->Assign((char *)buf, len);

                    dispatcher->SetSpsNalu(sps);
                    return;
                }
                if ((*(buf + prefix) & 0x1f) == 0x08) {
                    auto ppsOld = dispatcher->GetPPS();
                    if (ppsOld != nullptr && ppsOld->buff != nullptr) {
                        return;
                    }

                    // set pps into buffer dispather
                    auto pps = std::make_shared<MediaData>();
                    pps->buff = std::make_shared<DataBuffer>();
                    pps->mediaType = MEDIA_TYPE_VIDEO;
                    pps->buff->Assign((char *)buf, len);

                    dispatcher->SetPpsNalu(pps);
                    return;
                }
                auto mediaData = std::make_shared<MediaData>();
                mediaData->buff = std::make_shared<DataBuffer>();
                mediaData->mediaType = MEDIA_TYPE_VIDEO;
                mediaData->isRaw = false;
                // i-frame is key frame
                mediaData->keyFrame = (*(buf + prefix) & 0x1f) == 0x05 ? true : false;

                if (mediaData->keyFrame) {
                    if (isFirstKeyFrame_) {
                        MEDIA_LOGD("TEST STATISTICS Miracast:first, agent ID:%{public}d, get video frame.",
                                   GetSinkAgentId());
                        isFirstKeyFrame_ = false;
                    } else {
                        auto end = std::chrono::steady_clock::now();
                        std::chrono::duration<double, std::milli> diff = end - gopInterval_;
                        MEDIA_LOGD("TEST STATISTIC Miracast:interval:%{public}.0f ms, "
                            "agent ID:%{public}d, get video frame, gop:%{public}d, "
                            "average receiving frames time:%{public}.0f ms.",
                            diff.count(), GetSinkAgentId(), frameNums_, diff.count() / frameNums_);
                    }

                    frameNums_ = 1;
                    gopInterval_ = std::chrono::steady_clock::now();
                }

                mediaData->buff->ReplaceData((char *)buf, len);
                mediaData->pts = frame->Pts();

                dispatcher->InputData(mediaData);
            });
        }
    }
}

void WfdRtpConsumer::OnServerReadData(int32_t fd, DataBuffer::Ptr buf, INetworkSession::Ptr sesssion)
{
    if (rtpUnpacker_ != nullptr && isRunning_) {
        rtpUnpacker_->ParseRtp(buf->Peek(), buf->Size());
        if (isFirstPacket_) {
            SHARING_LOGD("TEST STATISTICS Miracast:first, agent ID:%{public}d, recv first packet.", GetSinkAgentId());
            isFirstPacket_ = false;
        }
    }
}

bool WfdRtpConsumer::StartNetworkServer(uint16_t port, NetworkFactory::ServerPtr &server, int32_t &fd)
{
    SHARING_LOGD("trace.");
    if (!NetworkFactory::CreateUdpServer(port, "0.0.0.0", shared_from_this(), server)) {
        server.reset();
        return false;
    }

    fd = server->GetSocketInfo()->GetLocalFd();
    return true;
}

REGISTER_CLASS_REFLECTOR(WfdRtpConsumer);
} // namespace Sharing
} // namespace OHOS

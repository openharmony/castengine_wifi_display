/*
 * Copyright (c) 2023 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "screen_capture_consumer.h"
#include "common/common_macro.h"
#include "common/reflect_registration.h"
#include "common/sharing_log.h"
#include "screen_capture_def.h"

namespace OHOS {
namespace Sharing {
void ScreenCaptureConsumer::OnFrameBufferUsed()
{
    ReleaseScreenBuffer();
}

void ScreenCaptureConsumer::HandleSpsFrame(BufferDispatcher::Ptr dispatcher, const Frame::Ptr &frame)
{
    auto spsOld = dispatcher->GetSPS();
    if (spsOld != nullptr && spsOld->buff != nullptr) {
        return;
    }
    // set sps into buffer dispathcer
    auto sps = std::make_shared<MediaData>();
    sps->buff = move(frame);
    sps->mediaType = MEDIA_TYPE_VIDEO;
    dispatcher->SetSpsNalu(sps);
}

void ScreenCaptureConsumer::HandlePpsFrame(BufferDispatcher::Ptr dispatcher, const Frame::Ptr &frame)
{
    auto ppsOld = dispatcher->GetPPS();
    if (ppsOld != nullptr && ppsOld->buff != nullptr) {
        return;
    }
    // set pps into buffer dispather
    auto pps = std::make_shared<MediaData>();
    pps->buff = move(frame);
    pps->mediaType = MEDIA_TYPE_VIDEO;
    dispatcher->SetPpsNalu(pps);
}

void ScreenCaptureConsumer::OnFrame(const Frame::Ptr &frame, FRAME_TYPE frameType, bool keyFrame)
{
    SHARING_LOGD("trace.");
    if (frame == nullptr) {
        SHARING_LOGE("Screen Capture encodedBuf is null!");
        return;
    }

    if (listener_.expired() || IsPaused()) {
        return;
    }

    auto listener = listener_.lock();
    BufferDispatcher::Ptr dispatcher = listener->GetDispatcher();
    if (dispatcher) {
        static std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
        uint32_t len = frame->Size();
        switch (frameType) {
            case SPS_FRAME: {
                SHARING_LOGD("get sps frame, size:%{public}u.", len);
                HandleSpsFrame(dispatcher, frame);
                break;
            }
            case PPS_FRAME: {
                SHARING_LOGD("get pps frame, size:%{public}u.", len);
                HandlePpsFrame(dispatcher, frame);
                break;
            }
            case IDR_FRAME: {
                std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
                uint64_t pts = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
                auto mediaData = dispatcher->RequestDataBuffer(MEDIA_TYPE_VIDEO, len);
                mediaData->mediaType = MEDIA_TYPE_VIDEO;
                mediaData->isRaw = false;
                mediaData->keyFrame = keyFrame;
                mediaData->pts = pts;
                SHARING_LOGD("[%{public}" PRId64 "] capture a video into dispatcher:%{public}u, len:%{public}u.",
                             mediaData->pts, dispatcher->GetDispatcherId(), len);
                mediaData->buff = move(frame);
                dispatcher->InputData(mediaData);
                break;
            }
            default:
                break;
        }
    }
}

ScreenCaptureConsumer::ScreenCaptureConsumer()
{
    SHARING_LOGD("capture consumer Id: %{public}u.", GetId());
}

ScreenCaptureConsumer::~ScreenCaptureConsumer()
{
    SHARING_LOGD("~ capture consumer Id: %{public}u.", GetId());
    Release();
}

void ScreenCaptureConsumer::HandleProsumerInitState(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    auto pPrivateMsg = std::make_shared<ScreenCaptureSessionEventMsg>();
    pPrivateMsg->errorCode = ERR_GENERAL_ERROR;

    auto msg = ConvertEventMsg<ScreenCaptureConsumerEventMsg>(event);
    if (msg) {
        SHARING_LOGD("msg convert suc, vcodecId:%{public}d.", msg->videoTrack.codecId);
        if (msg->audioTrack.codecId != CodecId::CODEC_NONE) {
            audioTrack_ = msg->audioTrack;
        }

        if (msg->videoTrack.codecId != CodecId::CODEC_NONE) {
            videoTrack_ = msg->videoTrack;
        }

        if (Init(msg->screenId)) {
            pPrivateMsg->errorCode = ERR_OK;
        }
    } else {
        SHARING_LOGE("msg convert fail!");
        return;
    }

    pPrivateMsg->type = EVENT_AGENT_STATE_PROSUMER_INIT;
    pPrivateMsg->toMgr = ModuleType::MODULE_CONTEXT;
    pPrivateMsg->fromMgr = ModuleType::MODULE_MEDIACHANNEL;
    pPrivateMsg->srcId = GetId();
    pPrivateMsg->dstId = event.eventMsg->srcId;
    pPrivateMsg->agentId = GetSinkAgentId();
    pPrivateMsg->prosumerId = GetId();
    pPrivateMsg->requestId = event.eventMsg->requestId;

    NotifyPrivateEvent(pPrivateMsg);
}

int32_t ScreenCaptureConsumer::HandleEvent(SharingEvent &event)
{
    SHARING_LOGD("trace.");
    RETURN_INVALID_IF_NULL(event.eventMsg);

    SHARING_LOGD("eventType: %{public}s = %{public}d, capture consumerId: %{public}u.",
                 std::string(magic_enum::enum_name(event.eventMsg->type)).c_str(), event.eventMsg->type, GetId());
    switch (event.eventMsg->type) {
        case EventType::EVENT_SCREEN_CAPTURE_INIT:
            HandleProsumerInitState(event);
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }

    return 0;
}

void ScreenCaptureConsumer::UpdateOperation(ProsumerStatusMsg::Ptr &statusMsg)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(statusMsg);

    SHARING_LOGD("status: %{public}s consumerId: %{public}u.",
                 std::string(magic_enum::enum_name(static_cast<ProsumerOptRunningStatus>(statusMsg->status))).c_str(),
                 GetId());
    switch (statusMsg->status) {
        case ProsumerOptRunningStatus::PROSUMER_INIT:
            statusMsg->status = PROSUMER_NOTIFY_INIT_SUCCESS;
            break;
        case ProsumerOptRunningStatus::PROSUMER_START: {
            paused_ = false;
            if (isInit_ && StartCapture()) {
                statusMsg->status = PROSUMER_NOTIFY_START_SUCCESS;
            } else {
                statusMsg->status = PROSUMER_NOTIFY_ERROR;
                statusMsg->errorCode = ERR_PROSUMER_START;
            }
            break;
        }
        case ProsumerOptRunningStatus::PROSUMER_PAUSE: {
            paused_ = true;
            statusMsg->status = PROSUMER_NOTIFY_PAUSE_SUCCESS;
            break;
        }
        case ProsumerOptRunningStatus::PROSUMER_RESUME: {
            paused_ = false;
            statusMsg->status = PROSUMER_NOTIFY_RESUME_SUCCESS;
            break;
        }
        case ProsumerOptRunningStatus::PROSUMER_STOP: {
            if (StopCapture()) {
                statusMsg->status = PROSUMER_NOTIFY_STOP_SUCCESS;
            } else {
                statusMsg->status = PROSUMER_NOTIFY_ERROR;
                statusMsg->errorCode = ERR_PROSUMER_STOP;
            }
            break;
        }
        case ProsumerOptRunningStatus::PROSUMER_DESTROY:
            Release();
            SHARING_LOGD("prosumer release success, out.");
            statusMsg->status = PROSUMER_NOTIFY_DESTROY_SUCCESS;
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }

    Notify(statusMsg);
}

int32_t ScreenCaptureConsumer::Release()
{
    SHARING_LOGD("trace.");
    StopCapture();

    SHARING_LOGD("consumerId: %{public}u, capture consumer release=out.", GetId());
    return 0;
}

bool ScreenCaptureConsumer::IsPaused()
{
    SHARING_LOGD("trace.");
    return paused_;
}

int32_t ScreenCaptureConsumer::ReleaseScreenBuffer()
{
    SHARING_LOGD("trace.");
    if (videoSourceScreen_ == nullptr) {
        return ERR_GENERAL_ERROR;
    }
    return videoSourceScreen_->ReleaseScreenBuffer();
}

bool ScreenCaptureConsumer::Init(uint64_t screenId)
{
    SHARING_LOGD("capture consumerId: %{public}u, codecId: %{public}d.", GetId(), videoTrack_.codecId);
    std::lock_guard<std::mutex> lock(mutex_);
    if ((videoTrack_.codecId != CODEC_NONE) && !InitVideoCapture(screenId)) {
        SHARING_LOGE("capture consumerId: %{public}u, init video capture failed.", GetId());
        return false;
    }

    SHARING_LOGD("capture consumerId: %{public}u, success.", GetId());
    isInit_ = true;
    return true;
}

bool ScreenCaptureConsumer::InitVideoCapture(uint64_t screenId)
{
    SHARING_LOGD("consumerId: %{public}u.", GetId());
    VideoSourceConfigure config;
    config.srcScreenId_ = screenId;

    videoSourceEncoder_ = std::make_shared<VideoSourceEncoder>(shared_from_this());
    if (!videoSourceEncoder_->InitEncoder(config)) {
        SHARING_LOGE("InitEncoder failed! %{public}u.", GetId());
        OnInitVideoCaptureError();
        return false;
    }
    videoSourceScreen_ = std::make_shared<VideoSourceScreen>(videoSourceEncoder_->GetEncoderSurface());
    videoSourceScreen_->InitScreenSource(config);
    return true;
}

bool ScreenCaptureConsumer::InitAudioCapture() const
{
    SHARING_LOGD("trace.");
    return true;
}

bool ScreenCaptureConsumer::StartCapture()
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(mutex_);
    if (isRunning_ || !isInit_) {
        return false;
    }

    isRunning_ = true;
    if ((videoTrack_.codecId != CODEC_NONE) && !StartVideoCapture()) {
        SHARING_LOGE("consumerId: %{public}u, start video capture failed.", GetId());
        isRunning_ = false;
        return false;
    }

    SHARING_LOGD("consumerId: %{public}u, success.", GetId());
    return true;
}

bool ScreenCaptureConsumer::StartVideoCapture()
{
    SHARING_LOGD("trace.");
    if (videoSourceScreen_ != nullptr) {
        videoSourceScreen_->StartScreenSourceCapture();
    }
    if (videoSourceEncoder_ != nullptr) {
        videoSourceEncoder_->StartEncoder();
    }
    return true;
}

bool ScreenCaptureConsumer::StopCapture()
{
    SHARING_LOGD("stop capture consumer, consumerId: %{public}u.", GetId());
    std::lock_guard<std::mutex> lock(mutex_);
    if (!isRunning_ || !isInit_) {
        SHARING_LOGD("stop capture consumer already stoped, consumerId: %{public}u.", GetId());
        return true;
    }

    isRunning_ = false;

    if (videoTrack_.codecId != CODEC_NONE) {
        if (videoSourceEncoder_ != nullptr) {
            videoSourceEncoder_->StopEncoder();
        }
        if (videoSourceScreen_ != nullptr) {
            videoSourceScreen_->StopScreenSourceCapture();
        }
    }

    return true;
}

void ScreenCaptureConsumer::OnInitVideoCaptureError()
{
    SHARING_LOGD("trace.");
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    if (audioTrack_.codecId == CODEC_NONE) {
        statusMsg->status = PROSUMER_NOTIFY_ERROR;
    }

    Notify(statusMsg);
}

REGISTER_CLASS_REFLECTOR(ScreenCaptureConsumer);
} // namespace Sharing
} // namespace OHOS

/*
 * Copyright (c) 2023-2024 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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
static std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
void ScreenCaptureConsumer::AudioEncoderReceiver::OnFrame(const Frame::Ptr &frame)
{
    MEDIA_LOGD("trace.");

    auto parent = parent_.lock();
    if (parent && !parent->listener_.expired()) {
        MEDIA_LOGD("consumerId: %{public}u.", parent->GetId());
        auto listener = parent->listener_.lock();
        auto dispatcher = listener->GetDispatcher();
        if (dispatcher) {
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
            uint64_t pts = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
            auto mediaData = dispatcher->RequestDataBuffer(MEDIA_TYPE_AUDIO, frame->Size());
            mediaData->mediaType = MEDIA_TYPE_AUDIO;
            mediaData->isRaw = false;
            mediaData->keyFrame = false;
            mediaData->pts = pts;
            MEDIA_LOGD("recv audio encode data: %{public}p size: %{public}d & put it into dispatcher: %{public}u.",
                       frame->Data(), frame->Size(), dispatcher->GetDispatcherId());
            mediaData->buff = move(frame);
            dispatcher->InputData(mediaData);
        }
    }
}

void ScreenCaptureConsumer::OnFrameBufferUsed()
{
    ReleaseScreenBuffer();
}

void ScreenCaptureConsumer::HandleSpsFrame(BufferDispatcher::Ptr dispatcher, const Frame::Ptr &frame)
{
    RETURN_IF_NULL(dispatcher);
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
    RETURN_IF_NULL(dispatcher);
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
                if (mediaData == nullptr) {
                    SHARING_LOGE("RequestDataBuffer get null!");
                    break;
                }
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
    SHARING_LOGD("capture consumerId: %{public}u, viddeo codecId: %{public}d, audio codecId: %{public}d.", GetId(),
                 videoTrack_.codecId, audioTrack_.codecId);
    std::lock_guard<std::mutex> lock(mutex_);
    if (audioTrack_.codecId != CODEC_NONE) {
        if (!InitAudioEncoder()) {
            SHARING_LOGE("capture consumerId: %{public}u, init audio encoder failed.", GetId());
            return false;
        }

        if (!InitAudioCapture()) {
            SHARING_LOGE("capture consumerId: %{public}u, init audio capture failed.", GetId());
            return false;
        }
    }

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

bool ScreenCaptureConsumer::InitAudioCapture()
{
    SHARING_LOGD("trace.");

    AudioStandard::AudioCapturerOptions options;
    options.capturerInfo.capturerFlags = 0;
    options.capturerInfo.sourceType = AudioStandard::SourceType::SOURCE_TYPE_PLAYBACK_CAPTURE;
    options.streamInfo.channels = AudioStandard::AudioChannel::STEREO;
    options.streamInfo.encoding = AudioStandard::AudioEncodingType::ENCODING_PCM;
    options.streamInfo.format = AudioStandard::AudioSampleFormat::SAMPLE_S16LE;
    options.streamInfo.samplingRate = AudioStandard::AudioSamplingRate::SAMPLE_RATE_48000;

    AudioStandard::AppInfo appInfo;
    appInfo.appUid = 0;

    audioCapturer_ = AudioStandard::AudioCapturer::Create(options, appInfo);
    if (audioCapturer_ == nullptr) {
        SHARING_LOGE("create AudioCapturer failed! consumerId: %{public}u.", GetId());
        return false;
    }

    return true;
}

bool ScreenCaptureConsumer::InitAudioEncoder()
{
    SHARING_LOGD("trace.");
    audioEncoder_ = CodecFactory::CreateAudioEncoder(CODEC_AAC);
    if (!audioEncoder_) {
        SHARING_LOGE("create audio encoder failed.");
        return false;
    }

    audioEncoder_->Init(AUDIO_CHANNEL_STEREO, AUDIO_SAMPLE_BIT_S16LE, AUDIO_SAMPLE_RATE_48000);
    audioEncoderReceiver_ = std::make_shared<AudioEncoderReceiver>(shared_from_this());
    audioEncoder_->AddAudioDestination(audioEncoderReceiver_);
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

    if ((audioTrack_.codecId != CODEC_NONE) && !StartAudioCapture()) {
        SHARING_LOGE("consumerId: %{public}u, start audio capture failed, only viedo works!", GetId());
    }

    SHARING_LOGD("consumerId: %{public}u, success.", GetId());
    return true;
}

bool ScreenCaptureConsumer::StartAudioCapture()
{
    SHARING_LOGD("trace.");
    if (audioCapturer_ == nullptr) {
        SHARING_LOGE("start capture fail - invalid capturer, consumerId: %{public}u.", GetId());
        return false;
    }

    if (!audioCapturer_->Start()) {
        SHARING_LOGE("audioCapturer Start() failed, consumerId: %{public}u.", GetId());
        return false;
    }

    if (audioCapturer_->GetBufferSize(audioBufferLen_) != 0) {
        audioBufferLen_ = 0;
        SHARING_LOGE("audioCapturer GetBufferSize failed, consumerId: %{public}u.", GetId());
        return false;
    }

    audioCaptureThread_ = std::make_unique<std::thread>(&ScreenCaptureConsumer::AudioCaptureThreadWorker, this);

    SHARING_LOGD("consumerId: %{public}u, audio capture start successful.", GetId());
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
        SHARING_LOGD("capture consumer already stoped, consumerId: %{public}u.", GetId());
        return true;
    }

    isRunning_ = false;
    StopAudioCapture();
    StopVideoCapture();

    return true;
}

bool ScreenCaptureConsumer::StopAudioCapture()
{
    SHARING_LOGD("trace.");
    if (audioTrack_.codecId != CODEC_NONE) {
        if (audioCaptureThread_ != nullptr && audioCaptureThread_->joinable()) {
            audioCaptureThread_->join();
            audioCaptureThread_.reset();
            audioCaptureThread_ = nullptr;
        }

        if (audioCapturer_) {
            audioCapturer_->Flush();
            audioCapturer_->Stop();
        }
    }
    return true;
}

bool ScreenCaptureConsumer::StopVideoCapture()
{
    SHARING_LOGD("trace.");
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

void ScreenCaptureConsumer::AudioCaptureThreadWorker()
{
    SHARING_LOGD("trace.");
    int32_t bytesRead = 0;
    bool isBlockingRead = true;

    auto pcmFrame = FrameImpl::Create();
    pcmFrame->codecId_ = CODEC_PCM;

    uint8_t *frame = (uint8_t *)malloc(audioBufferLen_);
    SHARING_LOGI("audio capture buffer size: %{public}zu.", audioBufferLen_);
    if (!frame) {
        SHARING_LOGE("malloc buffer failed.");
        return;
    }

    while (isRunning_) {
        SHARING_LOGD("audio capture thread get pcm data, size: %{public}u.", bytesRead);
        bytesRead = audioCapturer_->Read(*frame, audioBufferLen_, isBlockingRead);
        if (bytesRead > 0 && audioEncoder_) {
            pcmFrame->Clear();
            pcmFrame->Assign((char *)frame, static_cast<uint32_t>(bytesRead));
            audioEncoder_->OnFrame(pcmFrame);
        }
    }

    if (frame) {
        free(frame);
    }

    SHARING_LOGD("audio capture thread exit.");
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

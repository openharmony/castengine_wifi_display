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

#include "video_play_controller.h"
#include <chrono>
#include <securec.h>
#include "avcodec_errors.h"
#include "common/common_macro.h"
#include "common/const_def.h"
#include "common/event_channel.h"
#include "common/media_log.h"
#include "configuration/include/config.h"
#include "magic_enum.hpp"
#include "media_controller.h"
#include "protocol/frame/h264_frame.h"
#include "surface.h"
#include "utils/data_buffer.h"

using namespace OHOS::MediaAVCodec;

namespace OHOS {
namespace Sharing {

VideoPlayController::VideoPlayController(uint32_t mediaChannelId)
{
    SHARING_LOGD("trace.");
    bufferReceiver_ = std::make_shared<BufferReceiver>();
    mediachannelId_ = mediaChannelId;
}

VideoPlayController::~VideoPlayController()
{
    SHARING_LOGD("delete video play controller, mediachannelId: %{public}u.", mediachannelId_);
}

bool VideoPlayController::Init(VideoTrack &videoTrack)
{
    SHARING_LOGD("trace.");
    if (CODEC_NONE == videoTrack.codecId) {
        SHARING_LOGW("no need to play.");
        return false;
    }
    videoTrack_ = videoTrack;
    SHARING_LOGI("videoWidth: %{public}d, videoHeight: %{public}d.", videoTrack.width, videoTrack.height);

    SharingValue::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("codec", "forceSWDecoder", "isEnable", values);
    if (ret == CONFIGURE_ERROR_NONE) {
        bool value;
        values->GetValue<bool>(value);
        forceSWDecoder_ = static_cast<bool>(value);
    }

    videoSinkDecoder_ = std::make_shared<VideoSinkDecoder>(mediachannelId_, forceSWDecoder_);
    if (!videoSinkDecoder_->Init(videoTrack.codecId) || !videoSinkDecoder_->SetDecoderFormat(videoTrack)) {
        SHARING_LOGE("video play init error.");
        return false;
    }

    videoSinkDecoder_->SetVideoDecoderListener(shared_from_this());
    bufferReceiver_->SetBufferReceiverListener(shared_from_this());
    return true;
}

bool VideoPlayController::SetSurface(sptr<Surface> surface, bool keyFrame)
{
    SHARING_LOGD("trace.");
    RETURN_FALSE_IF_NULL(surface);
    if (isVideoRunning_) {
        SHARING_LOGE("video play is running, cann't set surface.");
        return false;
    }

    if (videoSinkDecoder_ == nullptr) {
        SHARING_LOGE("cann't set surface if the decoder not init.");
        return false;
    }

    if (forceSWDecoder_) {
        if ((nullptr != videoSinkDecoder_)) {
            bool isVaild = true;
            if (isSurfaceNoCopy_) {
                isVaild = videoSinkDecoder_->SetSurface(surface);
            }
            if (isVaild) {
                enableSurface_ = true;
                surface_ = surface;
                SHARING_LOGD("set surface success.");
                return true;
            } else {
                SHARING_LOGD("set surface failed.");
                return false;
            }
        }
    } else {
        if ((nullptr != videoSinkDecoder_) && (videoSinkDecoder_->SetSurface(surface))) {
            surface_ = surface;
            isKeyMode_ = keyFrame;
            enableSurface_ = true;
            SHARING_LOGD("set surface success.");
            return true;
        }
    }

    return false;
}

bool VideoPlayController::Start(BufferDispatcher::Ptr &dispatcher)
{
    SHARING_LOGD("trace.");
    if (isVideoRunning_) {
        SHARING_LOGW("video play is running.");
        return true;
    }

    if (enableSurface_ && (nullptr != videoSinkDecoder_)) {
        if (videoSinkDecoder_->Start()) {
            isVideoRunning_ = true;
            dispatcher->AttachReceiver(bufferReceiver_);
            SetKeyMode(isKeyMode_);
            StartVideoThread();
            return true;
        }
    }
    SHARING_LOGE("video play start failed, mediachannelId: %{public}u!", mediachannelId_);
    return false;
}

void VideoPlayController::Stop(BufferDispatcher::Ptr &dispatcher)
{
    SHARING_LOGD("trace.");
    RETURN_IF_NULL(dispatcher);
    SHARING_LOGI("video play stop, mediachannelId: %{public}u.", mediachannelId_);
    if (bufferReceiver_) {
        dispatcher->DetachReceiver(bufferReceiver_);
    }

    if (isVideoRunning_ && (nullptr != videoSinkDecoder_)) {
        videoSinkDecoder_->Stop();
        isVideoRunning_ = false;
        if (bufferReceiver_) {
            bufferReceiver_->NotifyReadStop();
        }
        StopVideoThread();
    }

    SHARING_LOGI("media play stop done, mediachannelId: %{public}u.", mediachannelId_);
}

void VideoPlayController::Release()
{
    SHARING_LOGD("trace.");
    if (nullptr != videoSinkDecoder_) {
        videoSinkDecoder_->Release();
        videoSinkDecoder_.reset();
    }
}

void VideoPlayController::StartVideoThread()
{
    SHARING_LOGD("trace.");
    if (videoPlayThread_ != nullptr) {
        SHARING_LOGD("play start thread already exist, mediachannelId: %{public}u.", mediachannelId_);
        return;
    }

    videoPlayThread_ = std::make_shared<std::thread>(&VideoPlayController::VideoPlayThread, this);
    std::string name = "videoplay";
    pthread_setname_np(videoPlayThread_->native_handle(), name.c_str());
    if (videoPlayThread_ == nullptr) {
        SHARING_LOGE("play start create thread error, mediachannelId: %{public}u!", mediachannelId_);
    }
}

void VideoPlayController::StopVideoThread()
{
    SHARING_LOGD("trace.");
    if (videoPlayThread_) {
        if (videoPlayThread_->joinable()) {
            videoPlayThread_->join();
        }
        videoPlayThread_ = nullptr;
    }
    SHARING_LOGD("stop video thread exit, mediachannelId: %{public}u.", mediachannelId_);
}

void VideoPlayController::VideoPlayThread()
{
    SHARING_LOGD("video play thread start mediaChannelId: %{public}u tid: %{public}d.", mediachannelId_, gettid());
    while (isVideoRunning_) {
        MediaData::Ptr outData = std::make_shared<MediaData>();
        outData->buff = std::make_shared<DataBuffer>();
        int32_t ret = 0;
        if (bufferReceiver_) {
            ret = bufferReceiver_->RequestRead(MediaType::MEDIA_TYPE_VIDEO, [&outData](const MediaData::Ptr &data) {
                if (data == nullptr) {
                    return;
                }
                outData->buff->ReplaceData(data->buff->Peek(), data->buff->Size());
                outData->keyFrame = data->keyFrame;
                outData->mediaType = data->mediaType;
                outData->pts = data->pts;
                outData->isRaw = data->isRaw;
                outData->ssrc = data->ssrc;
            });
        }
        if (ret != 0) {
            continue;
        }
        if (videoSinkDecoder_) {
            if (outData->keyFrame) {
                MEDIA_LOGD("get key frame.");
                auto sps = bufferReceiver_->GetSPS();
                if (sps != nullptr && sps->buff != nullptr) {
                    MEDIA_LOGD("get sps from dispatcher.");
                    ProcessVideoData(sps->buff->Peek(), sps->buff->Size());
                }
                auto pps = bufferReceiver_->GetPPS();
                if (pps != nullptr && pps->buff != nullptr) {
                    MEDIA_LOGD("get pps from dispatcher.");
                    ProcessVideoData(pps->buff->Peek(), pps->buff->Size());
                }
            }
            MEDIA_LOGD("process video data, size: %{public}d, keyFrame: %{public}d.", outData->buff->Size(),
                       outData->keyFrame);
            ProcessVideoData(outData->buff->Peek(), outData->buff->Size());
        }
    }

    SHARING_LOGD("play thread exit, mediachannelId: %{public}u tid: %{public}d.", mediachannelId_, gettid());
}

void VideoPlayController::ProcessVideoData(const char *data, int32_t size)
{
    MEDIA_LOGD("trace.");
    if (data == nullptr || size <= 0) {
        SHARING_LOGD("data is null, mediachannelId: %{public}u.", mediachannelId_);
        return;
    }

    {
        std::unique_lock<std::mutex> lock(videoSinkDecoder_->inMutex_);
        if (videoSinkDecoder_->inQueue_.empty()) {
            while (isVideoRunning_) {
                SHARING_LOGD("try wait, mediachannelId: %{public}u.", mediachannelId_);
                videoSinkDecoder_->inCond_.wait_for(lock, std::chrono::milliseconds(DECODE_WAIT_MILLISECONDS),
                                                    [this]() { return (!videoSinkDecoder_->inQueue_.empty()); });

                if (videoSinkDecoder_->inQueue_.empty()) {
                    SHARING_LOGD("index queue empty, mediachannelId: %{public}u.", mediachannelId_);
                    continue;
                }
                break;
            }
        }
    }

    if (!isVideoRunning_) {
        SHARING_LOGD("stop return, mediachannelId: %{public}u.", mediachannelId_);
        return;
    }

    bool ret = videoSinkDecoder_->DecodeVideoData(data, size);
    if (ret == false) {
        SHARING_LOGE("sink decode data failed.");
    }
}

void VideoPlayController::OnVideoDataDecoded(DataBuffer::Ptr decodedData)
{
    MEDIA_LOGD("trace.");
    if (forceSWDecoder_) {
        if (!isSurfaceNoCopy_) {
            RenderInCopyMode(decodedData);
        }
    }
}

void VideoPlayController::OnError(int32_t errorCode)
{
    SHARING_LOGW("trace mediachannelId: %{public}u.", mediachannelId_);
    auto mediaController = mediaController_.lock();
    RETURN_IF_NULL(mediaController);

    auto msg = std::make_shared<EventMsg>();
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = msg;
    statusMsg->errorCode = ERR_OK;
    if (surface_) {
        statusMsg->surfaceId = surface_->GetUniqueId();
    }

    switch (errorCode) {
        case MediaAVCodec::AVCS_ERR_SERVICE_DIED:
            SHARING_LOGE("media service died: %{public}u.", mediachannelId_);
            statusMsg->status = CONNTROLLER_NOTIFY_DECODER_DIED;
            statusMsg->errorCode = ERR_SURFACE_FAILURE;
            break;
        default:
            break;
    }

    mediaController->OnPlayControllerNotify(statusMsg);
}

int32_t VideoPlayController::RenderInCopyMode(DataBuffer::Ptr decodedData)
{
    SHARING_LOGD("Render begin.");
    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence = -1;
    int32_t renderWidth = videoTrack_.width == 0 ? DEFAULT_VIDEO_WIDTH : videoTrack_.width;
    int32_t renderHeight = videoTrack_.height == 0 ? DEFAULT_CAPTURE_VIDEO_HEIGHT : videoTrack_.height;

    BufferRequestConfig requestConfig = {
        .width = renderWidth,
        .height = renderHeight,
        .strideAlignment = 8,
        // .format = PIXEL_FMT_RGBA_8888,
        // .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .format = GRAPHIC_PIXEL_FMT_YCRCB_420_SP,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };

    if (surface_ == nullptr) {
        return -1;
    }

    SurfaceError error = surface_->RequestBuffer(buffer, releaseFence, requestConfig);
    if (error != SURFACE_ERROR_OK) {
        return -1;
    }

    void *bufferVirAddr = buffer->GetVirAddr();
    SHARING_LOGD("buffer size is %{public}d.", decodedData->Size());
    if (firstFrame_ == true) {
        firstFrame_ = false;
        SHARING_LOGD("first frame.");
    }

    uint32_t dataSize = renderWidth * renderHeight * 3 / 2;
    auto ret = memcpy_s(bufferVirAddr, dataSize, decodedData->Peek(), dataSize);
    if (ret != EOK) {
        SHARING_LOGE("copy data failed !");
        return -1;
    }

    if (bufferVirAddr == nullptr) {
        SHARING_LOGD("bufferVirAddr is nullptr.");
        return -1;
    }

    BufferFlushConfig flushConfig = {
        .damage =
            {
                .x = 0,
                .y = 0,
                .w = renderWidth,
                .h = renderHeight,
            },
        .timestamp = 0,
    };

    surface_->FlushBuffer(buffer, -1, flushConfig);
    SHARING_LOGD("Render End.");
    return 0;
}

void VideoPlayController::OnAccelerationDoneNotify()
{
    SHARING_LOGW("mediachannelId: %{public}u!", mediachannelId_);
    auto mediaController = mediaController_.lock();
    RETURN_IF_NULL(mediaController);

    auto msg = std::make_shared<EventMsg>();
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = msg;
    statusMsg->errorCode = ERR_DECODE_DISABLE_ACCELERATION;
    statusMsg->status = CONNTROLLER_NOTIFY_ACCELERATION;

    if (surface_) {
        statusMsg->surfaceId = surface_->GetUniqueId();
    }

    mediaController->OnPlayControllerNotify(statusMsg);
}

void VideoPlayController::OnKeyModeNotify(bool enable)
{
    SHARING_LOGW("enable: %{public}d, mediachannelId: %{public}u!", enable, mediachannelId_);
    auto mediaController = mediaController_.lock();
    RETURN_IF_NULL(mediaController);

    auto msg = std::make_shared<EventMsg>();
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->eventMsg = msg;
    statusMsg->errorCode = ERR_OK;

    if (surface_) {
        statusMsg->surfaceId = surface_->GetUniqueId();
    }

    if (enable) {
        statusMsg->status = CONNTROLLER_NOTIFY_KEYMOD_START;
    } else {
        statusMsg->status = CONNTROLLER_NOTIFY_KEYMOD_STOP;
    }

    mediaController->OnPlayControllerNotify(statusMsg);
}

} // namespace Sharing
} // namespace OHOS
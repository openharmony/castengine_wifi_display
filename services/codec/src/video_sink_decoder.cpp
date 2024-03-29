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

#include "video_sink_decoder.h"
#include <securec.h>
#include "avcodec_codec_name.h"
#include "avcodec_errors.h"
#include "avcodec_mime_type.h"
#include "buffer/avsharedmemory.h"
#include "common/common_macro.h"
#include "common/media_log.h"
#include "configuration/include/config.h"
#include "utils/utils.h"

namespace OHOS {
namespace Sharing {
VideoSinkDecoder::VideoSinkDecoder(uint32_t controlId, bool forceSWDecoder)
{
    SHARING_LOGD("trace.");
    controlId_ = controlId;
    forceSWDecoder_ = forceSWDecoder;
}

VideoSinkDecoder::~VideoSinkDecoder()
{
    SHARING_LOGD("trace.");
    Release();
}

bool VideoSinkDecoder::Init(CodecId videoCodecId)
{
    SHARING_LOGD("trace.");
    videoCodecId_ = videoCodecId;
    return InitDecoder() && SetVideoCallback();
}

bool VideoSinkDecoder::InitDecoder()
{
    SHARING_LOGD("trace.");
    if (videoDecoder_ != nullptr) {
        SHARING_LOGD("start video decoder already init.");
        return true;
    }
    if (forceSWDecoder_) {
        SHARING_LOGD("begin create software video decoder.");
        videoDecoder_ = OHOS::MediaAVCodec::VideoDecoderFactory::CreateByName(
            (MediaAVCodec::AVCodecCodecName::VIDEO_DECODER_AVC_NAME).data());
    } else {
        SHARING_LOGD("begin create hardware video decoder.");
        videoDecoder_ = OHOS::MediaAVCodec::VideoDecoderFactory::CreateByMime(
            (MediaAVCodec::AVCodecMimeType::MEDIA_MIMETYPE_VIDEO_AVC).data());
    }

    if (videoDecoder_ == nullptr) {
        SHARING_LOGE("create video decoder failed!");
        return false;
    }
    SHARING_LOGD("init video decoder success.");
    return true;
}

bool VideoSinkDecoder::SetDecoderFormat(const VideoTrack &track)
{
    SHARING_LOGD("trace.");
    RETURN_FALSE_IF_NULL(videoDecoder_);
    videoTrack_ = track;
    MediaAVCodec::Format format;
    format.PutIntValue("width", track.width);
    format.PutIntValue("height", track.height);

    auto ret = videoDecoder_->Configure(format);
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        SHARING_LOGE("configure decoder format param failed!");
        return false;
    }
    SHARING_LOGD("configure video decoder format success.");
    return true;
}

bool VideoSinkDecoder::SetVideoCallback()
{
    SHARING_LOGD("trace.");
    auto ret = videoDecoder_->SetCallback(shared_from_this());
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        SHARING_LOGE("set video decoder callback failed!");
        return false;
    }
    SHARING_LOGD("set video decoder callback success.");
    return true;
}

void VideoSinkDecoder::SetVideoDecoderListener(VideoSinkDecoderListener::Ptr listener)
{
    SHARING_LOGD("trace.");
    videoDecoderListener_ = listener;
}

bool VideoSinkDecoder::Start()
{
    SHARING_LOGD("trace.");
    if (isRunning_) {
        MEDIA_LOGD("decoder is running!");
        return true;
    }
    if (StartDecoder()) {
        isRunning_ = true;
        return true;
    } else {
        return false;
    }
}

void VideoSinkDecoder::Stop()
{
    SHARING_LOGD("trace %{public}p.", this);
    if (!isRunning_) {
        SHARING_LOGD("decoder is not running.");
        return;
    }

    if (StopDecoder()) {
        SHARING_LOGD("stop success.");
        isRunning_ = false;
        std::lock_guard<std::mutex> lock(inMutex_);
        std::queue<int32_t> temp;
        std::swap(temp, inQueue_);
    }
}

void VideoSinkDecoder::Release()
{
    SHARING_LOGD("trace.");
    if (videoDecoder_ != nullptr) {
        videoDecoder_->Release();
        videoDecoder_.reset();
    }
}

bool VideoSinkDecoder::StartDecoder()
{
    SHARING_LOGD("trace.");
    RETURN_FALSE_IF_NULL(videoDecoder_);
    auto ret = videoDecoder_->Prepare();
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        SHARING_LOGE("prepare decoder failed!");
        return false;
    }

    ret = videoDecoder_->Start();
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        SHARING_LOGE("start decoder failed!");
        return false;
    }
    SHARING_LOGD("start video decoder success.");
    return true;
}

bool VideoSinkDecoder::StopDecoder()
{
    SHARING_LOGD("trace.");
    if (videoDecoder_ == nullptr) {
        SHARING_LOGD("StopDecoder no need.");
        return true;
    }

    SHARING_LOGD("before Stop.");
    auto ret = videoDecoder_->Stop();
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        SHARING_LOGE("stop decoder failed!");
        return false;
    }

    SHARING_LOGD("before Reset.");
    ret = videoDecoder_->Reset();
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        SHARING_LOGE("Reset decoder failed!");
        return false;
    }

    SHARING_LOGD("try set decoder formt for next play.");
    if (SetDecoderFormat(videoTrack_)) {
        if (enableSurface_ && (nullptr != surface_)) {
            videoDecoder_->SetOutputSurface(surface_);
        }
    } else {
        SHARING_LOGE("set decoder formt failed!");
        return false;
    }

    SHARING_LOGD("StopDecoder success.");
    return true;
}

bool VideoSinkDecoder::DecodeVideoData(const char *data, int32_t size)
{
    MEDIA_LOGD("decode data controlId: %{public}u.", controlId_);
    RETURN_FALSE_IF_NULL(videoDecoder_);

    std::unique_lock<std::mutex> lock(inMutex_);
    auto inputIndex = inQueue_.front();
    MEDIA_LOGD("inQueue front: %{public}d.", inputIndex);
    auto inputBuffer = inBufferQueue_.front();
    if (inputBuffer == nullptr) {
        MEDIA_LOGE("GetInputBuffer failed controlId: %{public}u.", controlId_);
        return false;
    }
    lock.unlock();
    MEDIA_LOGD("try copy data dest size: %{public}d data size: %{public}d.", inputBuffer->GetSize(), size);

    auto ret = memcpy_s(inputBuffer->GetBase(), inputBuffer->GetSize(), data, size);
    if (ret != EOK) {
        MEDIA_LOGE("copy data failed controlId: %{public}u.", controlId_);
        return false;
    }

    MediaAVCodec::AVCodecBufferInfo bufferInfo;
    bufferInfo.presentationTimeUs = 0;
    bufferInfo.size = size;
    bufferInfo.offset = 0;

    auto p = data;
    p = *(p + 2) == 0x01 ? p + 3 : p + 4; // 2: offset, 3: offset, 4: offset
    if ((p[0] & 0x1f) == 0x06 || (p[0] & 0x1f) == 0x07 || (p[0] & 0x1f) == 0x08) {
        MEDIA_LOGD("media flag codec data controlId: %{public}u.", controlId_);
        ret = videoDecoder_->QueueInputBuffer(inputIndex, bufferInfo, MediaAVCodec::AVCODEC_BUFFER_FLAG_CODEC_DATA);
    } else {
        MEDIA_LOGD("media flag none controlId: %{public}u.", controlId_);
        ret = videoDecoder_->QueueInputBuffer(inputIndex, bufferInfo, MediaAVCodec::AVCODEC_BUFFER_FLAG_NONE);
    }

    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        MEDIA_LOGE("QueueInputBuffer failed error: %{public}d controlId: %{public}u.", ret, controlId_);
        return false;
    }
    
    lock.lock();
    inQueue_.pop();
    inBufferQueue_.pop();

    MEDIA_LOGD("process data success controlId: %{public}u.", controlId_);
    return true;
}

void VideoSinkDecoder::OnError(MediaAVCodec::AVCodecErrorType errorType, int32_t errorCode)
{
    SHARING_LOGE("controlId: %{public}u.", controlId_);
    auto listener = videoDecoderListener_.lock();
    if (listener) {
        listener->OnError(errorCode);
    }
}

void VideoSinkDecoder::OnOutputBufferAvailable(uint32_t index, MediaAVCodec::AVCodecBufferInfo info,
    MediaAVCodec::AVCodecBufferFlag flag, std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer)
{
    MEDIA_LOGD("OnOutputBufferAvailable index: %{public}u controlId: %{public}u.", index, controlId_);
    if (!videoDecoder_) {
        MEDIA_LOGW("decoder is null!");
        return;
    }
    if (forceSWDecoder_) {
        MEDIA_LOGD("forceSWDecoder_ is true.");
    }
    if (forceSWDecoder_) {
        if (buffer == nullptr || buffer->GetBase() == nullptr) {
            MEDIA_LOGW("OnOutputBufferAvailable buffer null!");
            return;
        }
        size_t dataSize = static_cast<size_t>(info.size);
        SHARING_LOGD("OnOutputBufferAvailable size: %{public}zu.", dataSize);
        auto dataBuf = std::make_shared<DataBuffer>(dataSize);
        if (dataBuf != nullptr) {
            dataBuf->PushData((char *)buffer->GetBase(), dataSize);
            auto listerner = videoDecoderListener_.lock();
            if (listerner) {
                listerner->OnVideoDataDecoded(dataBuf);
            }
        } else {
            MEDIA_LOGE("get databuffer failed!");
        }
    }

    if (videoDecoder_->ReleaseOutputBuffer(index, true) != MediaAVCodec::AVCS_ERR_OK) {
        MEDIA_LOGW("ReleaseOutputBuffer failed!");
    }
}

void VideoSinkDecoder::OnInputBufferAvailable(uint32_t index, std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer)
{
    MEDIA_LOGD("OnInputBufferAvailable index: %{public}u controlId: %{public}u.", index, controlId_);
    {
        std::lock_guard<std::mutex> lock(inMutex_);
        inQueue_.push(index);
        inBufferQueue_.push(buffer);
    }
    inCond_.notify_all();
    MEDIA_LOGD("OnInputBufferAvailable notify.");
}

void VideoSinkDecoder::OnOutputFormatChanged(const MediaAVCodec::Format &format)
{
    SHARING_LOGD("controlId: %{public}u.", controlId_);
}

bool VideoSinkDecoder::SetSurface(sptr<Surface> surface)
{
    SHARING_LOGD("trace.");
    RETURN_FALSE_IF_NULL(surface);
    RETURN_FALSE_IF_NULL(videoDecoder_);
    if (isRunning_) {
        SHARING_LOGW("decoder is running, cann't set surface!");
        return false;
    }

    auto ret = videoDecoder_->SetOutputSurface(surface);
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        SHARING_LOGE("decoder set surface error, ret code:  %{public}u.", ret);
        return false;
    }
    enableSurface_ = true;
    surface_ = surface;
    return true;
}

} // namespace Sharing
} // namespace OHOS
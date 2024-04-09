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

#include "video_source_encoder.h"
#include "avcodec_errors.h"
#include "buffer/avsharedmemory.h"
#include "common/common_macro.h"
#include "protocol/frame/h264_frame.h"
#include "sharing_log.h"

namespace OHOS {
namespace Sharing {

void VideoSourceEncoder::VideoEncodeCallback::OnError(MediaAVCodec::AVCodecErrorType errorType, int32_t errorCode)
{
    SHARING_LOGD("trace.");
    if (auto parent = parent_.lock()) {
        parent->OnError(errorType, errorCode);
    }
}

void VideoSourceEncoder::VideoEncodeCallback::OnOutputFormatChanged(const MediaAVCodec::Format &format)
{
    SHARING_LOGD("trace.");
    if (auto parent = parent_.lock()) {
        parent->OnOutputFormatChanged(format);
    }
}

void VideoSourceEncoder::VideoEncodeCallback::OnInputBufferAvailable(uint32_t index,
    std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer)
{
    SHARING_LOGD("trace.");
    if (auto parent = parent_.lock()) {
        parent->OnInputBufferAvailable(index, buffer);
    }
}

void VideoSourceEncoder::VideoEncodeCallback::OnOutputBufferAvailable(uint32_t index,
    MediaAVCodec::AVCodecBufferInfo info, MediaAVCodec::AVCodecBufferFlag flag,
    std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer)
{
    SHARING_LOGD("trace.");
    if (auto parent = parent_.lock()) {
        parent->OnOutputBufferAvailable(index, info, flag, buffer);
    }
}

VideoSourceEncoder::~VideoSourceEncoder()
{
    SHARING_LOGD("trace.");
    StopEncoder();
    ReleaseEncoder();
    encoderCb_ = nullptr;
}

bool VideoSourceEncoder::InitEncoder(const VideoSourceConfigure &configure)
{
    SHARING_LOGD("trace.");
    if (!CreateEncoder(configure)) {
        SHARING_LOGE("Create encoder failed!");
        return false;
    }

    if (!ConfigEncoder(configure)) {
        SHARING_LOGE("Config encoder failed!");
        ReleaseEncoder();
        return false;
    }

    videoEncoderSurface_ = videoEncoder_->CreateInputSurface();
    if (videoEncoderSurface_ == nullptr) {
        SHARING_LOGE("Create encoder surface failed!");
        ReleaseEncoder();
        return false;
    }

    return true;
}

bool VideoSourceEncoder::CreateEncoder(const VideoSourceConfigure &configure)
{
    SHARING_LOGD("trace.");
    switch (configure.codecType_) {
        case CodecId::CODEC_H264:
            videoEncoder_ = OHOS::MediaAVCodec::VideoEncoderFactory::CreateByMime("video/avc");
            break;
        case CodecId::CODEC_H265:
            videoEncoder_ = OHOS::MediaAVCodec::VideoEncoderFactory::CreateByMime("video/hevc");
            break;
        default:
            SHARING_LOGE("Encoder codecType is invalid!");
            videoEncoder_ = nullptr;
    }
    if (videoEncoder_ == nullptr) {
        SHARING_LOGE("Create Video Source Encoder failed!");
        return false;
    }
    encoderCb_ = std::make_shared<VideoEncodeCallback>(shared_from_this());
    int32_t ret = videoEncoder_->SetCallback(encoderCb_);
    if (ret != MediaAVCodec::AVCodecServiceErrCode::AVCS_ERR_OK) {
        SHARING_LOGE("Set encoder callback failed!");
        ReleaseEncoder();
        return false;
    }

    return true;
}

bool VideoSourceEncoder::ConfigEncoder(const VideoSourceConfigure &configure)
{
    SHARING_LOGD("trace.");
    if (videoEncoder_ == nullptr) {
        SHARING_LOGE("Encoder is null!");
        return false;
    }
    MediaAVCodec::Format videoFormat;
    switch (configure.codecType_) {
        case CodecId::CODEC_H264:
            videoFormat.PutStringValue("codec_mime", "video/avc");
            break;
        case CodecId::CODEC_H265:
            videoFormat.PutStringValue("codec_mime", "video/hevc");
            break;
        default:
            SHARING_LOGE("Encoder codecType is invalid!");
            return false;
    }
    videoFormat.PutIntValue("pixel_format", configure.pixleFormat_);
    videoFormat.PutLongValue("max_input_size", MAX_YUV420_BUFFER_SIZE);
    videoFormat.PutIntValue("width", configure.videoWidth_);
    videoFormat.PutIntValue("height", configure.videoHeight_);
    videoFormat.PutIntValue("frame_rate", configure.frameRate_);
    videoFormat.PutIntValue("bitrate", SCREEN_CAPTURE_ENCODE_BITRATE);
    int32_t ret = videoEncoder_->Configure(videoFormat);
    if (ret != MediaAVCodec::AVCodecServiceErrCode::AVCS_ERR_OK) {
        SHARING_LOGE("Configure encoder failed!");
        return false;
    }

    return true;
}

sptr<Surface> &VideoSourceEncoder::GetEncoderSurface()
{
    SHARING_LOGD("trace.");
    return videoEncoderSurface_;
}

bool VideoSourceEncoder::StartEncoder()
{
    SHARING_LOGD("trace.");
    if (videoEncoder_ == nullptr) {
        SHARING_LOGE("Encoder is null!");
        return false;
    }

    int32_t ret = videoEncoder_->Prepare();
    if (ret != MediaAVCodec::AVCodecServiceErrCode::AVCS_ERR_OK) {
        SHARING_LOGE("Prepare encoder failed!");
        return false;
    }

    ret = videoEncoder_->Start();
    if (ret != MediaAVCodec::AVCodecServiceErrCode::AVCS_ERR_OK) {
        SHARING_LOGE("Start encoder failed!");
        return false;
    }

    return true;
}

bool VideoSourceEncoder::StopEncoder()
{
    SHARING_LOGD("trace.");
    if (videoEncoder_ == nullptr) {
        SHARING_LOGE("Encoder is null!");
        return false;
    }

    int32_t ret = videoEncoder_->Flush();
    if (ret != MediaAVCodec::AVCodecServiceErrCode::AVCS_ERR_OK) {
        SHARING_LOGE("Flush encoder failed!");
    }

    ret = videoEncoder_->Stop();
    if (ret != MediaAVCodec::AVCodecServiceErrCode::AVCS_ERR_OK) {
        SHARING_LOGE("Stop encoder failed!");
        return false;
    }

    return true;
}

bool VideoSourceEncoder::ReleaseEncoder()
{
    SHARING_LOGD("trace.");
    if (videoEncoder_ == nullptr) {
        SHARING_LOGE("encoder is null!");
        return false;
    }
    int32_t ret = videoEncoder_->Release();
    if (ret != MediaAVCodec::AVCodecServiceErrCode::AVCS_ERR_OK) {
        SHARING_LOGE("Release encoder failed.");
        return false;
    }
    videoEncoder_ = nullptr;

    return true;
}

void VideoSourceEncoder::OnOutputBufferAvailable(uint32_t index, MediaAVCodec::AVCodecBufferInfo info,
                                                 MediaAVCodec::AVCodecBufferFlag flag,
                                                 std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer)
{
    SHARING_LOGI("index: %{public}u, size:%{public}u, pts: %{public}" PRIi64 ".", index, info.size,
                 info.presentationTimeUs);
    if (!videoEncoder_) {
        SHARING_LOGE("encoder is null!");
        return;
    }

    if (buffer == nullptr || buffer->GetBase() == nullptr) {
        SHARING_LOGE("Get output buffer null!");
        return;
    }
    size_t dataSize = static_cast<size_t>(info.size);
    if (dataSize == 0) {
        SHARING_LOGE("params invalid, size: %{public}zu.", dataSize);
        return;
    }

    const char *data = reinterpret_cast<const char *>(buffer->GetBase());
    if (auto listener = listener_.lock()) {
        SplitH264(data, dataSize, 0, [&](const char *buf, size_t len, size_t prefix) {
            if ((*(buf + prefix) & 0x1f) == 0x07) {
                SHARING_LOGD("get sps, size:%{public}zu.", len);
                Frame::Ptr videoFrame = FrameImpl::Create();
                videoFrame->Assign(buf, len);
                listener->OnFrame(videoFrame, SPS_FRAME, false);
                videoFrame = nullptr;
                return;
            }
            if ((*(buf + prefix) & 0x1f) == 0x08) {
                SHARING_LOGD("get pps, size:%{public}zu.", len);
                Frame::Ptr videoFrame = FrameImpl::Create();
                videoFrame->Assign(buf, len);
                listener->OnFrame(videoFrame, PPS_FRAME, false);
                videoFrame = nullptr;
                return;
            }
            SHARING_LOGD("get frame , size:%{public}zu.", len);
            bool keyFrame = (*(buf + prefix) & 0x1f) == 0x05 ? true : false;
            Frame::Ptr videoFrame = FrameImpl::Create();
            videoFrame->Assign(buf, len);
            listener->OnFrame(videoFrame, IDR_FRAME, keyFrame);
            videoFrame = nullptr;
        });
    } else {
        SHARING_LOGE("listener_ is null, call OnFrame failed!");
    }

    if (videoEncoder_->ReleaseOutputBuffer(index) != MediaAVCodec::AVCodecServiceErrCode::AVCS_ERR_OK) {
        SHARING_LOGW("release output buffer failed!");
    }
}


void VideoSourceEncoder::OnInputBufferAvailable(uint32_t index, std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer)
{
    SHARING_LOGI("index:%{public}u.", index);
    if (auto listener = listener_.lock()) {
        listener->OnFrameBufferUsed();
    }
}

void VideoSourceEncoder::OnOutputFormatChanged(const MediaAVCodec::Format &format)
{
    SHARING_LOGD("trace.");
    (void)format;
}

void VideoSourceEncoder::OnError(MediaAVCodec::AVCodecErrorType errorType, int32_t errorCode)
{
    SHARING_LOGD("Encoder error, errorType(%{public}d), errorCode(%{public}d)!", errorType, errorCode);
}
} // namespace Sharing
} // namespace OHOS

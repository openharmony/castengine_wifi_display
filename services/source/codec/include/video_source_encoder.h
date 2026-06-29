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

#ifndef OHOS_SHARING_VIDEO_SOURCE_ENCODER_H
#define OHOS_SHARING_VIDEO_SOURCE_ENCODER_H

#include "avcodec_video_encoder.h"
#include "common/const_def.h"
#include "protocol/frame/frame.h"
#include "utils/data_buffer.h"

namespace OHOS {
namespace Sharing {
enum FRAME_TYPE {
    SPS_FRAME,
    PPS_FRAME,
    IDR_FRAME
};

class VideoSourceConfigure {
public:
    uint32_t screenWidth_ = DEFAULT_VIDEO_WIDTH;
    uint32_t screenHeight_ = DEFAULT_VIDEO_HEIGHT;
    uint32_t videoWidth_ = DEFAULT_VIDEO_WIDTH;
    uint32_t videoHeight_ = DEFAULT_VIDEO_HEIGHT;
    uint32_t frameRate_ = DEFAULT_FRAME_RATE;

    int32_t codecType_ = CodecId::CODEC_H264;
    int32_t pixleFormat_ = static_cast<int32_t>(OHOS::MediaAVCodec::VideoPixelFormat::RGBA);

    uint64_t srcScreenId_ = 0;
};

class VideoSourceEncoderListener {
public:
    virtual ~VideoSourceEncoderListener() = default;

    virtual void OnFrameBufferUsed() = 0;
    virtual void OnFrame(const Frame::Ptr &frame, FRAME_TYPE frameType, bool keyFrame) = 0;
};

class VideoSourceEncoder : public std::enable_shared_from_this<VideoSourceEncoder> {
public:
    class VideoEncodeCallback : public MediaAVCodec::AVCodecCallback {
    public:
        VideoEncodeCallback(std::shared_ptr<VideoSourceEncoder> parent) : parent_(parent) {}
        ~VideoEncodeCallback() = default;

        void OnOutputFormatChanged(const MediaAVCodec::Format &format);
        void OnError(MediaAVCodec::AVCodecErrorType errorType, int32_t errorCode);
        void OnInputBufferAvailable(uint32_t index, std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer);
        void OnOutputBufferAvailable(uint32_t index, MediaAVCodec::AVCodecBufferInfo info,
            MediaAVCodec::AVCodecBufferFlag flag, std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer);
    private:
        std::weak_ptr<VideoSourceEncoder> parent_;
    };

    VideoSourceEncoder(std::shared_ptr<VideoSourceEncoderListener> listener) : listener_(listener){};
    ~VideoSourceEncoder();

    bool StartEncoder();
    bool StopEncoder();
    bool ReleaseEncoder();
    bool InitEncoder(const VideoSourceConfigure &configure);

    sptr<Surface> &GetEncoderSurface();

protected:
    void OnOutputFormatChanged(const MediaAVCodec::Format &format);
    void OnError(MediaAVCodec::AVCodecErrorType errorType, int32_t errorCode);
    void OnInputBufferAvailable(uint32_t index, std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer);
    void OnOutputBufferAvailable(uint32_t index, MediaAVCodec::AVCodecBufferInfo info,
        MediaAVCodec::AVCodecBufferFlag flag, std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer);

private:
    bool CreateEncoder(const VideoSourceConfigure &configure);
    bool ConfigEncoder(const VideoSourceConfigure &configure);

private:
    sptr<OHOS::Surface> videoEncoderSurface_;
    std::shared_ptr<VideoEncodeCallback> encoderCb_;
    std::weak_ptr<VideoSourceEncoderListener> listener_;
    std::shared_ptr<OHOS::MediaAVCodec::AVCodecVideoEncoder> videoEncoder_;
};
} // namespace Sharing
} // namespace OHOS
#endif

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

#ifndef OHOS_SHARING_VIDEO_SINK_DECODER_H
#define OHOS_SHARING_VIDEO_SINK_DECODER_H

#include <condition_variable>
#include <queue>
#include "avcodec_video_decoder.h"
#include "common/const_def.h"
#include "common/event_comm.h"
#include "frame.h"
#include "utils/data_buffer.h"


namespace OHOS {
namespace Sharing {
static constexpr uint32_t DECODE_WAIT_MILLISECONDS = 5000;
class VideoSinkDecoderListener {
public:
    using Ptr = std::shared_ptr<VideoSinkDecoderListener>;
    virtual ~VideoSinkDecoderListener() = default;

    virtual void OnError(int32_t errorCode) = 0;
    virtual void OnVideoDataDecoded(DataBuffer::Ptr decodedData) = 0;
};

class VideoSinkDecoder : public MediaAVCodec::AVCodecCallback,
                         public std::enable_shared_from_this<VideoSinkDecoder> {
public:
    explicit VideoSinkDecoder(uint32_t controlId, bool forceSWDecoder = false);
    virtual ~VideoSinkDecoder();

    bool Start();
    void Stop();
    void Release();
    bool Init(CodecId videoCodecId = CODEC_H264);
    bool DecodeVideoData(const char *data, int32_t size);

    bool SetSurface(sptr<OHOS::Surface> surface);
    bool SetDecoderFormat(const VideoTrack &track);
    void SetVideoDecoderListener(VideoSinkDecoderListener::Ptr listener);

    void OnOutputFormatChanged(const MediaAVCodec::Format &format) override;
    void OnError(MediaAVCodec::AVCodecErrorType errorType, int32_t errorCode) override;
    void OnInputBufferAvailable(uint32_t index, std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer) override;
    void OnOutputBufferAvailable(uint32_t index, MediaAVCodec::AVCodecBufferInfo info,
        MediaAVCodec::AVCodecBufferFlag flag, std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer) override;

private:
    bool InitDecoder();
    bool StopDecoder();
    bool StartDecoder();
    bool SetVideoCallback();

public:
    bool enableSurface_ = false;
    bool forceSWDecoder_ = false;
    uint32_t controlId_ = -1;

    std::queue<int32_t> inQueue_;
    std::queue<std::shared_ptr<MediaAVCodec::AVSharedMemory>> inBufferQueue_;

    std::mutex inMutex_;
    std::condition_variable inCond_;
    std::atomic_bool isRunning_ = false;
    std::weak_ptr<VideoSinkDecoderListener> videoDecoderListener_;
    std::shared_ptr<OHOS::MediaAVCodec::AVCodecVideoDecoder> videoDecoder_ = nullptr;

    VideoTrack videoTrack_;
    CodecId videoCodecId_ = CODEC_NONE;
    sptr<OHOS::Surface> surface_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif
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

#ifndef OHOS_SHARING_TS_RTP_CODEC_H
#define OHOS_SHARING_TS_RTP_CODEC_H

#include <memory>
#include <queue>
#include <thread>
#include "frame/frame.h"
#include "frame/frame_merger.h"
#include "rtp_codec.h"
#include "rtp_maker.h"
extern "C" {
#include <libavformat/avformat.h>
}

namespace OHOS {
namespace Sharing {
class RtpDecoderTs : public RtpDecoder {
public:
    using Ptr = std::shared_ptr<RtpDecoderTs>;

    RtpDecoderTs();
    ~RtpDecoderTs();

    void Release();

    void InputRtp(const RtpPacket::Ptr &rtp) override;
    void SetOnFrame(const OnFrame &cb) override;

private:
    void StartDecoding();
    int ReadPacket(uint8_t *buf, int buf_size);

    static int StaticReadPacket(void *opaque, uint8_t *buf, int buf_size);

private:
    bool exit_ = false;
    int videoStreamIndex_ = -1;
    int audioStreamIndex_ = -1;
    uint8_t *avioCtxBuffer_ = nullptr;

    std::mutex queueMutex_;
    std::queue<RtpPacket::Ptr> dataQueue_;
    std::unique_ptr<std::thread> decodeThread_;

    AVIOContext *avioContext_ = nullptr;
    AVFormatContext *avFormatContext_ = nullptr;
};

class RtpEncoderTs : public RtpEncoder,
                     public RtpMaker {
public:
    using Ptr = std::shared_ptr<RtpEncoderTs>;

    void Release();

    RtpEncoderTs(uint32_t ssrc, uint32_t mtuSize, uint32_t sampleRate, uint8_t payloadType, uint16_t seq = 0);
    ~RtpEncoderTs();

    void InputFrame(const Frame::Ptr &frame) override;
    void SetOnRtpPack(const OnRtpPack &cb) override;

private:
    void StartEncoding();
    void RemoveFrameAfterMuxing();
    int ReadFrame(AVPacket *packet);
    void SaveFrame(Frame::Ptr frame);
    static int WritePacket(void *opaque, uint8_t *buf, int buf_size);

private:
    bool exit_ = false;
    uint8_t *avioCtxBuffer_ = nullptr;

    bool keyFrame_;
    uint32_t timeStamp_;
    FrameMerger merger_;

    std::mutex queueMutex_;
    std::mutex cbLockMutex_;
    std::queue<Frame::Ptr> dataQueue_;
    std::unique_ptr<std::thread> encodeThread_;

    AVStream *out_stream = nullptr;
    AVIOContext *avioContext_ = nullptr;
    AVFormatContext *avFormatContext_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif

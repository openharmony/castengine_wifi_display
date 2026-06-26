/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef OHOS_SHARING_RTP_DECODER_TS_H
#define OHOS_SHARING_RTP_DECODER_TS_H

#include <condition_variable>
#include <memory>
#include <queue>
#include <thread>
#include "frame/frame.h"
#include "rtp_decoder.h"
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
    constexpr static int64_t US_PER_SEC = 1000 * 1000;

    bool exit_ = false;
    int videoStreamIndex_ = -1;
    int audioStreamIndex_ = -1;

    std::mutex queueMutex_;
    std::condition_variable queueCond_;
    std::queue<RtpPacket::Ptr> dataQueue_;
    std::unique_ptr<std::thread> decodeThread_;

    AVIOContext *avioContext_ = nullptr;
    AVFormatContext *avFormatContext_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif

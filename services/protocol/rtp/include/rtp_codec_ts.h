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

#include "frame/frame.h"
#include "frame/frame_merger.h"
#include "mpeg_ts/mpeg_demuxer.h"
#include "mpeg_ts/mpeg_muxer.h"
#include "rtp_codec.h"
#include "rtp_maker.h"

namespace OHOS {
namespace Sharing {
class RtpDecoderTs : public RtpDecoder {
public:
    using Ptr = std::shared_ptr<RtpDecoderTs>;

    RtpDecoderTs();
    ~RtpDecoderTs();

    void InputRtp(const RtpPacket::Ptr &rtp) override;
    void SetOnFrame(const OnFrame &cb) override;

private:
    void FrameReady(const Frame::Ptr &frame);

    void OnStream(int32_t stream, int32_t codecId, const void *extra, size_t bytes, int32_t finish);
    void OnDecode(int32_t stream, int32_t codecId, int32_t flags, int64_t pts, int64_t dts, const void *data,
                  size_t bytes);

private:
    MpegDemuxer::Ptr tsDemuxer_ = nullptr;
    FrameMerger merger_;
};

class RtpEncoderTs : public RtpEncoder,
                     public RtpMaker {
public:
    using Ptr = std::shared_ptr<RtpEncoderTs>;

    RtpEncoderTs(uint32_t ssrc, uint32_t mtuSize, uint32_t sampleRate, uint8_t payloadType, uint16_t seq = 0);
    ~RtpEncoderTs();

    void InputFrame(const Frame::Ptr &frame) override;
    void SetOnRtpPack(const OnRtpPack &cb) override;

private:
    MpegMuxer::Ptr tsMuxer_ = nullptr;
    FrameMerger merger_;
};
} // namespace Sharing
} // namespace OHOS
#endif

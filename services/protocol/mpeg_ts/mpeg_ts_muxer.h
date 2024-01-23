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

#ifndef OHOS_SHARING_TS_MUXER_H
#define OHOS_SHARING_TS_MUXER_H

#include <map>
#include "frame/frame.h"
#include "mpeg-muxer.h"
#include "mpeg_muxer.h"
namespace OHOS {
namespace Sharing {
class MpegTsMuxer : public MpegMuxer {
public:
    using Ptr = std::shared_ptr<MpegTsMuxer>;

    MpegTsMuxer();
    ~MpegTsMuxer() override;

    void InputFrame(const Frame::Ptr &frame) override;
    void SetOnMux(const OnMux &cb) override;

private:
    void OnTSPacket();

private:
    OnMux onMux_;
    struct mpeg_muxer_t *muxerCtx_;
    std::map<int, int> codecTracks_; // key for codecId, val is stream for PS
    DataBuffer::Ptr buffer_;
    volatile uint32_t _timestamp;
    uint32_t curPos_ = 0;
    bool keyFrame_;
};
} // namespace Sharing
} // namespace OHOS
#endif

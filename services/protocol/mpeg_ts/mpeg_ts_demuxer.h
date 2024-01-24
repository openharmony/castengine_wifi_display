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

#ifndef OHOS_SHARING_TS_DEMUX_H
#define OHOS_SHARING_TS_DEMUX_H

#include "mpeg-ts-proto.h"
#include "mpeg-ts.h"
#include "mpeg_demuxer.h"
#include "mpeg_ts_splitter.h"

namespace OHOS {
namespace Sharing {
class MpegTsDemuxer final : public MpegDemuxer {
public:
    using Ptr = std::shared_ptr<MpegTsDemuxer>;

    MpegTsDemuxer();
    ~MpegTsDemuxer() override;

    void SetOnDecode(const OnDecode &cb) override;
    void SetOnStream(const OnStream &cb) override;

    size_t Input(const uint8_t *data, size_t len) override;

private:
    MpegTsSplitter splitter_;
    OnDecode onDecode_ = nullptr;
    OnStream onStream_ = nullptr;
    struct ts_demuxer_t *demuxerCtx_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif
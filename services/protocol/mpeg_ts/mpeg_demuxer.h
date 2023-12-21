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

#ifndef OHOS_SHARING_MPEG_DEMUX_H
#define OHOS_SHARING_MPEG_DEMUX_H

#include <functional>
#include <memory>

namespace OHOS {
namespace Sharing {
class MpegDemuxer {
public:
    using Ptr = std::shared_ptr<MpegDemuxer>;
    using OnDecode = std::function<void(int32_t stream, int32_t codecid, int32_t flags, int64_t pts, int64_t dts,
                                        const void *data, size_t bytes)>;
    using OnStream =
        std::function<void(int32_t stream, int32_t codecid, const void *extra, size_t bytes, int32_t finish)>;

    virtual void SetOnDecode(const OnDecode &cb) = 0;
    virtual void SetOnStream(const OnStream &cb) = 0;
    virtual size_t Input(const uint8_t *data, size_t len) = 0;

protected:
    MpegDemuxer() = default;
    virtual ~MpegDemuxer() = default;
};
} // namespace Sharing
} // namespace OHOS
#endif
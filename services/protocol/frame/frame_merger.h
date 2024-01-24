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

#ifndef OHOS_SHARING_FRAME_MERGE_H
#define OHOS_SHARING_FRAME_MERGE_H

#include <functional>
#include <list>
#include <memory>
#include "frame.h"

namespace OHOS {
namespace Sharing {
/**
 * Merge some frames with the same timestamp
 */
class FrameMerger {
public:
    using onOutput =
        std::function<void(uint32_t dts, uint32_t pts, const DataBuffer::Ptr &buffer, bool have_key_frame)>;
    using Ptr = std::shared_ptr<FrameMerger>;

    enum {
        NONE = 0,
        H264_PREFIX,
        MP4_NAL_SIZE,
    };

public:
    FrameMerger() = default;
    ~FrameMerger() = default;

    void Clear();
    void SetType(int32_t type);
    bool InputFrame(const Frame::Ptr &frame, DataBuffer::Ptr &buffer, const onOutput &cb);

private:
    bool WillFlush(const Frame::Ptr &frame) const;
    void DoMerge(DataBuffer::Ptr &merged, const Frame::Ptr &frame) const;

private:
    bool haveDecodeAbleFrame_ = false;
    int32_t type_;

    std::list<Frame::Ptr> frameCache_;
};
} // namespace Sharing
} // namespace OHOS
#endif
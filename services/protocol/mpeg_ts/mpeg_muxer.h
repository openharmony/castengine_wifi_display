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

#ifndef OHOS_SHARING_MEPG_MUXER_H
#define OHOS_SHARING_MEPG_MUXER_H

#include <memory>
#include "protocol/frame/frame.h"

namespace OHOS {
namespace Sharing {
class MpegMuxer {
public:
    using Ptr = std::shared_ptr<MpegMuxer>;
    using OnMux = std::function<void(const std::shared_ptr<DataBuffer> &buffer, uint32_t stamp, bool keyPos)>;

    virtual void SetOnMux(const OnMux &cb) = 0;
    virtual void InputFrame(const Frame::Ptr &frame) = 0;

protected:
    MpegMuxer() = default;
    virtual ~MpegMuxer() = default;

private:
    void ReleaseContext();
    void HandleContextCreate();
};
} // namespace Sharing
} // namespace OHOS
#endif
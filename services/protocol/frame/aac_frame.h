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

#ifndef OHOS_SHARING_AAC_FRAME_H
#define OHOS_SHARING_AAC_FRAME_H

#include <cstdlib>
#include <memory>
#include <string>
#include "frame.h"

namespace OHOS {
namespace Sharing {
class AACFrame : public FrameImpl {
public:
    using Ptr = std::shared_ptr<AACFrame>;

    AACFrame();
    AACFrame(uint8_t *ptr, size_t size, uint32_t dts, uint32_t pts = 0, size_t prefix_size = 0);
    ~AACFrame() override {}
};

} // namespace Sharing
} // namespace OHOS
#endif
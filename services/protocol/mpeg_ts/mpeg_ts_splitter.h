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

#ifndef OHOS_SHARING_TS_SPLITTER_H
#define OHOS_SHARING_TS_SPLITTER_H

#include <functional>
#include <string>
#include "mpeg-ts-proto.h"

namespace OHOS {
namespace Sharing {
constexpr size_t MAX_CACHE_SIZE = 1 * 1024 * 1024;

class MpegTsSplitter {
public:
    using OnSplit = std::function<void(const char *data, size_t len)>;

    explicit MpegTsSplitter(size_t size = TS_PACKET_SIZE);
    ~MpegTsSplitter() = default;

    void Reset();
    void setOnSplit(const OnSplit &cb);
    void Input(const char *data, size_t len);
    static bool IsTSPacket(const char *data, size_t len);

protected:
    void OnSplitter(const char *data, size_t len);

private:
    size_t size_ = TS_PACKET_SIZE;

    std::string remainData_;

    OnSplit onSplit_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif
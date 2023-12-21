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

#include "mpeg_ts_splitter.h"
#include "common/common_macro.h"
#include "common/media_log.h"

namespace OHOS {
namespace Sharing {
MpegTsSplitter::MpegTsSplitter(size_t size) : size_(size) {}

void MpegTsSplitter::Input(const char *data, size_t len)
{
    auto size = remainData_.length();
    if (size > MAX_CACHE_SIZE) {
        remainData_.clear();
    }
    remainData_.append(data, len);

    std::string::size_type pos = 0;
    int32_t splitLen = 0;
    int32_t count = 0;
    while (pos != std::string::npos) {
        pos = remainData_.find(TS_SYNC_BYTE, pos);
        if (pos != std::string::npos) {
            MEDIA_LOGD("split count: %{public}d.", ++count);
            auto &&tsdata = remainData_.substr(pos, size_);
            OnSplitter(tsdata.data(), tsdata.length());
            pos += size_;
            splitLen += size_;
        }
    }
    std::string tmp(data + splitLen, len - splitLen);
    remainData_ = std::move(tmp);
}

void MpegTsSplitter::setOnSplit(const OnSplit &cb)
{
    onSplit_ = std::move(cb);
}

bool MpegTsSplitter::IsTSPacket(const char *data, size_t len)
{
    RETURN_FALSE_IF_NULL(data);
    return len == TS_PACKET_SIZE && ((uint8_t *)data)[0] == TS_SYNC_BYTE;
}

void MpegTsSplitter::Reset()
{
    remainData_.clear();
}

void MpegTsSplitter::OnSplitter(const char *data, size_t len)
{
    if (!IsTSPacket(data, len)) {
        return;
    }
    onSplit_(data, len);
    return;
}
} // namespace Sharing
} // namespace OHOS
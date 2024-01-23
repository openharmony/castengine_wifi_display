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

#include "h264_frame.h"
#include "common/sharing_log.h"

namespace OHOS {
namespace Sharing {
size_t PrefixSize(const char *ptr, size_t len)
{
    if (len < 4) { // 4:byte offset
        return 0;
    }
    if (ptr[0] != 0x00 || ptr[1] != 0x00) {
        return 0;
    }
    if (ptr[2] == 0x00 && ptr[3] == 0x01) { // 2:byte offset, 3:byte offset
        return 4;                           // 4:prefix size
    }
    if (ptr[2] == 0x01) { // 2:byte offset
        return 3;         // 3:prefix size
    }

    return 0;
}

static const char *MemFind(const char *buf, ssize_t len, const char *subBuf, ssize_t subLen)
{
    for (auto i = 0; i < len - subLen; ++i) {
        if (memcmp(buf + i, subBuf, subLen) == 0) {
            return buf + i;
        }
    }

    return NULL;
}

void SplitH264(const char *ptr, size_t len, size_t prefix, const std::function<void(const char *, size_t, size_t)> &cb)
{
    if (prefix <= 0) {
        prefix = PrefixSize(ptr, len);
    }
    auto start = ptr + prefix;
    auto end = ptr + len;
    size_t nextPrefix;
    while (true) {
        auto nextStart = MemFind(start, end - start, "\x00\x00\x01", 3);
        if (nextStart) {
            if (*(nextStart - 1) == 0x00) {
                nextStart -= 1;
                nextPrefix = 4; // 4:prefix size
            } else {
                nextPrefix = 3; // 3:prefix size
            }

            cb(start - prefix, nextStart - start + prefix, prefix);

            start = nextStart + nextPrefix;

            prefix = nextPrefix;
            continue;
        }

        cb(start - prefix, end - start + prefix, prefix);
        break;
    }
}
} // namespace Sharing
} // namespace OHOS
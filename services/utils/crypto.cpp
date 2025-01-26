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

#include "crypto.h"
#include <iomanip>
#include <openssl/evp.h>
#include <sstream>
#include <vector>
#include "common/sharing_log.h"

namespace OHOS {
namespace Sharing {

std::string GetMD5(const std::string &src)
{
    int md5DigestLength = 4;
    unsigned char MD5Hash[md5DigestLength];
    std::string MD5Digest;
    std::string tmp;
    std::stringstream ss;

    for (int32_t i = 0; i < md5DigestLength; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int32_t)MD5Hash[i] << std::endl; // 2: fix offset
        ss >> tmp;
        MD5Digest += tmp;
    }

    return MD5Digest;
}

std::vector<uint8_t> GenerateSha256HashId(const uint8_t *originalId, uint32_t &len)
{
    if (originalId == nullptr || len == 0) {
        return std::vector<uint8_t>{};
    }

    std::unique_ptr<uint8_t[]> outHash = std::make_unique<uint8_t[]>(len);
    uint32_t outLen = 0; // should be 32 bytes
    int32_t ret = EVP_Digest(originalId, len, outHash.get(), &outLen, EVP_sha256(), nullptr);
    if (ret != 1) {
        SHARING_LOGE("Get Openssl Hash fail, %{public}d", ret);
        return std::vector<uint8_t>{};
    }
    std::vector<uint8_t> result(outHash.get(), outHash.get() + outLen);
    return result;
}
} // namespace Sharing
} // namespace OHOS
/*
 * Copyright (c) 2025 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#include "wfd_utils.h"
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <vector>
#include "sharing_log.h"
#include "utils/crypto.h"

namespace OHOS {
namespace Sharing {
const int BYTE_HEX_LEN = 2;
std::string ByteToHexStr(const std::vector<uint8_t> &data, uint32_t pos, uint32_t len, bool isNeed0x)
{
    std::stringstream ss;
    if (isNeed0x) {
        ss << "0x" << std::hex << std::setfill('0');
    } else {
        ss << std::hex << std::setfill('0');
    }
    for (uint32_t i = pos; (i < (pos + len)) && (i < data.size()); i++) {
        ss << std::setw(BYTE_HEX_LEN) << static_cast<int>(data[i]);
    }
    return ss.str();
}

std::string GetUdidHash(std::string &udid)
{
    if (udid.empty()) {
        return "";
    }
    uint32_t udidLen = udid.size();
    std::vector<uint8_t> hashIdResult = GenerateSha256HashId(reinterpret_cast<const uint8_t *>(udid.c_str()), udidLen);
    if (hashIdResult.empty()) {
        return "";
    }
    std::string ret = ByteToHexStr(hashIdResult, 0, hashIdResult.size(), false);
    return ret;
}

std::string GetAddressHash(const std::string &address)
{
    std::string addressStr = std::string(address);
    return GetUdidHash(addressStr);
}
} // namespace Sharing
} // namespace OHOS
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

#ifndef OHOS_SHARING_UTILS_H
#define OHOS_SHARING_UTILS_H

#include <regex>
#include <string>
#include <vector>

namespace OHOS {
namespace Sharing {

class RpcKeyParser {
public:
    RpcKeyParser() = default;

    std::string GetRpcKey(std::string bundleName, std::string abilityName, std::string deviceId, std::string className)

    {
        bundleName_ = std::move(bundleName);
        abilityName_ = std::move(abilityName);
        deviceId_ = std::move(deviceId);
        className_ = std::move(className);
        return bundleName_ + "+" + abilityName_ + "+" + deviceId_ + "+" + className_;
    }

    bool ParseKey(std::string key)
    {
        std::regex reg{"\\+"};
        std::vector<std::string> out{std::sregex_token_iterator(key.begin(), key.end(), reg, -1),
                                     std::sregex_token_iterator()};
        if (out.size() != 4) { // 4: rpckey length
            return false;
        }
        bundleName_ = std::move(out[0]);
        abilityName_ = std::move(out[1]);
        deviceId_ = std::move(out[2]); // 2: rpckey parse
        className_ = std::move(out[3]); // 3: rpckey parse
        return true;
    }

    std::string GetBundleName()
    {
        return bundleName_;
    }

    std::string GetAbilityName()
    {
        return abilityName_;
    }

    std::string GetDeviceId()
    {
        return deviceId_;
    }

    std::string GetClassName()
    {
        return className_;
    }

private:
    std::string deviceId_;
    std::string className_;
    std::string bundleName_;
    std::string abilityName_;
};

unsigned long long GetThreadId();
uint64_t GetCurrentMillisecond();

std::string ChangeCase(const std::string &value, bool lowerCase);
std::string Trim(std::string &&s, const std::string &chars = " \r\n\t");
std::string &Trim(std::string &s, const std::string &chars = " \r\n\t");

std::vector<std::string> Split(const std::string &s, const char *delim);
std::vector<std::string> SplitOnce(const std::string &s, const char *delim);

void SaveFile(const char *data, int32_t dataSize, const std::string &fileName);

#if defined(__ARM_NEON__)
void NeonMemcpy(volatile unsigned char *dst, volatile unsigned char *src, int32_t size);
#endif

uint16_t SwapEndian16(uint16_t value);
uint32_t SwapEndian32(uint32_t value);
uint64_t SwapEndian64(uint64_t value);

uint16_t LoadBE16(const uint8_t *p);
uint32_t LoadBE24(const uint8_t *p);
uint32_t LoadBE32(const uint8_t *p);
uint64_t LoadBE64(const uint8_t *p);

void SetBE24(void *p, uint32_t val);
void SetBE32(void *p, uint32_t val);
void SetLE32(void *p, uint32_t val);

std::string GetAnonyString(const std::string &value);
} // namespace Sharing
} // namespace OHOS
#endif
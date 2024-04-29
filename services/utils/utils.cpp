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

#include "utils.h"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <securec.h>
#include <sstream>
#include <sys/time.h>
#include <thread>
#include "common/media_log.h"

namespace OHOS {
namespace Sharing {
unsigned long long GetThreadId()
{
    std::thread::id tid = std::this_thread::get_id();
    std::stringstream buf;
    buf << tid;
    std::string stid = buf.str();
    return std::stoull(stid);
}

std::vector<std::string> Split(const std::string &s, const char *delim)
{
    std::vector<std::string> ret;
    size_t last = 0;
    auto index = s.find(delim, last);
    while (index != std::string::npos) {
        if (index - last > 0) {
            ret.push_back(s.substr(last, index - last));
        }
        last = index + strlen(delim);
        index = s.find(delim, last);
    }
    if (!s.size() || s.size() - last > 0) {
        ret.push_back(s.substr(last));
    }

    return ret;
}

std::vector<std::string> SplitOnce(const std::string &s, const char *delim)
{
    std::vector<std::string> ret;
    size_t last = 0;
    auto index = s.find(delim, last);
    if (index != std::string::npos) {
        ret.push_back(s.substr(0, index));
        ret.push_back(s.substr(index + 1));
    } else {
        ret.push_back(s);
    }

    return ret;
}

#define TRIM(s, chars)                                             \
    do {                                                           \
        std::string map(0xFF, '\0');                               \
        for (auto &ch : (chars)) {                                 \
            map[(unsigned char &)ch] = '\1';                       \
        }                                                          \
        while ((s).size() && map.at((unsigned char &)(s).back()))  \
            (s).pop_back();                                        \
        while ((s).size() && map.at((unsigned char &)(s).front())) \
            (s).erase(0, 1);                                       \
    } while (0)

std::string &Trim(std::string &s, const std::string &chars)
{
    TRIM(s, chars);
    return s;
}

std::string Trim(std::string &&s, const std::string &chars)
{
    TRIM(s, chars);
    return std::move(s);
}

#define ADD_VECTOR_END(v, i) (v).push_back((i))

std::string LowerCase(std::string value)
{
    return ChangeCase(value, true);
}

std::string UpperCase(std::string value)
{
    return ChangeCase(value, false);
}

void LTrim(std::string &value)
{
    std::string::size_type i = 0;
    for (i = 0; i < value.length(); i++) {
        if (value[i] != ' ' && value[i] != '\t' && value[i] != '\n' && value[i] != '\r')
            break;
    }
    value = value.substr(i);
}

void RTrim(std::string &value)
{
    int32_t i = 0;
    for (i = (int32_t)value.length() - 1; i >= 0; i--) {
        if (value[i] != ' ' && value[i] != '\t' && value[i] != '\n' && value[i] != '\r')
            break;
    }
    value = value.substr(0, i + 1);
}

void Trim(std::string &value)
{
    LTrim(value);
    RTrim(value);
}

std::string ChangeCase(const std::string &value, bool LowerCase)
{
    std::string newvalue(value);
    for (std::string::size_type i = 0, l = newvalue.length(); i < l; ++i)
        newvalue[i] = LowerCase ? tolower(newvalue[i]) : toupper(newvalue[i]);
    return newvalue;
}

void Replace(std::string &target, std::string search, std::string replacement)
{
    if (search == replacement) {
        return;
    }

    if (search == "") {
        return;
    }

    std::string::size_type i = std::string::npos;
    std::string::size_type lastPos = 0;
    while ((i = target.find(search, lastPos)) != std::string::npos) {
        target.replace(i, search.length(), replacement);
        lastPos = i + replacement.length();
    }
}

void Split(std::string str, std::string separator, std::vector<std::string> &result)
{
    result.clear();
    std::string::size_type position = str.find(separator);
    std::string::size_type lastPosition = 0;
    uint32_t separatorLength = separator.length();

    while (position != str.npos) {
        ADD_VECTOR_END(result, str.substr(lastPosition, position - lastPosition));
        lastPosition = position + separatorLength;
        position = str.find(separator, lastPosition);
    }
    ADD_VECTOR_END(result, str.substr(lastPosition, std::string::npos));
}

uint64_t GetCurrentMillisecond()
{
    struct timeval tv {};
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000; // 1000: time base conversion.
}

void SaveFile(const char *data, int32_t dataSize, const std::string &fileName)
{
    std::ofstream out(fileName, std::ios::out | std::ios::binary);
    if (!out.is_open()) {
        SHARING_LOGD("failed to opne file: %{public}s", fileName.c_str());
        return;
    }

    out.write(data, dataSize);
    out.flush();
    out.close();
}

void NeonMemcpy(volatile unsigned char *dst, volatile unsigned char *src, int size)
{
#if defined(__ARM_NEON__)
    int neonCopy = size - size % 64;
    if (neonCopy > 0) {
        int tempCount = neonCopy;
        asm volatile("NEONCopyPLD_%=: \n"
                     " VLDM %[src]!,{d0-d7} \n"
                     " VSTM %[dst]!,{d0-d7} \n"
                     " SUBS %[tempCount],%[tempCount],#0x40 \n"
                     " BGT NEONCopyPLD_%= \n"
                     : [dst] "+r"(dst), [src] "+r"(src), [tempCount] "+r"(tempCount)::"d0", "d1", "d2", "d3", "d4",
                       "d5", "d6", "d7", "cc", "memory");
    }
    if (size - neonCopy > 0) {
        auto ret = memcpy_s((void *)dst, size - neonCopy, (void *)src, size - neonCopy);
        if (ret != EOK) {
            return;
        }
    }
#else
    if (size > 0) {
        auto ret = memcpy_s((void *)dst, size, (void *)src, size);
        if (ret != EOK) {
            return;
        }
    }
#endif
}

uint16_t SwapEndian16(uint16_t value)
{
    return (value & 0xff00) >> 8 | (value & 0x00ff) << 8; // 8: swap endian
}

uint32_t SwapEndian32(uint32_t value)
{
    uint8_t res[4];
    for (int i = 0; i < 4; ++i) { // 4: swap endian
        res[i] = ((uint8_t *)&value)[3 - i]; // 3: swap endian
    }
    return *(uint32_t *)res;
}

uint64_t SwapEndian64(uint64_t value)
{
    uint8_t res[8];
    for (int i = 0; i < 8; ++i) { // 8: swap endian
        res[i] = ((uint8_t *)&value)[7 - i]; // 7: swap endian
    }
    return *(uint64_t *)res;
}

uint16_t LoadBE16(const uint8_t *p)
{
    return SwapEndian16(*(uint16_t *)p);
}

uint32_t LoadBE24(const uint8_t *p)
{
    uint8_t res[4] = {0, p[0], p[1], p[2]};
    return SwapEndian32(*(uint32_t *)res);
}

uint32_t LoadBE32(const uint8_t *p)
{
    return SwapEndian32(*(uint32_t *)p);
}

uint64_t LoadBE64(const uint8_t *p)
{
    return SwapEndian64(*(uint64_t *)p);
}

void SetBE24(void *p, uint32_t val)
{
    uint8_t *data = (uint8_t *)p;
    data[0] = val >> 16; // 16: byte offset
    data[1] = val >> 8; // 8: byte offset
    data[2] = val; // 2: transformed position
}

void SetBE32(void *p, uint32_t val)
{
    uint8_t *data = (uint8_t *)p;
    data[3] = val; // 3: transformed position
    data[2] = val >> 8; // 2: transformed position, 8: byte offset
    data[1] = val >> 16; // 16: byte offset
    data[0] = val >> 24; // 24: byte offset
}

void SetLE32(void *p, uint32_t val)
{
    uint8_t *data = (uint8_t *)p;
    data[0] = val;
    data[1] = val >> 8; //  8: byte offset
    data[2] = val >> 16; // 2: transformed position, 16: byte offset
    data[3] = val >> 24; // 3: transformed position, 24: byte offset
}

std::string GetAnonyString(const std::string &value)
{
    constexpr size_t INT32_SHORT_ID_LENGTH = 20;
    constexpr size_t INT32_MIN_ID_LENGTH = 3;
    std::string result;
    std::string tmpStr("******");
    size_t strLen = value.length();
    if (strLen < INT32_MIN_ID_LENGTH) {
        return tmpStr;
    }

    if (strLen <= INT32_SHORT_ID_LENGTH) {
        result += value[0];
        result += tmpStr;
        result += value[strLen - 1];
    } else {
        constexpr size_t INT32_PLAINTEXT_LENGTH = 4;
        result.append(value, 0, INT32_PLAINTEXT_LENGTH);
        result += tmpStr;
        result.append(value, strLen - INT32_PLAINTEXT_LENGTH, INT32_PLAINTEXT_LENGTH);
    }

    return result;
}

} // namespace Sharing
} // namespace OHOS
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

#ifndef OHOS_SHARING_ADTS_H
#define OHOS_SHARING_ADTS_H

#include <cstdint>
#include <string>

namespace OHOS {
namespace Sharing {
constexpr int32_t ADTS_HEADER_LEN = 7;

class AdtsHeader {
public:
    static int32_t GetAacFrameLength(const uint8_t *data, size_t bytes);

    static void ParseAacConfig(const std::string &config, AdtsHeader &adts);

    static void DumpAdtsHeader(const AdtsHeader &hed, uint8_t *out);
    static int32_t DumpAacConfig(const std::string &config, size_t length, uint8_t *out, size_t outSize);

public:
    uint32_t id_ = 0;                           // 1 bslbf
    uint32_t home_ = 0;                         // 1 bslbf
    uint32_t layer_ = 0;                        // 2 uimsbf Indicates which layer is used. Set to ‘00’
    uint32_t profile_ = 0;                      // 2 uimsbf
    uint32_t sfIndex_ = 0;                      // 4 uimsbf
    uint32_t syncword_ = 0;                     // 12 bslbf The bit string ‘1111 1111 1111’
    uint32_t original_ = 0;                     // 1 bslbf
    uint32_t privateBit_ = 0;                   // 1 bslbf
    uint32_t aacFrameLength_ = 0;               // 13 bslbf
    uint32_t protectionAbsent_ = 0;             // 1 bslbf
    uint32_t adtsBufferFullness_ = 0;           // 11 bslbf
    uint32_t channelConfiguration_ = 0;         // 3 uimsbf
    uint32_t copyrightIdentificationBit_ = 0;   // 1 bslbf
    uint32_t copyrightIdentificationStart_ = 0; // 1 bslbf
    uint32_t numberOfRawDataBlocksInFrame_ = 0; // 2 uimsfb
};
} // namespace Sharing
} // namespace OHOS
#endif
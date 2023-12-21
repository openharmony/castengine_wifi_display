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

#include "adts.h"
#include "common/common_macro.h"

namespace OHOS {
namespace Sharing {

void AdtsHeader::DumpAdtsHeader(const AdtsHeader &hed, uint8_t *out)
{
    out[0] = (hed.syncword_ >> 4 & 0xFF);                      // 8bit
    out[1] = (hed.syncword_ << 4 & 0xF0);                      // 4 bit
    out[1] |= (hed.id_ << 3 & 0x08);                           // 1 bit
    out[1] |= (hed.layer_ << 1 & 0x06);                        // 2bit
    out[1] |= (hed.protectionAbsent_ & 0x01);                  // 1 bit
    out[2] = (hed.profile_ << 6 & 0xC0);                       // 2 bit
    out[2] |= (hed.sfIndex_ << 2 & 0x3C);                      // 4bit
    out[2] |= (hed.privateBit_ << 1 & 0x02);                   // 1 bit
    out[2] |= (hed.channelConfiguration_ >> 2 & 0x03);         // 1 bit
    out[3] = (hed.channelConfiguration_ << 6 & 0xC0);          // 2 bit
    out[3] |= (hed.original_ << 5 & 0x20);                     // 1 bit
    out[3] |= (hed.home_ << 4 & 0x10);                         // 1 bit
    out[3] |= (hed.copyrightIdentificationBit_ << 3 & 0x08);   // 1 bit
    out[3] |= (hed.copyrightIdentificationStart_ << 2 & 0x04); // 1 bit
    out[3] |= (hed.aacFrameLength_ >> 11 & 0x03);              // 2 bit
    out[4] = (hed.aacFrameLength_ >> 3 & 0xFF);                // 8 bit
    out[5] = (hed.aacFrameLength_ << 5 & 0xE0);                // 3 bit
    out[5] |= (hed.adtsBufferFullness_ >> 6 & 0x1F);           // 5 bit
    out[6] = (hed.adtsBufferFullness_ << 2 & 0xFC);            // 6 bit
    out[6] |= (hed.numberOfRawDataBlocksInFrame_ & 0x03);      // 2 bit
}

void AdtsHeader::ParseAacConfig(const std::string &config, AdtsHeader &adts)
{
    uint8_t cfg1 = config[0];
    uint8_t cfg2 = config[1];

    int32_t audioObjectType;
    int32_t samplingFrequencyIndex;
    int32_t channelConfiguration;

    audioObjectType = cfg1 >> 3;
    samplingFrequencyIndex = ((cfg1 & 0x07) << 1) | (cfg2 >> 7);
    channelConfiguration = (cfg2 & 0x7F) >> 3;

    adts.syncword_ = 0x0FFF;
    adts.id_ = 0;
    adts.layer_ = 0;
    adts.protectionAbsent_ = 1;
    adts.profile_ = audioObjectType - 1;
    adts.sfIndex_ = samplingFrequencyIndex;
    adts.privateBit_ = 0;
    adts.channelConfiguration_ = channelConfiguration;
    adts.original_ = 0;
    adts.home_ = 0;
    adts.copyrightIdentificationBit_ = 0;
    adts.copyrightIdentificationStart_ = 0;
    adts.aacFrameLength_ = 7;
    adts.adtsBufferFullness_ = 2047;
    adts.numberOfRawDataBlocksInFrame_ = 0;
}

int32_t AdtsHeader::DumpAacConfig(const std::string &config, size_t length, uint8_t *out, size_t outSize)
{
    AdtsHeader header;
    ParseAacConfig(config, header);
    header.aacFrameLength_ = (decltype(header.aacFrameLength_))(ADTS_HEADER_LEN + length);
    DumpAdtsHeader(header, out);
    return ADTS_HEADER_LEN;
}

int32_t AdtsHeader::GetAacFrameLength(const uint8_t *data, size_t bytes)
{
    RETURN_INVALID_IF_NULL(data);
    uint16_t len;
    if (bytes < 7)
        return -1;
    if (0xFF != data[0] || 0xF0 != (data[1] & 0xF0)) {
        return -1;
    }
    len = ((uint16_t)(data[3] & 0x03) << 11) | ((uint16_t)data[4] << 3) | ((uint16_t)(data[5] >> 5) & 0x07);
    return len;
}
} // namespace Sharing
} // namespace OHOS
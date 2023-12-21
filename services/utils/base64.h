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

#ifndef OHOS_SHARING_RTSP_URI_H
#define OHOS_SHARING_RTSP_URI_H
#include <assert.h>
#include <stdint.h>
namespace OHOS {
namespace Sharing {

static uint8_t alphabetMap[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static uint8_t reverseMap[] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 62,
    255, 255, 255, 63,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  255, 255, 255, 255, 255, 255, 255, 0,
    1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,
    23,  24,  25,  255, 255, 255, 255, 255, 255, 26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
    39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  255, 255, 255, 255, 255};

class Base64 {
public:
    static uint32_t Encode(const char *text, uint32_t text_len, uint8_t *encode)
    {
        uint32_t i, j;
        for (i = 0, j = 0; i + 3 <= text_len; i += 3) {
            encode[j++] = alphabetMap[text[i] >> 2];
            encode[j++] = alphabetMap[((text[i] << 4) & 0x30) | (text[i + 1] >> 4)];
            encode[j++] = alphabetMap[((text[i + 1] << 2) & 0x3c) | (text[i + 2] >> 6)];
            encode[j++] = alphabetMap[text[i + 2] & 0x3f];
        }

        if (i < text_len) {
            uint32_t tail = text_len - i;
            if (tail == 1) {
                encode[j++] = alphabetMap[text[i] >> 2];
                encode[j++] = alphabetMap[(text[i] << 4) & 0x30];
                encode[j++] = '=';
                encode[j++] = '=';
            } else {
                encode[j++] = alphabetMap[text[i] >> 2];
                encode[j++] = alphabetMap[((text[i] << 4) & 0x30) | (text[i + 1] >> 4)];
                encode[j++] = alphabetMap[(text[i + 1] << 2) & 0x3c];
                encode[j++] = '=';
            }
        }

        return j;
    }

    static uint32_t Decode(const char *code, uint32_t code_len, uint8_t *plain)
    {
        assert((code_len & 0x03) == 0);

        uint32_t i, j = 0;
        uint8_t quad[4];
        for (i = 0; i < code_len; i += 4) {
            for (uint32_t k = 0; k < 4; k++) {
                quad[k] = reverseMap[(int32_t)code[i + k]];
            }

            assert(quad[0] < 64 && quad[1] < 64);

            plain[j++] = (quad[0] << 2) | (quad[1] >> 4);

            if (quad[2] >= 64) {
                break;
            } else if (quad[3] >= 64) {
                plain[j++] = (quad[1] << 4) | (quad[2] >> 2);
                break;
            } else {
                plain[j++] = (quad[1] << 4) | (quad[2] >> 2);
                plain[j++] = (quad[2] << 6) | quad[3];
            }
        }

        return j;
    }
};

} // namespace Sharing
} // namespace OHOS
#endif // OHOS_SHARING_RTSP_SDP_H
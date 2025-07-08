/*
 * Copyright (c) 2024 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include "common/const_def.h"
#include "protocol/rtp/include/rtp_queue.h"
#include "protocol/rtp/include/rtp_unpack_impl.h"
#include "wfd_sink_rtsp_fuzzer.h"

namespace OHOS {
namespace Sharing {
constexpr uint32_t RTP_MAX_SIZE = 4000;

bool RtpUnpackImplParseRtpFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }

    FuzzedDataProvider provider(data, size);
    const char *fuzzedData = reinterpret_cast<const char *>(data);
    size_t fuzzedLen = provider.ConsumeIntegralInRange<size_t>(0, RTP_MAX_SIZE);
    RtpUnpackImpl rtpUnpack;
    rtpUnpack.ParseRtp(fuzzedData, fuzzedLen);

    return true;
}
} // namespace Sharing
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::Sharing::RtpUnpackImplParseRtpFuzzTest(data, size);
    return 0;
}

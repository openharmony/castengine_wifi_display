/*
 * Copyright (c) 2024 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_SHARING_MOCK_RTP_UNPACK_H
#define OHOS_SHARING_MOCK_RTP_UNPACK_H

#include <gmock/gmock.h>
#define private public
#define protected public
#include "rtp_unpack.h"
#undef private
#undef protected

namespace OHOS {
namespace Sharing {

class MockRtpUnpack : public RtpUnpack {
public:
    MOCK_METHOD(void, Release, ());
    MOCK_METHOD(void, ParseRtp, (const char *data, size_t len));
    MOCK_METHOD(void, SetOnRtpUnpack, (const OnRtpUnpack &cb));
    MOCK_METHOD(void, SetOnRtpNotify, (const OnRtpNotify &cb));
};

} // namespace Sharing
} // namespace OHOS
#endif

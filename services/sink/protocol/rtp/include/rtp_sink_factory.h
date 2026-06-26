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

#ifndef OHOS_SHARING_RTP_SINK_FACTORY_H
#define OHOS_SHARING_RTP_SINK_FACTORY_H

#include "rtp_def.h"
#include "rtp_unpack.h"

namespace OHOS {
namespace Sharing {
class RtpSinkFactory {
public:
    static RtpUnpack::Ptr CreateRtpUnpack();
    static RtpUnpack::Ptr CreateRtpUnpack(const RtpPlaylodParam &rpp);

private:
    RtpSinkFactory() = delete;
    ~RtpSinkFactory() = delete;
};
} // namespace Sharing
} // namespace OHOS
#endif

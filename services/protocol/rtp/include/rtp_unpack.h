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

#ifndef OHOS_SHARING_RTP_UNPACK_H
#define OHOS_SHARING_RTP_UNPACK_H

#include <functional>
#include <map>
#include "rtp_codec.h"
#include "rtp_packet.h"

namespace OHOS {
namespace Sharing {
class RtpUnpack {
public:
    using Ptr = std::shared_ptr<RtpUnpack>;
    using OnRtpNotify = std::function<void(int32_t)>;
    // Unpack a RtpPackget callback
    using OnRtpUnpack = std::function<void(uint32_t, const Frame::Ptr &frame)>;

    enum {
        RTP_UNPACK_OK = 0,
    };
    /**
     * @brief Release resources
     */
    virtual void Release() = 0;
    /**
     * @brief Parse rtp data
     * @param data rtp data
     * @param len rtp length
     */
    virtual void ParseRtp(const char *data, size_t len) = 0;
    /**
     * @brief Externally exposed  rtp unpack callback function
     * @param cb unpack to a Video/Audio frame callback
     */
    virtual void SetOnRtpUnpack(const OnRtpUnpack &cb) = 0;
    /**
     * @brief Externally exposed  rtp notify callback function
     * @param cb rtp notify callback
     */
    virtual void SetOnRtpNotify(const OnRtpNotify &cb) = 0;

protected:
    RtpUnpack() = default;
    virtual ~RtpUnpack() = default;

protected:
    OnRtpUnpack onRtpUnpack_ = nullptr;
    OnRtpNotify onRtpNotify_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif
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

#include <gmock/gmock.h>
#include "mock_wfd_rtp_producer.h"

namespace OHOS {
namespace Sharing {

void MockWfdRtpProducer::ValidateUdpClient()
{
    if (tsUdpClient_ == nullptr) {
        tsUdpClient_ = std::make_shared<UdpClient>(false);
    }
    if (tsRtcpUdpClient_ == nullptr) {
        tsRtcpUdpClient_ = std::make_shared<UdpClient>(true);
    }
}

void MockWfdRtpProducer::SetInitState(bool state)
{
    isInit_ = state;
}

void MockWfdRtpProducer::SetPauseState(bool state)
{
    isPaused_ = state;
}

void MockWfdRtpProducer::SetRunningState(bool state)
{
    isRunning_ = state;
}

} // namespace Sharing
} // namespace OHOS

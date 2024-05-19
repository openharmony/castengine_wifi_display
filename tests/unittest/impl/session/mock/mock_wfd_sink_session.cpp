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
#include "mock_wfd_sink_session.h"

namespace OHOS {
namespace Sharing {

void MockWfdSinkSession::InitSession()
{
    timeoutTimer_ = std::make_unique<TimeoutTimer>();
    keepAliveTimer_ = std::make_unique<TimeoutTimer>();
    keepAliveTimeout_ = 30; // 30 : keep alive timer 30 seconds
}

void MockWfdSinkSession::SetConnectState(bool state)
{
    connected_ = state;
}

void MockWfdSinkSession::SetInterruptState(bool state)
{
    interrupting_ = state;
}

void MockWfdSinkSession::SetWfdState(WfdSessionState state)
{
    wfdState_ = state;
}

void MockWfdSinkSession::SetClient(std::shared_ptr<IClient> client)
{
    rtspClient_ = client;
}

void MockWfdSinkSession::SetRunningState(SessionRunningStatus state)
{
    status_ = state;
}

} // namespace Sharing
} // namespace OHOS

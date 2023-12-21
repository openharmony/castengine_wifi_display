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

#ifndef OHOS_SHARING_SESSION_DEF_H
#define OHOS_SHARING_SESSION_DEF_H

#include <string>

namespace OHOS {

namespace Sharing {

enum SessionNotifyStatus {
    STATE_SESSION_IDLE,
    STATE_SESSION_ERROR,
    STATE_SESSION_STARTED,
    STATE_SESSION_STOPED,
    STATE_SESSION_PAUSED,
    STATE_SESSION_RESUMED,
    STATE_SESSION_DESTROYED,
    STATE_SESSION_INTERRUPTED,
    NOTIFY_PROSUMER_CREATE,
    NOTIFY_PROSUMER_DESTROY,
    NOTIFY_PROSUMER_START,
    NOTIFY_PROSUMER_STOP,
    NOTIFY_PROSUMER_PAUSE,
    NOTIFY_PROSUMER_RESUME,
    NOTIFY_SESSION_PRIVATE_EVENT
};

enum MediaNotifyStatus {
    STATE_PROSUMER_NONE,
    STATE_PROSUMER_CREATE_SUCCESS,
    STATE_PROSUMER_START_SUCCESS,
    STATE_PROSUMER_PAUSE_SUCCESS,
    STATE_PROSUMER_RESUME_SUCCESS,
    STATE_PROSUMER_STOP_SUCCESS,
    STATE_PROSUMER_DESTROY_SUCCESS
};

} // namespace Sharing
} // namespace OHOS
#endif
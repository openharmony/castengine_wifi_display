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

#ifndef OHOS_SHARING_BASE_SESSION_H
#define OHOS_SHARING_BASE_SESSION_H

#include "common/identifier.h"
#include "common/object.h"
#include "common/sharing_log.h"
#include "event/handle_event_base.h"
#include "isession_listener.h"
#include "session_def.h"

namespace OHOS {
namespace Sharing {

enum SessionRunningStatus {
    SESSION_START,
    SESSION_STOP,
    SESSION_PAUSE,
    SESSION_DESTROY,
    SESSION_RESUME,
    SESSION_INTERRUPT
};

class BaseSession : public Object,
                    public HandleEventBase,
                    public IdentifierInfo {
public:
    using Ptr = std::shared_ptr<BaseSession>;

    BaseSession() = default;
    virtual ~BaseSession() = default;

    void SetSessionListener(std::weak_ptr<ISessionListener> listener)
    {
        listener_ = listener;
    }

public:
    virtual void UpdateOperation(SessionStatusMsg::Ptr &statusMsg) = 0;
    virtual void UpdateMediaStatus(SessionStatusMsg::Ptr &statusMsg) = 0;

protected:
    void Notify(SessionStatusMsg::Ptr &statusMsg)
    {
        SHARING_LOGD("trace.");
        auto listener = listener_.lock();
        if (listener) {
            listener->OnSessionNotify(statusMsg);
        }
    }

protected:
    bool interrupting_ = false;
    std::weak_ptr<ISessionListener> listener_;
    SessionRunningStatus status_ = SESSION_START;
};

} // namespace Sharing
} // namespace OHOS
#endif
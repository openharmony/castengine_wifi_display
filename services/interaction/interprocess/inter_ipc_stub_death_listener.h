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

#ifndef OHOS_SHARING_INTER_IPC_STUB_DEATH_LISTENER_H
#define OHOS_SHARING_INTER_IPC_STUB_DEATH_LISTENER_H

#include "inter_ipc_stub.h"

namespace OHOS {
namespace Sharing {

class InterIpcStubDeathListener : public InterIpcDeathListener {
public:
    explicit InterIpcStubDeathListener(InterIpcStub::Ptr stub)
    {
        SHARING_LOGD("trace.");
        stub_ = stub;
    }

    ~InterIpcStubDeathListener() override = default;

    void OnRemoteDied(std::string key) override
    {
        SHARING_LOGD("on IpcStub remote died.");
        auto stub = stub_.lock();
        if (stub != nullptr) {
            stub->OnRemoteDied();
        }
    }

private:
    std::weak_ptr<InterIpcStub> stub_;
};

} // namespace Sharing
} // namespace OHOS
#endif

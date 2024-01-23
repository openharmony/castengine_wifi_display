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

#ifndef OHOS_SHARING_EVENT_DESCRIPTOR_LISTENER_H
#define OHOS_SHARING_EVENT_DESCRIPTOR_LISTENER_H

#include "common/const_def.h"
#include "file_descriptor_listener.h"

namespace OHOS {
namespace Sharing {

class DataBuffer;
class EventDescriptorListener : public OHOS::AppExecFwk::FileDescriptorListener {
public:
    EventDescriptorListener();
    explicit EventDescriptorListener(int32_t fd);
    virtual ~EventDescriptorListener();

    virtual void OnClose(int32_t fd);
    void OnReadable(int32_t fd) override;
    void OnWritable(int32_t fd) override;
    void OnShutdown(int32_t fd) override;
    void OnException(int32_t fd) override;

    void RemoveFdListener(int32_t fd, const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &handler = nullptr);
    bool AddFdListener(int32_t fd, const std::shared_ptr<FileDescriptorListener> &listener,
                       const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &handler = nullptr,
                       const std::string &taskName = DUMMY_EMPTY);

    virtual int32_t GetSocketFd();

protected:
    virtual void OnReadData(int32_t fd, std::shared_ptr<DataBuffer> &buf);

protected:
    int32_t socketLocalFd_ = -1;
};
} // namespace Sharing
} // namespace OHOS
#endif

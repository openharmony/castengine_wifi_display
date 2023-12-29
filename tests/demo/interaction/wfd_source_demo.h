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

#ifndef OHOS_SHARING_WFD_SOURCE_DEMO_H
#define OHOS_SHARING_WFD_SOURCE_DEMO_H

#include "wfd_source.h"
namespace OHOS {
namespace Sharing {
class WfdSourceDemoListener;
class WfdSourceDemo : public std::enable_shared_from_this<WfdSourceDemo> {
public:
    WfdSourceDemo();
    ~WfdSourceDemo() = default;

    bool Stop();
    bool Start();
    bool CreateClient();
    bool StopDiscover();
    bool StartDiscover();

    void DoCmd(std::string cmd);
    void AddCastDevice(const std::string deviceId);
    void RemoveCastDevice(const std::string deviceId);
    void ExecuteCmd(int32_t cmd, std::string &deviceId);

    std::shared_ptr<WfdSource> GetClient();

private:
    std::shared_ptr<WfdSource> client_ = nullptr;
    std::shared_ptr<WfdSourceDemoListener> listener_ = nullptr;
};

class WfdSourceDemoListener : public IWfdEventListener {
public:
    void OnInfo(std::shared_ptr<BaseMsg> &msg) override;
    void OnConnectionChanged(const ConnectionInfo &info);
    void OnDeviceFound(const std::vector<WfdCastDeviceInfo> &deviceInfos);
    void OnError(uint32_t regionId, uint32_t agentId, SharingErrorCode errorCode);
    void SetListener(std::shared_ptr<WfdSourceDemo> listener)
    {
        listener_ = listener;
    }

private:
    std::weak_ptr<WfdSourceDemo> listener_;
};

} // namespace Sharing
} // namespace OHOS
#endif

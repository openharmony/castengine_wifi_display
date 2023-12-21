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

#ifndef SHARING_DM_KIT_H
#define SHARING_DM_KIT_H

#include "common/sharing_log.h"
#include "device_manager.h"
#include "device_manager_callback.h"
#include "dm_device_info.h"

namespace OHOS {
namespace Sharing {

class DmKitInitDMCallback : public OHOS::DistributedHardware::DmInitCallback {
public:
    DmKitInitDMCallback() = default;
    ~DmKitInitDMCallback() override = default;

    void OnRemoteDied() override{};
};

class DmKit {
public:
    static void InitDeviceManager();
    static std::string GetIpAddrByNetworkId(const std::string &networkId);
    static std::vector<OHOS::DistributedHardware::DmDeviceInfo> &GetTrustedDevicesInfo();

    static OHOS::DistributedHardware::DmDeviceInfo &GetLocalDevicesInfo();

private:
    static bool isInited;
    static std::vector<OHOS::DistributedHardware::DmDeviceInfo> trustedDeviceInfos_;

    static OHOS::DistributedHardware::DmDeviceInfo localDeviceInfo_;
};

} // namespace Sharing
} // namespace OHOS

#endif

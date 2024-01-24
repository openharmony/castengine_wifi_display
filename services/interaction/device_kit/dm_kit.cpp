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

#include "dm_kit.h"

namespace OHOS {
namespace Sharing {
bool DmKit::isInited = false;
const std::string PKG_NAME = "sharing_framework";
OHOS::DistributedHardware::DmDeviceInfo DmKit::localDeviceInfo_ = {};
std::vector<OHOS::DistributedHardware::DmDeviceInfo> DmKit::trustedDeviceInfos_ = {};

void DmKit::InitDeviceManager()
{
    SHARING_LOGD("trace.");
    if (isInited) {
        SHARING_LOGD("DmKit is already Inited.");
        return;
    }

    auto callback = std::make_shared<DmKitInitDMCallback>();
    CHECK_AND_RETURN_LOG(callback != nullptr, "no memory.");
    int32_t ret = OHOS::DistributedHardware::DeviceManager::GetInstance().InitDeviceManager(PKG_NAME, callback);
    if (ret == 0) {
        isInited = true;
    }

    CHECK_AND_RETURN_LOG(ret == 0, "InitDeviceManager error ret is %{public}d.", ret);
}

OHOS::DistributedHardware::DmDeviceInfo &DmKit::GetLocalDevicesInfo()
{
    SHARING_LOGD("GetLocalNetworkId.");
    if (!isInited) {
        SHARING_LOGE("DmKit is not Inited.");
    }

    localDeviceInfo_ = {};
    int32_t ret = DistributedHardware::DeviceManager::GetInstance().GetLocalDeviceInfo(PKG_NAME, localDeviceInfo_);
    CHECK_AND_RETURN_RET_LOG(ret == 0, localDeviceInfo_, "get local deviceInfo failed.");

    return localDeviceInfo_;
}

std::vector<OHOS::DistributedHardware::DmDeviceInfo> &DmKit::GetTrustedDevicesInfo()
{
    SHARING_LOGD("trace.");
    if (!isInited) {
        SHARING_LOGE("DmKit is not Inited.");
    }

    trustedDeviceInfos_.clear();
    int32_t ret =
        DistributedHardware::DeviceManager::GetInstance().GetTrustedDeviceList(PKG_NAME, "", trustedDeviceInfos_);
    CHECK_AND_RETURN_RET_LOG(ret == 0, trustedDeviceInfos_, "get trusted device list failed");

    return trustedDeviceInfos_;
}

std::string DmKit::GetIpAddrByNetworkId(const std::string &networkId)
{
    SHARING_LOGD("trace.");
    if (!isInited) {
        SHARING_LOGE("DmKit is not Inited.");
    }

    std::string ipAddr = "";

    return ipAddr;
}

} // namespace Sharing
} // namespace OHOS
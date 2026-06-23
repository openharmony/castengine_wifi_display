/*
 * Copyright (c) 2025 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#include "wfd_trust_list_manager.h"
#include "common/const_def.h"
#include "common/sharing_log.h"
#include "wfd_utils.h"

namespace OHOS {
namespace Sharing {
WfdTrustListManager::WfdTrustListManager()
{
    p2pInstance_ = Wifi::WifiP2p::GetInstance(WIFI_P2P_ABILITY_ID);
    preferencesUtil_ = std::make_shared<PreferencesUtil>(TRUST_DEVICES_PREFERENCES_PATH);
}

std::vector<BoundDeviceInfo> WfdTrustListManager::GetAllBoundDevices()
{
    SHARING_LOGI("trace");
    std::vector<BoundDeviceInfo> devices;
    std::vector<Wifi::WifiP2pGroupInfo> groups;
    if (p2pInstance_ == nullptr || preferencesUtil_ == nullptr) {
        SHARING_LOGE("GetAllBoundDevices nullptr");
        return devices;
    }
    p2pInstance_->QueryP2pGroups(groups);
    std::map<std::string, BoundDeviceInfo> deviceMap;
    Wifi::WifiP2pDevice localDevice;
    p2pInstance_->QueryP2pLocalDevice(localDevice);
    std::string localMac = localDevice.GetDeviceAddress();
    for (Wifi::WifiP2pGroupInfo groupInfo : groups) {
        std::string ownerMac = groupInfo.GetOwner().GetDeviceAddress();
        if (localMac == ownerMac) {
            std::vector<Wifi::WifiP2pDevice> clientDevices = groupInfo.GetClientDevices();
            if (clientDevices.empty()) {
                SHARING_LOGE("clientDevices empty");
                continue;
            }
            for (Wifi::WifiP2pDevice device : clientDevices) {
                AddBoundDeviceItem(deviceMap, groupInfo, MaskAddress(device.GetDeviceAddress()));
            }
        } else {
            AddBoundDeviceItem(deviceMap, groupInfo, MaskAddress(ownerMac));
        }
    }
    for (auto device : deviceMap) {
        devices.push_back(device.second);
    }
    return devices;
}

void WfdTrustListManager::AddBoundDeviceItem(std::map<std::string, BoundDeviceInfo> &deviceMap,
                                             const Wifi::WifiP2pGroupInfo group, std::string encryptedMac)
{
    if (encryptedMac.empty()) {
        SHARING_LOGE("encryptedMac empty");
        return;
    }
    if (deviceMap.find(encryptedMac) != deviceMap.end()) {
        return;
    }
    std::string name = preferencesUtil_->GetString(encryptedMac);
    if (name.empty()) {
        return;
    }
    BoundDeviceInfo deviceInfo;
    deviceInfo.deviceId = encryptedMac;
    deviceInfo.deviceAddress = encryptedMac;
    deviceInfo.deviceName = name;
    deviceInfo.networkId = group.GetNetworkId();
    deviceMap.emplace(encryptedMac, deviceInfo);
}

void WfdTrustListManager::AddBoundDevice(const Wifi::WifiP2pGroupInfo &group)
{
    SHARING_LOGI("trace");
    if (group.IsGroupOwner()) {
        std::vector<Wifi::WifiP2pDevice> clientDevices = group.GetClientDevices();
        if (clientDevices.empty()) {
            SHARING_LOGE("clientDevices empty");
            return;
        }
        for (Wifi::WifiP2pDevice device : clientDevices) {
            saveBoundDevice(device);
        }
    } else {
        saveBoundDevice(group.GetOwner());
    }
}

void WfdTrustListManager::saveBoundDevice(const Wifi::WifiP2pDevice &device)
{
    std::string deviceName = GetDeviceName(device);
    if (!deviceName.empty()) {
        preferencesUtil_->PutString(MaskAddress(device.GetDeviceAddress()), deviceName);
    }
}

std::string WfdTrustListManager::GetDeviceName(const Wifi::WifiP2pDevice &device)
{
    if (!device.GetDeviceName().empty()) {
        return device.GetDeviceName();
    }
    std::string networkName = device.GetNetworkName();
    if (networkName.empty()) {
        return "";
    }
    auto splits = Split(networkName, "-");
    if (splits.size() < MIN_DEVICE_NAME_LEN) {
        return networkName;
    }
    auto index = networkName.find(splits[DEVICE_NAME_INDEX]);
    if (index != std::string::npos) {
        return networkName.substr(index);
    }
    return "";
}

int32_t WfdTrustListManager::DeleteBoundDeviceGroup(std::string &deviceAddress)
{
    SHARING_LOGI("trace");
    std::vector<Wifi::WifiP2pGroupInfo> groups;
    if (p2pInstance_ == nullptr || preferencesUtil_ == nullptr) {
        SHARING_LOGE("DeleteBoundDeviceGroup nullptr");
        return OPERATE_ERR;
    }
    int32_t result = OPERATE_OK;
    p2pInstance_->QueryP2pGroups(groups);
    Wifi::WifiP2pDevice localDevice;
    p2pInstance_->QueryP2pLocalDevice(localDevice);
    std::string localMac = localDevice.GetDeviceAddress();
    for (Wifi::WifiP2pGroupInfo groupInfo : groups) {
        std::string ownerMac = groupInfo.GetOwner().GetDeviceAddress();
        if (localMac != ownerMac) {
            if (MaskAddress(ownerMac) != deviceAddress) {
                continue;
            }
            if (!DeleteP2pGroup(groupInfo.GetOwner(), deviceAddress, groupInfo)) {
                result = OPERATE_ERR;
            }
            continue;
        }
        std::vector<Wifi::WifiP2pDevice> clientDevices = groupInfo.GetClientDevices();
        if (clientDevices.empty()) {
            SHARING_LOGW("clientDevices empty");
            continue;
        }
        for (Wifi::WifiP2pDevice device : clientDevices) {
            if (MaskAddress(device.GetDeviceAddress()) != deviceAddress) {
                continue;
            }
            if (!DeleteP2pGroup(device, deviceAddress, groupInfo)) {
                result = OPERATE_ERR;
            }
        }
    }
    return result;
}

bool WfdTrustListManager::DeleteP2pGroup(const Wifi::WifiP2pDevice &device, std::string &deviceAddress,
                                         const Wifi::WifiP2pGroupInfo &groupInfo)
{
    Wifi::ErrCode errCode = p2pInstance_->DeleteGroup(groupInfo);
    if (errCode == Wifi::WIFI_OPT_SUCCESS) {
        preferencesUtil_->DeleteKey(deviceAddress);
        return true;
    } else {
        SHARING_LOGE("DeleteP2pGroup fail, errCode = %{public}d", errCode);
        return false;
    }
}

std::vector<std::string> WfdTrustListManager::Split(const std::string &str, const std::string &delimiter)
{
    std::regex reg{delimiter};
    return {std::sregex_token_iterator(str.begin(), str.end(), reg, -1), std::sregex_token_iterator()};
}

std::string WfdTrustListManager::MaskAddress(const std::string &address)
{
    if (address.empty()) {
        return "";
    }
    std::string addressStr = std::string(address);
    return addressStr.replace(HASH_ADDRESS_SUB_START, HASH_ADDRESS_SUB_LENGTH, "****");
}

} // namespace Sharing
} // namespace OHOS
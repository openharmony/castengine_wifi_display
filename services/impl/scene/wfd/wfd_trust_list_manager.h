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

#ifndef OHOS_SHARING_WFD_TRUST_LIST_MANAGER_H
#define OHOS_SHARING_WFD_TRUST_LIST_MANAGER_H

#include <regex>
#include "utils/preferences_util.h"
#include "utils/utils.h"
#include "wfd_def.h"
#include "wifi_p2p.h"

namespace OHOS {
namespace Sharing {
constexpr int32_t MIN_DEVICE_NAME_LEN = 3;
constexpr int32_t DEVICE_NAME_INDEX = 2;
constexpr int32_t HASH_ADDRESS_LENGTH = 64;
constexpr int32_t HASH_ADDRESS_SUB_START = 5;
constexpr int32_t HASH_ADDRESS_SUB_LENGTH = 5;
const std::string TRUST_DEVICES_PREFERENCES_PATH = "/data/service/el1/public/database/sharingcodec/trust_list.xml";

class WfdTrustListManager {
public:
    WfdTrustListManager();

public:
    std::vector<BoundDeviceInfo> GetAllBoundDevices();
    void DeleteBoundDeviceGroup(std::string &deviceAddress);
    void AddBoundDevice(const Wifi::WifiP2pGroupInfo &group);

private:
    void AddBoundDeviceItem(std::map<std::string, BoundDeviceInfo> &deviceMap, const Wifi::WifiP2pGroupInfo group,
                            std::string encryptedMac);
    void DeleteP2pGroup(const Wifi::WifiP2pDevice &device, std::string &deviceAddress,
                        const Wifi::WifiP2pGroupInfo &groupInfo);
    std::string GetDeviceName(const Wifi::WifiP2pDevice &device);
    std::vector<std::string> Split(const std::string &str, const std::string &delimiter);
    std::string MaskAddress(const std::string &address);
    void saveBoundDevice(const Wifi::WifiP2pDevice &device);

private:
    std::shared_ptr<Wifi::WifiP2p> p2pInstance_;
    std::shared_ptr<PreferencesUtil> preferencesUtil_;
};
} // namespace Sharing
} // namespace OHOS
#endif
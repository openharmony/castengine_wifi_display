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

#ifndef OHOS_SHARING_MOCK_WIFI_P2P_H
#define OHOS_SHARING_MOCK_WIFI_P2P_H

#include <gmock/gmock.h>
#include "wifi_p2p.h"

namespace OHOS {
namespace Sharing {

class MockWifiP2p : public Wifi::WifiP2p {
public:
    MockWifiP2p();
    virtual ~MockWifiP2p() = default;
public:
    MOCK_METHOD(Wifi::ErrCode, EnableP2p, ());
    MOCK_METHOD(Wifi::ErrCode, DisableP2p, ());
    MOCK_METHOD(Wifi::ErrCode, DiscoverDevices, ());
    MOCK_METHOD(Wifi::ErrCode, StopDiscoverDevices, ());
    MOCK_METHOD(Wifi::ErrCode, DiscoverServices, ());
    MOCK_METHOD(Wifi::ErrCode, StopDiscoverServices, ());
    MOCK_METHOD(Wifi::ErrCode, RequestService, (const Wifi::WifiP2pDevice &device,
                const Wifi::WifiP2pServiceRequest &request));
    MOCK_METHOD(Wifi::ErrCode, PutLocalP2pService, (const Wifi::WifiP2pServiceInfo &srvInfo));
    MOCK_METHOD(Wifi::ErrCode, DeleteLocalP2pService, (const Wifi::WifiP2pServiceInfo &srvInfo));
    MOCK_METHOD(Wifi::ErrCode, StartP2pListen, (int period, int interval));
    MOCK_METHOD(Wifi::ErrCode, StopP2pListen, ());
    MOCK_METHOD(Wifi::ErrCode, CreateGroup, (const Wifi::WifiP2pConfig &config));
    MOCK_METHOD(Wifi::ErrCode, RemoveGroup, ());
    MOCK_METHOD(Wifi::ErrCode, RemoveGroupClient, (const Wifi::GcInfo &info));
    MOCK_METHOD(Wifi::ErrCode, DeleteGroup, (const Wifi::WifiP2pGroupInfo &group));
    MOCK_METHOD(Wifi::ErrCode, P2pConnect, (const Wifi::WifiP2pConfig &config));
    MOCK_METHOD(Wifi::ErrCode, P2pCancelConnect, ());
    MOCK_METHOD(Wifi::ErrCode, QueryP2pLinkedInfo, (Wifi::WifiP2pLinkedInfo &linkedInfo));
    MOCK_METHOD(Wifi::ErrCode, GetCurrentGroup, (Wifi::WifiP2pGroupInfo &group));
    MOCK_METHOD(Wifi::ErrCode, GetP2pEnableStatus, (int &status));
    MOCK_METHOD(Wifi::ErrCode, GetP2pDiscoverStatus, (int &status));
    MOCK_METHOD(Wifi::ErrCode, GetP2pConnectedStatus, (int &status));
    MOCK_METHOD(Wifi::ErrCode, QueryP2pLocalDevice, (Wifi::WifiP2pDevice &device));
    MOCK_METHOD(Wifi::ErrCode, QueryP2pDevices, (std::vector<Wifi::WifiP2pDevice> &devices));
    MOCK_METHOD(Wifi::ErrCode, QueryP2pGroups, (std::vector<Wifi::WifiP2pGroupInfo> &groups));
    MOCK_METHOD(Wifi::ErrCode, QueryP2pServices, (std::vector<Wifi::WifiP2pServiceInfo> &services));
    MOCK_METHOD(Wifi::ErrCode, RegisterCallBack, (const sptr<Wifi::IWifiP2pCallback> &callback,
                const std::vector<std::string> &event));
    MOCK_METHOD(Wifi::ErrCode, GetSupportedFeatures, (long &features));
    MOCK_METHOD(bool, IsFeatureSupported, (long feature));
    MOCK_METHOD(Wifi::ErrCode, SetP2pDeviceName, (const std::string &deviceName));
    MOCK_METHOD(Wifi::ErrCode, SetP2pWfdInfo, (const Wifi::WifiP2pWfdInfo &wfdInfo));
};

} // namespace Sharing
} // namespace OHOS
#endif

/*
 * Copyright (c) 2024 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#include "wfd_source_scene_test.h"
#include "common/sharing_log.h"
#include "wfd_session_def.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

void WfdSourceSceneTest::SetUpTestCase(void)
{
    sourceScene_ = std::make_shared<MockWfdSourceScene>();
}

void WfdSourceSceneTest::TearDownTestCase(void)
{
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup()).Times(1)
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    sourceScene_ = nullptr;
}

void WfdSourceSceneTest::SetUp(void)
{
}

void WfdSourceSceneTest::TearDown(void)
{
}

void WfdSourceSceneTest::BackupConnDev(void)
{
    connectionInfo.mac = sourceScene_->connDev_->mac;
    connectionInfo.contextId = sourceScene_->connDev_->contextId;
    connectionInfo.agentId = sourceScene_->connDev_->agentId;
}

void WfdSourceSceneTest::RestoreConnDev(void)
{
    sourceScene_->connDev_ = std::make_unique<ConnectionInfo>();
    sourceScene_->connDev_->mac = connectionInfo.mac;
    sourceScene_->connDev_->contextId = connectionInfo.contextId;
    sourceScene_->connDev_->agentId = connectionInfo.agentId;
}

HWTEST_F(WfdSourceSceneTest, Initialize_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
        
    sourceScene_->Initialize();
    sourceScene_->MockInitialize();
}

HWTEST_F(WfdSourceSceneTest, HandleStartDiscovery_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<WfdSourceStartDiscoveryReq> msg = std::make_shared<WfdSourceStartDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), GetP2pEnableStatus(_))
        .WillOnce(DoAll(SetArgReferee<0>(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_NONE)),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    int32_t ret = sourceScene_->HandleStartDiscovery(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSourceSceneTest, HandleStartDiscovery_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<WfdSourceStartDiscoveryReq> msg = std::make_shared<WfdSourceStartDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), GetP2pEnableStatus(_))
        .WillOnce(DoAll(SetArgReferee<0>(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_IDLE)),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    int32_t ret = sourceScene_->HandleStartDiscovery(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSourceSceneTest, HandleStartDiscovery_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<WfdSourceStartDiscoveryReq> msg = std::make_shared<WfdSourceStartDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), GetP2pEnableStatus(_))
        .WillOnce(DoAll(SetArgReferee<0>(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_STARTING)),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    int32_t ret = sourceScene_->HandleStartDiscovery(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSourceSceneTest, HandleStartDiscovery_004, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<WfdSourceStartDiscoveryReq> msg = std::make_shared<WfdSourceStartDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), GetP2pEnableStatus(_))
        .WillOnce(DoAll(SetArgReferee<0>(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_CLOSING)),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    int32_t ret = sourceScene_->HandleStartDiscovery(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSourceSceneTest, HandleStartDiscovery_005, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<WfdSourceStartDiscoveryReq> msg = std::make_shared<WfdSourceStartDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), GetP2pEnableStatus(_))
        .WillOnce(DoAll(SetArgReferee<0>(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_STARTED)),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), SetP2pWfdInfo(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), DiscoverDevices()).Times(2)
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    int32_t ret = sourceScene_->HandleStartDiscovery(msg, reply);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSourceSceneTest, OnP2pStateChanged_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_NONE));
}

HWTEST_F(WfdSourceSceneTest, OnP2pStateChanged_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_IDLE));
}

HWTEST_F(WfdSourceSceneTest, OnP2pStateChanged_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_STARTING));
}

HWTEST_F(WfdSourceSceneTest, OnP2pStateChanged_004, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), SetP2pWfdInfo(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), DiscoverDevices())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_STARTED));
}

HWTEST_F(WfdSourceSceneTest, OnP2pStateChanged_005, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_CLOSING));
}

HWTEST_F(WfdSourceSceneTest, OnP2pStateChanged_006, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup()).Times(1)
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));
    
    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_CLOSED));
}

HWTEST_F(WfdSourceSceneTest, HandleStartDiscovery_006, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<WfdSourceStartDiscoveryReq> msg = std::make_shared<WfdSourceStartDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), GetP2pEnableStatus(_))
        .WillOnce(DoAll(SetArgReferee<0>(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_CLOSED)),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), EnableP2p())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), DiscoverDevices())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    int32_t ret = sourceScene_->HandleStartDiscovery(msg, reply);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSourceSceneTest, OnDeviceFound_001, TestSize.Level1)
{
    std::vector<WfdCastDeviceInfo> deviceInfos;
    sourceScene_->OnDeviceFound(deviceInfos);
}

HWTEST_F(WfdSourceSceneTest, OnP2pPeerConnected_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    ConnectionInfo connectionInfo;
    connectionInfo.mac = "AA.BB.CC.DD.EE.FF";
    connectionInfo.state = ConnectionState::INIT;
    sourceScene_->OnP2pPeerConnected(connectionInfo);
}

HWTEST_F(WfdSourceSceneTest, OnP2pPeerConnected_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    ConnectionInfo connectionInfo;
    connectionInfo.mac = "AA.BB.CC.DD.EE.FF";
    sourceScene_->OnP2pPeerConnected(connectionInfo);
}

HWTEST_F(WfdSourceSceneTest, OnP2pPeersChanged_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    std::vector<Wifi::WifiP2pDevice> devices;
    Wifi::WifiP2pDevice device;
    device.SetP2pDeviceStatus(Wifi::P2pDeviceStatus::PDS_AVAILABLE);
    Wifi::WifiP2pWfdInfo info;
    info.setSessionAvailable(true);
    device.SetWfdInfo(info);
    devices.emplace_back(device);
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pPeersChanged(devices);
}

HWTEST_F(WfdSourceSceneTest, OnP2pConnectionChanged_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    Wifi::WifiP2pLinkedInfo info;
    info.SetConnectState(Wifi::P2pConnectedState::P2P_CONNECTED);
    info.SetIsGroupOwnerAddress("1.1.1.1");
    Wifi::WifiP2pGroupInfo group;
    Wifi::WifiP2pDevice device;
    device.SetDeviceAddress("AA.BB.CC.DD.EE.FF");
    group.SetOwner(device);

    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), GetCurrentGroup(_))
        .WillOnce(DoAll(SetArgReferee<0>(group),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pConnectionChanged(info);
}

HWTEST_F(WfdSourceSceneTest, SetCheckWfdConnectionTimer_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->SetCheckWfdConnectionTimer();
}

HWTEST_F(WfdSourceSceneTest, OnCheckWfdConnection_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    sourceScene_->OnCheckWfdConnection();
}

HWTEST_F(WfdSourceSceneTest, HandleStopDiscovery_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<WfdSourceStopDiscoveryReq> msg = std::make_shared<WfdSourceStopDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), StopDiscoverDevices())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    sourceScene_->HandleStopDiscovery(msg, reply);
}

HWTEST_F(WfdSourceSceneTest, HandleAddDevice_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<WfdSourceAddDeviceReq> msg = std::make_shared<WfdSourceAddDeviceReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    msg->screenId = 0;
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), P2pConnect(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    int32_t ret = sourceScene_->HandleAddDevice(msg, reply);
    EXPECT_EQ(ret, Wifi::ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WfdSourceSceneTest, HandleAddDevice_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<WfdSourceAddDeviceReq> msg = std::make_shared<WfdSourceAddDeviceReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    msg->screenId = -1;

    int32_t ret = sourceScene_->HandleAddDevice(msg, reply);
    EXPECT_EQ(ret, ERR_BAD_PARAMETER);
}

HWTEST_F(WfdSourceSceneTest, CreateScreenCapture_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), CreateAgent(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<0>(INVALID_ID), SetArgReferee<1>(INVALID_ID), Return(0)));

    int32_t ret = sourceScene_->CreateScreenCapture();
    EXPECT_EQ(ret, ERR_AGENT_CREATE);
}

HWTEST_F(WfdSourceSceneTest, CreateScreenCapture_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), CreateAgent(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(1), Return(0)));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), Start(_, _))
        .WillOnce(Return(0));

    int32_t ret = sourceScene_->CreateScreenCapture();
    EXPECT_EQ(ret, ERR_OK);
}

HWTEST_F(WfdSourceSceneTest, HandleDestroyScreenCapture_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<DestroyScreenCaptureReq> msg = std::make_shared<DestroyScreenCaptureReq>();
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(0));

    int32_t ret = sourceScene_->HandleDestroyScreenCapture(msg);
    EXPECT_EQ(ret, ERR_OK);
}

HWTEST_F(WfdSourceSceneTest, AppendCast_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), CreateAgent(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(2), Return(0)));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), Start(_, _))
        .WillOnce(Return(0));
    
    int32_t ret = sourceScene_->AppendCast("AA.BB.CC.DD.EE.FF");
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSourceSceneTest, AppendCast_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    int32_t ret = sourceScene_->AppendCast("");
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSourceSceneTest, WfdP2pStart_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), SetP2pWfdInfo(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), DiscoverDevices())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    sourceScene_->WfdP2pStart();
}

HWTEST_F(WfdSourceSceneTest, WfdP2pStop_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup()).Times(1)
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    sourceScene_->WfdP2pStop();
}

HWTEST_F(WfdSourceSceneTest, OnConnectionChanged_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    ConnectionInfo connectionInfo;
    connectionInfo.state = ConnectionState::INIT;

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    sourceScene_->OnConnectionChanged(connectionInfo);
}


HWTEST_F(WfdSourceSceneTest, OnP2pPeerDisconnected_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    ConnectionInfo connectionInfo;
    connectionInfo.mac = "";

    sourceScene_->OnP2pPeerDisconnected(connectionInfo);
}


HWTEST_F(WfdSourceSceneTest, OnP2pPeerDisconnected_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->OnP2pPeerDisconnected("");
}

HWTEST_F(WfdSourceSceneTest, OnP2pPeerDisconnected_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    ConnectionInfo connectionInfo;
    connectionInfo.mac = "AA.BB.CC.DD.EE.FF";

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup()).Times(1)
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    BackupConnDev();
    sourceScene_->OnP2pPeerDisconnected(connectionInfo);
    RestoreConnDev();
}

HWTEST_F(WfdSourceSceneTest, OnP2pPeerDisconnected_004, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup()).Times(1)
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    BackupConnDev();
    sourceScene_->OnP2pPeerDisconnected("AA.BB.CC.DD.EE.FF");
    RestoreConnDev();
}

HWTEST_F(WfdSourceSceneTest, OnP2pThisDeviceChanged_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    Wifi::WifiP2pDevice device;
    Wifi::WifiP2pLinkedInfo info;
    info.SetConnectState(Wifi::P2pConnectedState::P2P_DISCONNECTED);
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), QueryP2pLinkedInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(info),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup()).Times(1)
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    BackupConnDev();
    sourceScene_->wfdP2pCallback_->OnP2pThisDeviceChanged(device);
    RestoreConnDev();
}

HWTEST_F(WfdSourceSceneTest, ErrorCodeFiltering_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    int32_t code = static_cast<int32_t>(ERR_PROSUMER_START);
    sourceScene_->ErrorCodeFiltering(code);
}

HWTEST_F(WfdSourceSceneTest, ErrorCodeFiltering_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    int32_t code = static_cast<int32_t>(ERR_PROSUMER_TIMEOUT);
    sourceScene_->ErrorCodeFiltering(code);
}

HWTEST_F(WfdSourceSceneTest, ErrorCodeFiltering_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    int32_t code = static_cast<int32_t>(ERR_PROSUMER_DESTROY);
    sourceScene_->ErrorCodeFiltering(code);
}

HWTEST_F(WfdSourceSceneTest, OnInnerError_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    sourceScene_->OnInnerError(0, 0, SharingErrorCode::ERR_PROSUMER_TIMEOUT, "error");
}

HWTEST_F(WfdSourceSceneTest, OnInnerError_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    sourceScene_->OnInnerError(0, 0, SharingErrorCode::ERR_PROSUMER_INIT, "error");
}

HWTEST_F(WfdSourceSceneTest, OnInnerDestroy_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    sourceScene_->OnInnerDestroy(1, 1, AgentType::SRC_AGENT);
}

HWTEST_F(WfdSourceSceneTest, OnInnerEvent_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    SharingEvent event;
    auto msg = std::make_shared<WfdSceneEventMsg>();
    msg->mac = "";
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_PLAYED;
    event.eventMsg = msg;

    sourceScene_->OnInnerEvent(event);
}

HWTEST_F(WfdSourceSceneTest, OnInnerEvent_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    SharingEvent event;
    auto msg = std::make_shared<WfdSceneEventMsg>();
    msg->mac = "AA.BB.CC.DD.EE.FF";
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_PLAYED;
    event.eventMsg = msg;

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    sourceScene_->OnInnerEvent(event);
}

HWTEST_F(WfdSourceSceneTest, OnInnerEvent_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    SharingEvent event;
    auto msg = std::make_shared<WfdSceneEventMsg>();
    msg->mac = "";
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_TEARDOWN;
    event.eventMsg = msg;

    sourceScene_->OnInnerEvent(event);
}

HWTEST_F(WfdSourceSceneTest, OnInnerEvent_004, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    SharingEvent event;
    auto msg = std::make_shared<WfdSceneEventMsg>();
    msg->mac = "AA.BB.CC.DD.EE.FF";
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_TEARDOWN;
    event.eventMsg = msg;

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup()).Times(1)
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    BackupConnDev();
    sourceScene_->OnInnerEvent(event);
    RestoreConnDev();
}

HWTEST_F(WfdSourceSceneTest, OnRemoteDied_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), ReleaseScene(_));

    sourceScene_->OnRemoteDied();
}

HWTEST_F(WfdSourceSceneTest, HandleRemoveDevice_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<WfdSourceRemoveDeviceReq> msg = std::make_shared<WfdSourceRemoveDeviceReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "AA.BB.CC.DD.EE.FF";
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup()).Times(1)
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    BackupConnDev();
    int32_t ret = sourceScene_->HandleRemoveDevice(msg, reply);
    RestoreConnDev();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSourceSceneTest, HandleRemoveDevice_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<WfdSourceRemoveDeviceReq> msg = std::make_shared<WfdSourceRemoveDeviceReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "";
    int32_t ret = sourceScene_->HandleRemoveDevice(msg, reply);
    EXPECT_EQ(ret, -1);
}

} // namespace Sharing
} // namespace OHOS

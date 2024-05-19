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

#include "wfd_sink_scene_test.h"
#include "common/sharing_log.h"
#include "wfd_session_def.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

void WfdSinkSceneTest::SetUpTestCase(void)
{
    sinkScene_ = std::make_shared<MockWfdSinkScene>();
}

void WfdSinkSceneTest::TearDownTestCase(void)
{
    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), RemoveGroup()).Times(1)
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    sinkScene_ = nullptr;
}

void WfdSinkSceneTest::SetUp(void)
{
}

void WfdSinkSceneTest::TearDown(void)
{
}

void WfdSinkSceneTest::RestoreConnMap(void)
{
    ConnectionInfo connectionInfo;
    sinkScene_->devConnectionMap_.clear();
    connectionInfo.contextId = 1;
    connectionInfo.agentId = 1;
    connectionInfo.mac = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<ConnectionInfo> connectionInfoPtr = std::make_shared<ConnectionInfo>(connectionInfo);
    sinkScene_->devConnectionMap_.emplace(connectionInfo.mac, connectionInfoPtr);
}

void WfdSinkSceneTest::RestoreSurfaceMap(void)
{
    DevSurfaceItem surface;
    surface.contextId = 1;
    surface.agentId = 1;
    surface.deviceId = "AA:BB:CC:DD:EE:FF";
    auto surfaceItemPtr = std::make_shared<DevSurfaceItem>(surface);
    sinkScene_->devSurfaceItemMap_.emplace(0, surfaceItemPtr);
}

HWTEST_F(WfdSinkSceneTest, Initialize_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);
        
    sinkScene_->Initialize();
    sinkScene_->MockInitialize();
}

HWTEST_F(WfdSinkSceneTest, OnP2pStateChanged_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);
    ASSERT_TRUE(sinkScene_->wfdP2pCallback_ != nullptr);

    sinkScene_->wfdP2pCallback_->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_NONE));
}

HWTEST_F(WfdSinkSceneTest, OnP2pStateChanged_002, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);
    ASSERT_TRUE(sinkScene_->wfdP2pCallback_ != nullptr);

    sinkScene_->wfdP2pCallback_->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_IDLE));
}

HWTEST_F(WfdSinkSceneTest, OnP2pStateChanged_003, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);
    ASSERT_TRUE(sinkScene_->wfdP2pCallback_ != nullptr);

    sinkScene_->wfdP2pCallback_->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_STARTING));
}

HWTEST_F(WfdSinkSceneTest, OnP2pStateChanged_004, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);
    ASSERT_TRUE(sinkScene_->wfdP2pCallback_ != nullptr);

    sinkScene_->isSinkRunning_ = true;
    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), RemoveGroup())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), SetP2pWfdInfo(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), QueryP2pGroups(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), QueryP2pLocalDevice(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), CreateGroup(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    sinkScene_->wfdP2pCallback_->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_STARTED));
}

HWTEST_F(WfdSinkSceneTest, OnP2pStateChanged_005, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);
    ASSERT_TRUE(sinkScene_->wfdP2pCallback_ != nullptr);

    sinkScene_->wfdP2pCallback_->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_CLOSING));
}

HWTEST_F(WfdSinkSceneTest, OnP2pStateChanged_006, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);
    ASSERT_TRUE(sinkScene_->wfdP2pCallback_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), RemoveGroupClient(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), StopDiscoverDevices())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), RemoveGroup())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    RestoreConnMap();
    sinkScene_->wfdP2pCallback_->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_CLOSED));
}

HWTEST_F(WfdSinkSceneTest, HandleStart_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), GetP2pEnableStatus(_)).Times(1)
        .WillOnce(DoAll(SetArgReferee<0>(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_NONE)),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));
    
    auto msg = std::make_shared<WfdSinkStartReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    int32_t ret = sinkScene_->HandleStart(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSinkSceneTest, HandleStart_002, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), GetP2pEnableStatus(_)).Times(1)
        .WillOnce(DoAll(SetArgReferee<0>(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_IDLE)),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));
    
    auto msg = std::make_shared<WfdSinkStartReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    int32_t ret = sinkScene_->HandleStart(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSinkSceneTest, HandleStart_003, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), GetP2pEnableStatus(_)).Times(1)
        .WillOnce(DoAll(SetArgReferee<0>(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_STARTING)),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));
    
    auto msg = std::make_shared<WfdSinkStartReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    int32_t ret = sinkScene_->HandleStart(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSinkSceneTest, HandleStart_004, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), GetP2pEnableStatus(_)).Times(1)
        .WillOnce(DoAll(SetArgReferee<0>(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_CLOSING)),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));
    
    auto msg = std::make_shared<WfdSinkStartReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    int32_t ret = sinkScene_->HandleStart(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSinkSceneTest, HandleStart_005, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), GetP2pEnableStatus(_)).Times(1)
        .WillOnce(DoAll(SetArgReferee<0>(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_STARTED)),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), RemoveGroup())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), SetP2pWfdInfo(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), QueryP2pGroups(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), QueryP2pLocalDevice(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), CreateGroup(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    auto msg = std::make_shared<WfdSinkStartReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    int32_t ret = sinkScene_->HandleStart(msg, reply);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSceneTest, HandleStart_006, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), GetP2pEnableStatus(_)).Times(1)
        .WillOnce(DoAll(SetArgReferee<0>(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_CLOSED)),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), EnableP2p())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    
    auto msg = std::make_shared<WfdSinkStartReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    sinkScene_->isSinkRunning_ = false;
    int32_t ret = sinkScene_->HandleStart(msg, reply);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSceneTest, HandleStop_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), RemoveGroupClient(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), StopDiscoverDevices())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), RemoveGroup())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    
    auto msg = std::make_shared<WfdSinkStopReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    RestoreConnMap();
    int32_t ret = sinkScene_->HandleStop(msg, reply);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSceneTest, OnP2pPeersChanged_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);
    ASSERT_TRUE(sinkScene_->wfdP2pCallback_ != nullptr);

    std::vector<Wifi::WifiP2pDevice> devices;
    Wifi::WifiP2pDevice device;
    device.SetP2pDeviceStatus(Wifi::P2pDeviceStatus::PDS_UNAVAILABLE);
    devices.emplace_back(device);
    sinkScene_->wfdP2pCallback_->OnP2pPeersChanged(devices);
}

HWTEST_F(WfdSinkSceneTest, OnP2pPeersChanged_002, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);
    ASSERT_TRUE(sinkScene_->wfdP2pCallback_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), RemoveGroupClient(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(0));

    std::vector<Wifi::WifiP2pDevice> devices;
    Wifi::WifiP2pDevice device;
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    device.SetP2pDeviceStatus(Wifi::P2pDeviceStatus::PDS_AVAILABLE);
    devices.emplace_back(device);
    RestoreConnMap();
    sinkScene_->wfdP2pCallback_->OnP2pPeersChanged(devices);
}

HWTEST_F(WfdSinkSceneTest, OnP2pGcJoinGroup_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);
    ASSERT_TRUE(sinkScene_->wfdP2pCallback_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), QueryP2pDevices(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_FAILED));

    OHOS::Wifi::GcInfo info;
    sinkScene_->wfdP2pCallback_->OnP2pGcJoinGroup(info);
}

HWTEST_F(WfdSinkSceneTest, OnP2pGcJoinGroup_002, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);
    ASSERT_TRUE(sinkScene_->wfdP2pCallback_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), QueryP2pDevices(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    OHOS::Wifi::GcInfo info;
    info.ip = "";
    sinkScene_->wfdP2pCallback_->OnP2pGcJoinGroup(info);
}

HWTEST_F(WfdSinkSceneTest, OnP2pGcJoinGroup_003, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);
    ASSERT_TRUE(sinkScene_->wfdP2pCallback_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), QueryP2pDevices(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    OHOS::Wifi::GcInfo info;
    info.ip = "AA:BB:CC:DD:EE:FF";
    sinkScene_->wfdP2pCallback_->OnP2pGcJoinGroup(info);
}

HWTEST_F(WfdSinkSceneTest, OnP2pGcJoinGroup_004, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);
    ASSERT_TRUE(sinkScene_->wfdP2pCallback_ != nullptr);
}

HWTEST_F(WfdSinkSceneTest, OnP2pPeerConnected_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), RemoveGroupClient(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    ConnectionInfo connectionInfo;
    sinkScene_->devConnectionMap_.clear();
    connectionInfo.mac = "AA:BB:CC:DD:EE:F1";
    sinkScene_->devConnectionMap_.emplace(connectionInfo.mac, std::make_shared<ConnectionInfo>(connectionInfo));
    connectionInfo.mac = "AA:BB:CC:DD:EE:F2";
    sinkScene_->devConnectionMap_.emplace(connectionInfo.mac, std::make_shared<ConnectionInfo>(connectionInfo));
    connectionInfo.mac = "AA:BB:CC:DD:EE:F3";
    sinkScene_->devConnectionMap_.emplace(connectionInfo.mac, std::make_shared<ConnectionInfo>(connectionInfo));
    connectionInfo.mac = "AA:BB:CC:DD:EE:F4";
    sinkScene_->devConnectionMap_.emplace(connectionInfo.mac, std::make_shared<ConnectionInfo>(connectionInfo));
    sinkScene_->isSinkRunning_ = true;
    connectionInfo.mac = "AA:BB:CC:DD:EE:FF";
    sinkScene_->OnP2pPeerConnected(connectionInfo);
}

HWTEST_F(WfdSinkSceneTest, OnP2pPeerConnected_002, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    ConnectionInfo connectionInfo;
    connectionInfo.mac = "AA:BB:CC:DD:EE:FF";
    sinkScene_->isSinkRunning_ = true;
    RestoreConnMap();
    sinkScene_->OnP2pPeerConnected(connectionInfo);
}

HWTEST_F(WfdSinkSceneTest, OnP2pPeerConnected_003, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), CreateAgent(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<0>(INVALID_ID), SetArgReferee<1>(INVALID_ID), Return(0)));
    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    ConnectionInfo connectionInfo;
    connectionInfo.mac = "AA:BB:CC:DD:EE:FF";
    sinkScene_->isSinkRunning_ = true;
    sinkScene_->devConnectionMap_.clear();
    sinkScene_->OnP2pPeerConnected(connectionInfo);
}

HWTEST_F(WfdSinkSceneTest, OnP2pPeerConnected_004, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), CreateAgent(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(1), Return(0)));

    ConnectionInfo connectionInfo;
    connectionInfo.mac = "AA:BB:CC:DD:EE:FF";
    sinkScene_->isSinkRunning_ = true;
    sinkScene_->devConnectionMap_.clear();
    sinkScene_->OnP2pPeerConnected(connectionInfo);
}

HWTEST_F(WfdSinkSceneTest, OnP2pPeerDisconnected_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), RemoveGroupClient(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(0));

    RestoreConnMap();
    ConnectionInfo connectionInfo;
    connectionInfo.mac = "AA:BB:CC:DD:EE:FF";
    sinkScene_->OnP2pPeerDisconnected(connectionInfo);
}

HWTEST_F(WfdSinkSceneTest, OnP2pPeerDisconnected_002, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), RemoveGroupClient(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    RestoreConnMap();
    std::string mac = "AA:BB:CC:DD:EE:FF";
    sinkScene_->OnP2pPeerDisconnected(mac);
}

HWTEST_F(WfdSinkSceneTest, HandleAppendSurface_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));


    RestoreConnMap();
    auto msg = std::make_shared<WfdAppendSurfaceReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "";
    int32_t ret = sinkScene_->HandleAppendSurface(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSinkSceneTest, HandleAppendSurface_002, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    RestoreConnMap();
    auto msg = std::make_shared<WfdAppendSurfaceReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "AA:BB:CC:DD:EE:FF";
    int32_t ret = sinkScene_->HandleAppendSurface(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSinkSceneTest, HandleRemoveSurface_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    RestoreConnMap();
    auto msg = std::make_shared<WfdRemoveSurfaceReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "";
    int32_t ret = sinkScene_->HandleRemoveSurface(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSinkSceneTest, HandleRemoveSurface_002, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    RestoreConnMap();
    auto msg = std::make_shared<WfdRemoveSurfaceReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "AA:BB:CC:DD:EE:FF";
    int32_t ret = sinkScene_->HandleRemoveSurface(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSinkSceneTest, HandleSetMediaFormat_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    RestoreConnMap();
    auto msg = std::make_shared<SetMediaFormatReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "";
    int32_t ret = sinkScene_->HandleSetMediaFormat(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSinkSceneTest, HandleSetMediaFormat_002, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    RestoreConnMap();
    auto msg = std::make_shared<SetMediaFormatReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "AA:BB:CC:DD:EE:FF";
    int32_t ret = sinkScene_->HandleSetMediaFormat(msg, reply);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSceneTest, HandleSetSceneType_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    RestoreConnMap();
    auto msg = std::make_shared<SetSceneTypeReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "";
    int32_t ret = sinkScene_->HandleSetSceneType(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSinkSceneTest, HandleSetSceneType_002, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    RestoreConnMap();
    auto msg = std::make_shared<SetSceneTypeReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "AA:BB:CC:DD:EE:FF";
    int32_t ret = sinkScene_->HandleSetSceneType(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSinkSceneTest, HandlePlay_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    RestoreConnMap();
    auto msg = std::make_shared<WfdPlayReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "";
    int32_t ret = sinkScene_->HandlePlay(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSinkSceneTest, HandlePlay_002, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), Start(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), Play(_, _))
        .WillOnce(Return(0));

    RestoreConnMap();
    auto msg = std::make_shared<WfdPlayReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "AA:BB:CC:DD:EE:FF";
    int32_t ret = sinkScene_->HandlePlay(msg, reply);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSceneTest, HandlePlay_003, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), Resume(_, _, _))
        .WillOnce(Return(0));

    ConnectionInfo connectionInfo;
    sinkScene_->devConnectionMap_.clear();
    connectionInfo.contextId = 1;
    connectionInfo.agentId = 1;
    connectionInfo.isRunning = true;
    connectionInfo.mac = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<ConnectionInfo> connectionInfoPtr = std::make_shared<ConnectionInfo>(connectionInfo);
    sinkScene_->devConnectionMap_.emplace(connectionInfo.mac, connectionInfoPtr);
    auto msg = std::make_shared<WfdPlayReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "AA:BB:CC:DD:EE:FF";
    int32_t ret = sinkScene_->HandlePlay(msg, reply);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSceneTest, HandlePause_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    RestoreConnMap();
    auto msg = std::make_shared<WfdPauseReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "";
    int32_t ret = sinkScene_->HandlePause(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSinkSceneTest, HandlePause_002, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), Pause(_, _, _))
        .WillOnce(Return(0));
    
    RestoreConnMap();
    auto msg = std::make_shared<WfdPauseReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "AA:BB:CC:DD:EE:FF";
    int32_t ret = sinkScene_->HandlePause(msg, reply);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSceneTest, HandleMute_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    RestoreConnMap();
    auto msg = std::make_shared<MuteReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "";
    int32_t ret = sinkScene_->HandleMute(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSinkSceneTest, HandleMute_002, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), Pause(_, _, _))
        .WillOnce(Return(0));
    
    RestoreConnMap();
    auto msg = std::make_shared<MuteReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "AA:BB:CC:DD:EE:FF";
    int32_t ret = sinkScene_->HandleMute(msg, reply);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSceneTest, HandleUnMute_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    RestoreConnMap();
    auto msg = std::make_shared<UnMuteReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "";
    int32_t ret = sinkScene_->HandleUnMute(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSinkSceneTest, HandleUnMute_002, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), Resume(_, _, _))
        .WillOnce(Return(0));
    
    RestoreConnMap();
    auto msg = std::make_shared<UnMuteReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "AA:BB:CC:DD:EE:FF";
    int32_t ret = sinkScene_->HandleUnMute(msg, reply);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSceneTest, HandleClose_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    RestoreConnMap();
    auto msg = std::make_shared<WfdCloseReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "";
    int32_t ret = sinkScene_->HandleClose(msg, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(WfdSinkSceneTest, HandleClose_002, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), RemoveGroupClient(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(0));
    
    RestoreConnMap();
    auto msg = std::make_shared<WfdCloseReq>();
    auto reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "AA:BB:CC:DD:EE:FF";
    int32_t ret = sinkScene_->HandleClose(msg, reply);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSceneTest, HandleGetConfig_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    auto msg = std::make_shared<GetSinkConfigReq>();
    auto reply = std::make_shared<GetSinkConfigRsp>();
    int32_t ret = sinkScene_->HandleGetConfig(msg, reply);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WfdSinkSceneTest, ErrorCodeFiltering_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    int32_t code = static_cast<int32_t>(ERR_CONTEXT_AGENT_BASE);
    sinkScene_->ErrorCodeFiltering(code);
}

HWTEST_F(WfdSinkSceneTest, ErrorCodeFiltering_002, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    int32_t code = static_cast<int32_t>(ERR_SESSION_BASE);
    sinkScene_->ErrorCodeFiltering(code);
}

HWTEST_F(WfdSinkSceneTest, ErrorCodeFiltering_003, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    int32_t code = static_cast<int32_t>(ERR_PROSUMER_START);
    sinkScene_->ErrorCodeFiltering(code);
}

HWTEST_F(WfdSinkSceneTest, ErrorCodeFiltering_004, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    int32_t code = static_cast<int32_t>(ERR_PROSUMER_TIMEOUT);
    sinkScene_->ErrorCodeFiltering(code);
}

HWTEST_F(WfdSinkSceneTest, ErrorCodeFiltering_005, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    int32_t code = static_cast<int32_t>(ERR_PROSUMER_DESTROY);
    sinkScene_->ErrorCodeFiltering(code);
}

HWTEST_F(WfdSinkSceneTest, ErrorCodeFiltering_006, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    int32_t code = static_cast<int32_t>(ERR_PROSUMER_INIT);
    sinkScene_->ErrorCodeFiltering(code);
}

HWTEST_F(WfdSinkSceneTest, OnInnerError_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    sinkScene_->OnInnerError(1, 1, SharingErrorCode::ERR_PROSUMER_TIMEOUT, "err msg");
}

HWTEST_F(WfdSinkSceneTest, OnInnerError_002, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    sinkScene_->OnInnerError(1, 1, SharingErrorCode::ERR_PROSUMER_INIT, "err msg");
}

HWTEST_F(WfdSinkSceneTest, OnInnerError_003, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    sinkScene_->OnInnerError("", SharingErrorCode::ERR_PROSUMER_INIT, "err msg");
}

HWTEST_F(WfdSinkSceneTest, OnInnerDestroy_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), RemoveGroupClient(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    RestoreConnMap();
    sinkScene_->OnInnerDestroy(1, 1, AgentType::SINK_AGENT);
}

HWTEST_F(WfdSinkSceneTest, OnInnerEvent_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    SharingEvent event;
    auto msg = std::make_shared<WfdSceneEventMsg>();
    msg->mac = "";
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_PLAYED;
    event.eventMsg = msg;
    RestoreConnMap();
    sinkScene_->OnInnerEvent(event);
}

HWTEST_F(WfdSinkSceneTest, OnInnerEvent_002, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    SharingEvent event;
    auto msg = std::make_shared<WfdSceneEventMsg>();
    msg->mac = "AA:BB:CC:DD:EE:FF";
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_PLAYED;
    event.eventMsg = msg;
    RestoreConnMap();
    sinkScene_->OnInnerEvent(event);
}

HWTEST_F(WfdSinkSceneTest, OnInnerEvent_003, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockWifiP2pInstance_), RemoveGroupClient(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(0));

    SharingEvent event;
    auto msg = std::make_shared<WfdSceneEventMsg>();
    msg->mac = "AA:BB:CC:DD:EE:FF";;
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_TEARDOWN;
    event.eventMsg = msg;
    RestoreConnMap();
    sinkScene_->OnInnerEvent(event);
}

HWTEST_F(WfdSinkSceneTest, OnInnerEvent_004, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    SharingEvent event;
    auto msg = std::make_shared<WfdSceneEventMsg>();
    msg->mac = "AA:BB:CC:DD:EE:FF";
    msg->type = EventType::EVENT_INTERACTION_ACCELERATION_DONE;
    msg->contextId = 1;
    msg->agentId = 1;
    event.eventMsg = msg;
    RestoreConnMap();
    sinkScene_->OnInnerEvent(event);
}

HWTEST_F(WfdSinkSceneTest, OnInnerEvent_005, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    SharingEvent event;
    auto msg = std::make_shared<WfdSceneEventMsg>();
    msg->mac = "AA:BB:CC:DD:EE:FF";
    msg->type = EventType::EVENT_INTERACTION_STATE_REMOVE_SURFACE;
    msg->contextId = 1;
    msg->agentId = 1;
    event.eventMsg = msg;
    sinkScene_->OnInnerEvent(event);
}

HWTEST_F(WfdSinkSceneTest, OnInnerEvent_006, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(0));

    SharingEvent event;
    auto msg = std::make_shared<WfdSceneEventMsg>();
    msg->mac = "AA:BB:CC:DD:EE:FF";
    msg->type = EventType::EVENT_INTERACTION_DECODER_DIED;
    msg->surfaceId = 0;
    msg->contextId = 1;
    msg->agentId = 1;
    event.eventMsg = msg;
    RestoreSurfaceMap();
    RestoreConnMap();
    sinkScene_->OnInnerEvent(event);
}

HWTEST_F(WfdSinkSceneTest, OnRemoteDied_001, TestSize.Level1)
{
    ASSERT_TRUE(sinkScene_ != nullptr);

    EXPECT_CALL(*(sinkScene_->mockSharingAdapter_), ReleaseScene(_));
    sinkScene_->OnRemoteDied();
}

} // namespace Sharing
} // namespace OHOS

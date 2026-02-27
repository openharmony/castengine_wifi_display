/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#include "wfd_source_scene_test.h"
#include "common/sharing_log.h"
#include "wfd_session_def.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

constexpr uint32_t TEST_CONTEXT_ID = 1;       // Test context ID for agent creation
constexpr uint32_t TEST_AGENT_ID = 2;         // Test agent ID for agent creation
constexpr uint32_t TEST_INVALID_CONTEXT_ID = 0;  // Invalid context ID for error testing
constexpr uint32_t TEST_INVALID_AGENT_ID = 0;    // Invalid agent ID for error testing
constexpr int32_t TEST_SUCCESS = 0;           // Success return value
constexpr int32_t TEST_FAILURE = -1;          // Failure return value
constexpr int32_t TEST_WIFI_SUCCESS = 0;     // WiFi operation success code
constexpr int32_t TEST_WIFI_FAILED = -1;      // WiFi operation failure code

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

/**
 * @tc.name: Initialize
 * @tc.desc: Test Initialize function - basic initialization
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, Initialize, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
        
    sourceScene_->Initialize();
    sourceScene_->MockInitialize();
}

/**
 * @tc.name: HandleStartDiscovery
 * @tc.desc: Test HandleStartDiscovery with P2P_STATE_NONE - should return error
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleStartDiscoveryNone, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<WfdSourceStartDiscoveryReq> msg = std::make_shared<WfdSourceStartDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), GetP2pEnableStatus(_))
        .WillOnce(DoAll(SetArgReferee<0>(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_NONE)),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    int32_t ret = sourceScene_->HandleStartDiscovery(msg, reply);
    EXPECT_EQ(ret, TEST_FAILURE);
}

/**
 * @tc.name: HandleStartDiscoveryIdle
 * @tc.desc: Test HandleStartDiscovery with P2P_STATE_IDLE - should return error
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleStartDiscoveryIdle, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<WfdSourceStartDiscoveryReq> msg = std::make_shared<WfdSourceStartDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), GetP2pEnableStatus(_))
        .WillOnce(DoAll(SetArgReferee<0>(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_IDLE)),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    int32_t ret = sourceScene_->HandleStartDiscovery(msg, reply);
    EXPECT_EQ(ret, TEST_FAILURE);
}

/**
 * @tc.name: HandleStartDiscoveryStarting
 * @tc.desc: Test HandleStartDiscovery with P2P_STATE_STARTING - should return error
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleStartDiscoveryStarting, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<WfdSourceStartDiscoveryReq> msg = std::make_shared<WfdSourceStartDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), GetP2pEnableStatus(_))
        .WillOnce(DoAll(SetArgReferee<0>(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_STARTING)),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    int32_t ret = sourceScene_->HandleStartDiscovery(msg, reply);
    EXPECT_EQ(ret, TEST_FAILURE);
}

/**
 * @tc.name: HandleStartDiscoveryClosing
 * @tc.desc: Test HandleStartDiscovery with P2P_STATE_CLOSING - should return error
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleStartDiscoveryClosing, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<WfdSourceStartDiscoveryReq> msg = std::make_shared<WfdSourceStartDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), GetP2pEnableStatus(_))
        .WillOnce(DoAll(SetArgReferee<0>(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_CLOSING)),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    int32_t ret = sourceScene_->HandleStartDiscovery(msg, reply);
    EXPECT_EQ(ret, TEST_FAILURE);
}

/**
 * @tc.name: HandleStartDiscoveryStarted
 * @tc.desc: Test HandleStartDiscovery with P2P_STATE_STARTED - should succeed
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleStartDiscoveryStarted, TestSize.Level1)
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
    EXPECT_EQ(ret, TEST_SUCCESS);
}

/**
 * @tc.name: HandleStartDiscoveryClosed
 * @tc.desc: Test HandleStartDiscovery with P2P_STATE_CLOSED - should enable P2P and succeed
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleStartDiscoveryClosed, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<WfdSourceStartDiscoveryReq> msg = std::make_shared<WfdSourceStartDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), GetP2pEnableStatus(_))
        .WillOnce(DoAll(SetArgReferee<0>(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_CLOSED)),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mock(mockWifiP2pInstance_), EnableP2p())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), DiscoverDevices())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    int32_t ret = sourceScene_->HandleStartDiscovery(msg, reply);
    EXPECT_EQ(ret, TEST_SUCCESS);
}

/**
 * @tc.name: HandleStartDiscoveryNullInstance
 * @tc.desc: Test HandleStartDiscovery with p2pInstance null - should return error
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleStartDiscoveryNullInstance, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->p2pInstance_ = nullptr;
    std::shared_ptr<WfdSourceStartDiscoveryReq> msg = std::make_shared<WfdSourceStartDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();

    int32_t ret = sourceScene_->HandleStartDiscovery(msg, reply);
    EXPECT_EQ(ret, TEST_FAILURE);
}

/**
 * @tc.name: HandleStartDiscoveryRunning
 * @tc.desc: Test HandleStartDiscovery when already running - should skip state check
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleStartDiscoveryRunning, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->isSourceRunning_ = true;
    std::shared_ptr<WfdSourceStartDiscoveryReq> msg = std::make_shared<WfdSourceStartDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), DiscoverDevices())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    int32_t ret = sourceScene_->HandleStartDiscovery(msg, reply);
    EXPECT_EQ(ret, TEST_SUCCESS);
}

/**
 * @tc.name: OnP2pStateChangedNone
 * @tc.desc: Test OnP2pStateChanged with P2P_STATE_NONE - no action expected
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pStateChangedNone, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_NONE));
}

/**
 * @tc.name: OnP2pStateChangedIdle
 * @tc.desc: Test OnP2pStateChanged with P2P_STATE_IDLE - no action expected
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pStateChangedIdle, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_IDLE));
}

/**
 * @tc.name: OnP2pStateChangedStarting
 * @tc.desc: Test OnP2pStateChanged with P2P_STATE_STARTING - no action expected
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pStateChangedStarting, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_STARTING));
}

/**
 * @tc.name: OnP2pStateChangedStarted
 * @tc.desc: Test OnP2pStateChanged with P2P_STATE_STARTED and isSourceRunning true - should call WfdP2pStart
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pStateChangedStarted, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    sourceScene_->isSourceRunning_ = true;
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), SetP2pWfdInfo(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), DiscoverDevices())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_STARTED));
}

/**
 * @tc.name: OnP2pStateChangedClosing
 * @tc.desc: Test OnP2pStateChanged with P2P_STATE_CLOSING - no action expected
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pStateChangedClosing, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_CLOSING));
}

/**
 * @tc.name: OnP2pStateChangedClosed
 * @tc.desc: Test OnP2pStateChanged with P2P_STATE_CLOSED and isSourceRunning true - should stop and report error
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pStateChangedClosed, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    sourceScene_->isSourceRunning_ = true;
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup()).Times(1)
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
))
        .WillOnce(Return(TEST_SUCCESS));
    
    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_CLOSED));
}

/**
 * @tc.name: OnP2pStateChangedClosedNotRunning
 * @tc.desc: Test OnP2pStateChanged with P2P_STATE_CLOSED and isSourceRunning false - no action expected
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pStateChangedClosedNotRunning, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    sourceScene_->isSourceRunning_ = false;
    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_CLOSED));
}

/**
 * @tc.name: OnDeviceFound_001
 * @tc.desc: Test OnDeviceFound with empty device list
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnDeviceFound_001, TestSize.Level1)
{
    std::vector<WfdCastDeviceInfo> deviceInfos;
    sourceScene_->OnDeviceFound(deviceInfos);
}

/**
 * @tc.name: OnP2pPeerConnected_001
 * @tc.desc: Test OnP2pPeerConnected with initial state - should trigger connection changed
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pPeerConnected_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    ConnectionInfo connectionInfo;
    connectionInfo.mac = "AA.BB.CC.DD.EE.FF";
    connectionInfo.state = ConnectionState::INIT;
    sourceScene_->OnP2pPeerConnected(connectionInfo);
}

/**
 * @tc.name: OnP2pPeerConnected_002
 * @tc.desc: Test OnP2pPeerConnected with null sharing adapter - no action expected
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pPeerConnected_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->sharingAdapter_.reset();
    ConnectionInfo connectionInfo;
    connectionInfo.mac = "AA.BB.CC.DD.EE.FF";
    sourceScene_->OnP2pPeerConnected(connectionInfo);
}

/**
 * @tc.name: OnP2pPeerConnected_003
 * @tc.desc: Test OnP2pPeerConnected when source not running - should return early
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pPeerConnected_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->isSourceRunning_ = false;
    ConnectionInfo connectionInfo;
    connectionInfo.mac = "AA.BB.CC.DD.EE.FF";

    sourceScene_->OnP2pPeerConnected(connectionInfo);
}

/**
 * @tc.name: OnP2pPeerConnected_004
 * @tc.desc: Test OnP2pPeerConnected when already connected - should return early
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pPeerConnected_004, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    ConnectionInfo connectionInfo;
    connectionInfo.mac = "AA.BB.CC.DD.EE.FF";
    connectionInfo.state = ConnectionState::CONNECTED;
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), CreateAgent(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<0>(TEST_CONTEXT_ID), SetArgReferee<1>(TEST_AGENT_ID), Return(TEST_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), Start(_, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), CreateAgent(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<0>(TEST_CONTEXT_ID + 1), SetArgReferee<1>(TEST_AGENT_ID + 1), Return(TEST_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), Start(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    sourceScene_->OnP2pPeerConnected(connectionInfo);
}

/**
 * @tc.name: OnP2pPeersChanged_001
 * @tc.desc: Test OnP2pPeersChanged with available device and session available - should report device found
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pPeersChanged_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    sourceScene_->isSourceDiscovering = true;
    std::vector<Wifi::WifiP2pDevice> devices;
    Wifi::WifiP2pDevice device;
    device.SetP2pDeviceStatus(Wifi::P2pDeviceStatus::PDS_AVAILABLE);
    Wifi::WifiP2pWfdInfo info;
    info.setSessionAvailable(true);
    device.SetWfdInfo(info);
    devices.emplace_back(device);
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pPeersChanged(devices);
}

/**
 * @tc.name: OnP2pPeersChanged_002
 * @tc.desc: Test OnP2pPeersChanged with unavailable device - should not report
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pPeersChanged_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    sourceScene_->isSourceDiscovering = true;
    std::vector<Wifi::WifiP2pDevice> devices;
    Wifi::WifiP2pDevice device;
    device.SetP2pDeviceStatus(Wifi::P2pDeviceStatus::PDS_UNAVAILABLE);
    devices.emplace_back(device);

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pPeersChanged(devices);
}

/**
 * @tc.name: OnP2pPeersChanged_003
 * @tc.desc: Test OnP2pPeersChanged with session not available - should not report
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pPeersChanged_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    sourceScene_->isSourceDiscovering = true;
    std::vector<Wifi::WifiP2pDevice> devices;
    Wifi::WifiP2pDevice device;
    device.SetP2pDeviceStatus(Wifi::P2pDeviceStatus::PDS_AVAILABLE);
    Wifi::WifiP2pWfdInfo info;
    info.setSessionAvailable(false);
    device.SetWfdInfo(info);
    devices.emplace_back(device);

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pPeersChanged(devices);
}

/**
 * @tc.name: OnP2pPeersChanged_004
 * @tc.desc: Test OnP2pPeersChanged with empty device list
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pPeersChanged_004, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    sourceScene_->isSourceDiscovering = true;
    std::vector<Wifi::WifiP2pDevice> devices;

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pPeersChanged(devices);
}

/**
 * @tc.name: OnP2pPeersChanged_005
 * @tc.desc: Test OnP2pPeersChanged when not discovering - should return early
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pPeersChanged_005, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    sourceScene_->isSourceDiscovering = false;
    std::vector<Wifi::WifiP2pDevice> devices;

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pPeersChanged(devices);
}

/**
 * @tc.name: OnP2pConnectionChanged_001
 * @tc.desc: Test OnP2pConnectionChanged with connected state and valid group owner
 * @tc.type: FUNC
 */
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
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pConnectionChanged(info);
}

/**
 * @tc.name: OnP2pConnectionChanged_002
 * @tc.desc: Test OnP2pConnectionChanged with disconnected state
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pConnectionChanged_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    Wifi::WifiP2pLinkedInfo info;
    info.SetConnectState(Wifi::P2pConnectedState::P2P_DISCONNECTED);

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pConnectionChanged(info);
}

/**
 * @tc.name: OnP2pConnectionChanged_003
 * @tc.desc: Test OnP2pConnectionChanged with connected state but empty group owner address
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pConnectionChanged_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    Wifi::WifiP2pLinkedInfo info;
    info.SetConnectState(Wifi::P2pConnectedState::P2P_CONNECTED);
    info.SetIsGroupOwnerAddress("");

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pConnectionChanged(info);
}

/**
 * @tc.name: OnP2pConnectionChanged_004
 * @tc.desc: Test OnP2pConnectionChanged with GetCurrentGroup failure
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pConnectionChanged_004, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    Wifi::WifiP2pLinkedInfo info;
    info.SetConnectState(Wifi::P2pConnectedState::P2P_CONNECTED);
    info.SetIsGroupOwnerAddress("1.1.1.1");

    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), GetCurrentGroup(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_FAILED));

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pConnectionChanged(info);
}

/**
 * @tc.name: OnP2pConnectionChanged_005
 * @tc.desc: Test OnP2pConnectionChanged with null p2pInstance
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pConnectionChanged_005, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    sourceScene_->p2pInstance_ = nullptr;
    Wifi::WifiP2pLinkedInfo info;
    info.SetConnectState(Wifi::P2pConnectedState::P2P_CONNECTED);

    sptr<MockWfdSourceScene::WfdP2pCallback> wfdP2pCallback = sourceScene_->wfdP2pCallback_;
    wfdP2pCallback->OnP2pConnectionChanged(info);
}

/**
 * @tc.name: SetCheckWfdConnectionTimer_001
 * @tc.desc: Test SetCheckWfdConnectionTimer - should register and setup timer
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, SetCheckWfdConnectionTimer_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->SetCheckWfdConnectionTimer();
}

/**
 * @tc.name: OnCheckWfdConnection_001
 * @tc.desc: Test OnCheckWfdConnection - should reset timer and report timeout error
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnCheckWfdConnection_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    sourceScene_->OnCheckWfdConnection();
}

/**
 * @tc.name: ResetCheckWfdConnectionTimer_001
 * @tc.desc: Test ResetCheckWfdConnectionTimer after setting timer
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, ResetCheckWfdConnectionTimer_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->SetCheckWfdConnectionTimer();
    sourceScene_->ResetCheckWfdConnectionTimer();
}

/**
 * @tc.name: ResetCheckWfdConnectionTimer_002
 * @tc.desc: Test ResetCheckWfdConnectionTimer without setting timer
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, ResetCheckWfdConnectionTimer_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->ResetCheckWfdConnectionTimer();
}

/**
 * @tc.name: HandleStopDiscovery_001
 * @tc.desc: Test HandleStopDiscovery when source is running
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleStopDiscovery_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->isSourceRunning_ = true;
    std::shared_ptr<WfdSourceStopDiscoveryReq> msg = std::make_shared<WfdSourceStopDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), StopDiscoverDevices())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    sourceScene_->HandleStopDiscovery(msg, reply);
}

/**
 * @tc.name: HandleStopDiscovery_002
 * @tc.desc: Test HandleStopDiscovery when source is not running - should return error
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleStopDiscovery_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->isSourceRunning_ = false;
    std::shared_ptr<WfdSourceStopDiscoveryReq> msg = std::make_shared<WfdSourceStopDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();

    int32_t ret = sourceScene_->HandleStopDiscovery(msg, reply);
    EXPECT_EQ(ret, TEST_FAILURE);
}

/**
 * @tc.name: HandleStopDiscovery_003
 * @tc.desc: Test HandleStopDiscovery with null p2pInstance - should return error
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleStopDiscovery_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->isSourceRunning_ = true;
    sourceScene_->p2pInstance_ = nullptr;
    std::shared_ptr<WfdSourceStopDiscoveryReq> msg = std::make_shared<WfdSourceStopDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();

    int32_t ret = sourceScene_->HandleStopDiscovery(msg, reply);
    EXPECT_EQ(ret, TEST_FAILURE);
}

/**
 * @tc.name: HandleAddDevice_001
 * @tc.desc: Test HandleAddDevice with valid screenId
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleAddDevice_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->isSourceRunning_ = true;
    std::shared_ptr<WfdSourceAddDeviceReq> msg = std::make_shared<WfdSourceAddDeviceReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    msg->screenId = 0;
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), P2pConnect(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    int32_t ret = sourceScene_->HandleAddDevice(msg, reply);
    EXPECT_EQ(ret, TEST_WIFI_SUCCESS);
}

/**
 * @tc.name: HandleAddDevice_002
 * @tc.desc: Test HandleAddDevice with invalid screenId - should return error
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleAddDevice_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->isSourceRunning_ = true;
    std::shared_ptr<WfdSourceAddDeviceReq> msg = std::make_shared<WfdSourceAddDeviceReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    msg->screenId = -1;

    int32_t ret = sourceScene_->HandleAddDevice(msg, reply);
    EXPECT_EQ(ret, ERR_BAD_PARAMETER);
}

/**
 * @tc.name: HandleAddDevice_003
 * @tc.desc: Test HandleAddDevice when source not running - should return error
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleAddDevice_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->isSourceRunning_ = false;
    std::shared_ptr<WfdSourceAddDeviceReq> msg = std::make_shared<WfdSourceAddDeviceReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();

    int32_t ret = sourceScene_->HandleAddDevice(msg, reply);
    EXPECT_EQ(ret, TEST_FAILURE);
}

/**
 * @tc.name: HandleAddDevice_004
 * @tc.desc: Test HandleAddDevice with null p2pInstance - should return error
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleAddDevice_004, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->isSourceRunning_ = true;
    sourceScene_->p2pInstance_ = nullptr;
    std::shared_ptr<WfdSourceAddDeviceReq> msg = std::make_shared<WfdSourceAddDeviceReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();

    int32_t ret = sourceScene_->HandleAddDevice(msg, reply);
    EXPECT_EQ(ret, TEST_FAILURE);
}

/**
 * @tc.name: CreateScreenCapture_001
 * @tc.desc: Test CreateScreenCapture with agent creation failure
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, CreateScreenCapture_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), CreateAgent(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<0>(TEST_INVALID_CONTEXT_ID), SetArgReferee<1>(TEST_INVALID_AGENT_ID), Return(TEST_SUCCESS)));

    int32_t ret = sourceScene_->CreateScreenCapture();
    EXPECT_EQ(ret, ERR_AGENT_CREATE);
}

/**
 * @tc.name: CreateScreenCapture_002
 * @tc.desc: Test CreateScreenCapture with successful agent creation
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, CreateScreenCapture_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), CreateAgent(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<0>(TEST_CONTEXT_ID), SetArgReferee<1>(TEST_AGENT_ID), Return(TEST_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), Start(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    int32_t ret = sourceScene_->CreateScreenCapture();
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: CreateScreenCapture_003
 * @tc.desc: Test CreateScreenCapture with null sharing adapter
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, CreateScreenCapture_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->sharingAdapter_.reset();

    int32_t ret = sourceScene_->CreateScreenCapture();
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: HandleDestroyScreenCapture_001
 * @tc.desc: Test HandleDestroyScreenCapture
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleDestroyScreenCapture_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<DestroyScreenCaptureReq> msg = std::make_shared<DestroyScreenCaptureReq>();
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    int32_t ret = sourceScene_->HandleDestroyScreenCapture(msg);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: HandleDestroyScreenCapture_002
 * @tc.desc: Test HandleDestroyScreenCapture with null sharing adapter
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleDestroyScreenCapture_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->sharingAdapter_.reset();
    std::shared_ptr<DestroyScreenCaptureReq> msg = std::make_shared<DestroyScreenCaptureReq>();

    int32_t ret = sourceScene_->HandleDestroyScreenCapture(msg);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: AppendCast_001
 * @tc.desc: Test AppendCast with valid device
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, AppendCast_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), CreateAgent(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<0>(TEST_CONTEXT_ID), SetArgReferee<1>(TEST_AGENT_ID), Return(TEST_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), Start(_, _))
        .WillOnce(Return(TEST_SUCCESS));
    
    int32_t ret = sourceScene_->AppendCast("AA.BB.CC.DD.EE.FF");
    EXPECT_EQ(ret, TEST_SUCCESS);
}

/**
 * @tc.name: AppendCast_002
 * @tc.desc: Test AppendCast with empty deviceId - should return error
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, AppendCast_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    int32_t ret = sourceScene_->AppendCast("");
    EXPECT_EQ(ret, TEST_FAILURE);
}

/**
 * @tc.name: AppendCast_003
 * @tc.desc: Test AppendCast when device already running - should return success
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, AppendCast_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), CreateAgent(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<0>(TEST_CONTEXT_ID), SetArgReferee<1>(TEST_AGENT_ID), Return(TEST_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), Start(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    BackupConnDev();
    sourceScene_->connDev_->isRunning = true;
    int32_t ret = sourceScene_->AppendCast("AA.BB.CC.DD.EE.FF");
    RestoreConnDev();
    EXPECT_EQ(ret, TEST_SUCCESS);
}

/**
 * @tc.name: AppendCast_004
 * @tc.desc: Test AppendCast with agent creation failure
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, AppendCast_004, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), CreateAgent(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<0>(TEST_INVALID_CONTEXT_ID), SetArgReferee<1>(TEST_INVALID_AGENT_ID), Return(TEST_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    BackupConnDev();
    int32_t ret = sourceScene_->AppendCast("AA.BB.CC.DD.EE.FF");
    RestoreConnDev();
    EXPECT_EQ(ret, TEST_FAILURE);
}

/**
 * @tc.name: AppendCast_005
 * @tc.desc: Test AppendCast with null sharing adapter
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, AppendCast_005, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->sharingAdapter_.reset();

    int32_t ret = sourceScene_->AppendCast("AA.BB.CC.DD.EE.FF");
    EXPECT_EQ(ret, TEST_FAILURE);
}

/**
 * @tc.name: WfdP2pStart_001
 * @tc.desc: Test WfdP2pStart with valid p2pInstance
 * @tc.type: FUNC
 */
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

/**
 * @tc.name: WfdP2pStart_002
 * @tc.desc: Test WfdP2pStart with null p2pInstance
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, WfdP2pStart_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->p2pInstance_ = nullptr;

    sourceScene_->WfdP2pStart();
}

/**
 * @tc.name: WfdP2pStop_001
 * @tc.desc: Test WfdP2pStop with connected device
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, WfdP2pStop_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup()).Times(1)
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    sourceScene_->WfdP2pStop();
}

/**
 * @tc.name: WfdP2pStop_002
 * @tc.desc: Test WfdP2pStop with null sharing adapter
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, WfdP2pStop_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->sharingAdapter_.reset();
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    sourceScene_->WfdP2pStop();
}

/**
 * @tc.name: OnConnectionChanged_001
 * @tc.desc: Test OnConnectionChanged with initial state
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnConnectionChanged_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    ConnectionInfo connectionInfo;
    connectionInfo.state = ConnectionState::INIT;

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    sourceScene_->OnConnectionChanged(connectionInfo);
}

/**
 * @tc.name: OnConnectionChanged_002
 * @tc.desc: Test OnConnectionChanged with connected state - should create screen capture and append cast
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnConnectionChanged_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    ConnectionInfo connectionInfo;
    connectionInfo.state = ConnectionState::CONNECTED;
    connectionInfo.mac = "AA.BB.CC.DD.EE.FF";
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), CreateAgent(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<0>(TEST_CONTEXT_ID), SetArgReferee<1>(TEST_AGENT_ID), Return(TEST_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), Start(_, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), CreateAgent(_, _, _, _))
        .WillOnce(
            DoAll(SetArgReferee<0>(TEST_CONTEXT_ID + 1), SetArgReferee<1>(TEST_AGENT_ID + 1), Return(TEST_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), Start(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    sourceScene_->OnConnectionChanged(connectionInfo);
}

/**
 * @tc.name: OnConnectionChanged_003
 * @tc.desc: Test OnConnectionChanged with connected state but screen capture failure
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnConnectionChanged_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    ConnectionInfo connectionInfo;
    connectionInfo.state = ConnectionState::CONNECTED;
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .Times(2)
        .WillRepeatedly(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), CreateAgent(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<0>(TEST_INVALID_CONTEXT_ID), SetArgReferee<1>(TEST_INVALID_AGENT_ID),
                        Return(TEST_SUCCESS)));

    sourceScene_->OnConnectionChanged(connectionInfo);
}

/**
 * @tc.name: OnConnectionChanged_004
 * @tc.desc: Test OnConnectionChanged with null ipc adapter
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnConnectionChanged_004, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->ipcAdapter_.reset();
    ConnectionInfo connectionInfo;
    connectionInfo.state = ConnectionState::CONNECTED;

    sourceScene_->OnConnectionChanged(connectionInfo);
}

/**
 * @tc.name: OnP2pPeerDisconnected_001
 * @tc.desc: Test OnP2pPeerDisconnected with empty mac
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pPeerDisconnected_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    ConnectionInfo connectionInfo;
    connectionInfo.mac = "";

    sourceScene_->OnP2pPeerDisconnected(connectionInfo);
}

/**
 * @tc.name: OnP2pPeerDisconnected_002
 * @tc.desc: Test OnP2pPeerDisconnected with empty mac (string version)
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pPeerDisconnected_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->OnP2pPeerDisconnected("");
}

/**
 * @tc.name: OnP2pPeerDisconnected_003
 * @tc.desc: Test OnP2pPeerDisconnected with valid device
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pPeerDisconnected_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    ConnectionInfo connectionInfo;
    connectionInfo.mac = "AA.BB.CC.DD.EE.FF";

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup()).Times(1)
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    BackupConnDev();
    sourceScene_->OnP2pPeerDisconnected(connectionInfo);
    RestoreConnDev();
}

/**
 * @tc.name: OnP2pPeerDisconnected_004
 * @tc.desc: Test OnP2pPeerDisconnected with valid device (string version)
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pPeerDisconnected_004, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup()).Times(1)
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    BackupConnDev();
    sourceScene_->OnP2pPeerDisconnected("AA.BB.CC.DD.EE.FF");
    RestoreConnDev();
}

/**
 * @tc.name: OnP2pPeerDisconnected_005
 * @tc.desc: Test OnP2pPeerDisconnected with invalid contextId/agentId
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pPeerDisconnected_005, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup()).Times(1)
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    BackupConnDev();
    sourceScene_->contextId_ = TEST_INVALID_CONTEXT_ID;
    sourceScene_->OnP2pPeerDisconnected("AA.BB.CC.DD.EE.FF");
    RestoreConnDev();
}

/**
 * @tc.name: OnP2pPeerDisconnected_006
 * @tc.desc: Test OnP2pPeerDisconnected with null sharing adapter
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pPeerDisconnected_006, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup()).Times(1)
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    BackupConnDev();
    sourceScene_->sharingAdapter_.reset();
    sourceScene_->OnP2pPeerDisconnected("AA.BB.CC.DD.EE.FF");
    RestoreConnDev();
}

/**
 * @tc.name: OnP2pThisDeviceChanged_001
 * @tc.desc: Test OnP2pThisDeviceChanged with disconnected state
 * @tc.type: FUNC
 */
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
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup()).Times(1)
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    BackupConnDev();
    sourceScene_->wfdP2pCallback_->OnP2pThisDeviceChanged(device);
    RestoreConnDev();
}

/**
 * @tc.name: OnP2pThisDeviceChanged_002
 * @tc.desc: Test OnP2pThisDeviceChanged with connected state
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pThisDeviceChanged_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    Wifi::WifiP2pDevice device;
    Wifi::WifiP2pLinkedInfo info;
    info.SetConnectState(Wifi::P2pConnectedState::P2P_CONNECTED);
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), QueryP2pLinkedInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(info),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));

    sourceScene_->wfdP2pCallback_->OnP2pThisDeviceChanged(device);
}

/**
 * @tc.name: OnP2pThisDeviceChanged_003
 * @tc.desc: Test OnP2pThisDeviceChanged with QueryP2pLinkedInfo failure
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pThisDeviceChanged_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    Wifi::WifiP2pDevice device;
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), QueryP2pLinkedInfo(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_FAILED));

    sourceScene_->wfdP2pCallback_->OnP2pThisDeviceChanged(device);
}

/**
 * @tc.name: OnP2pThisDeviceChanged_004
 * @tc.desc: Test OnP2pThisDeviceChanged with null p2pInstance
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pThisDeviceChanged_004, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    sourceScene_->p2pInstance_ = nullptr;
    Wifi::WifiP2pDevice device;

    sourceScene_->wfdP2pCallback_->OnP2pThisDeviceChanged(device);
}

/**
 * @tc.name: OnP2pThisDeviceChanged_005
 * @tc.desc: Test OnP2pThisDeviceChanged with null scene
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pThisDeviceChanged_005, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);

    sourceScene_->wfdP2pCallback_->scene_.reset();
    Wifi::WifiP2pDevice device;

    sourceScene_->wfdP2pCallback_->OnP2pThisDeviceChanged(device);
}

/**
 * @tc.name: ErrorCodeFiltering_001
 * @tc.desc: Test ErrorCodeFiltering with ERR_PROSUMER_START
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, ErrorCodeFiltering_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    int32_t code = static_cast<int32_t>(ERR_PROSUMER_START);
    sourceScene_->ErrorCodeFiltering(code);
}

/**
 * @tc.name: ErrorCodeFiltering_002
 * @tc.desc: Test ErrorCodeFiltering with ERR_PROSUMER_TIMEOUT
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, ErrorCodeFiltering_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    int32_t code = static_cast<int32_t>(ERR_PROSUMER_TIMEOUT);
    sourceScene_->ErrorCodeFiltering(code);
}

/**
 * @tc.name: ErrorCodeFiltering_003
 * @tc.desc: Test ErrorCodeFiltering with ERR_PROSUMER_DESTROY
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, ErrorCodeFiltering_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    int32_t code = static_cast<int32_t>(ERR_PROSUMER_DESTROY);
    sourceScene_->ErrorCodeFiltering(code);
}

/**
 * @tc.name: ErrorCodeFiltering_004
 * @tc.desc: Test ErrorCodeFiltering with ERR_CONTEXT_AGENT_BASE
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, ErrorCodeFiltering_004, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    int32_t code = static_cast<int32_t>(SharingErrorCode::ERR_CONTEXT_AGENT_BASE);
    sourceScene_->ErrorCodeFiltering(code);
}

/**
 * @tc.name: ErrorCodeFiltering_005
 * @tc.desc: Test ErrorCodeFiltering with ERR_SESSION_BASE
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, ErrorCodeFiltering_005, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    int32_t code = static_cast<int32_t>(SharingErrorCode::ERR_SESSION_BASE);
    sourceScene_->ErrorCodeFiltering(code);
}

/**
 * @tc.name: OnInnerError_001
 * @tc.desc: Test OnInnerError with ERR_PROSUMER_TIMEOUT
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnInnerError_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    sourceScene_->OnInnerError(TEST_CONTEXT_ID, TEST_AGENT_ID, SharingErrorCode::ERR_PROSUMER_TIMEOUT, "error");
}

/**
 * @tc.name: OnInnerError_002
 * @tc.desc: Test OnInnerError with other error code
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnInnerError_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    sourceScene_->OnInnerError(TEST_CONTEXT_ID, TEST_AGENT_ID, SharingErrorCode::ERR_PROSUMER_INIT, "error");
}

/**
 * @tc.name: OnInnerError_003
 * @tc.desc: Test OnInnerError with null ipc adapter
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnInnerError_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->ipcAdapter_.reset();

    sourceScene_->OnInnerError(TEST_CONTEXT_ID, TEST_AGENT_ID, SharingErrorCode::ERR_PROSUMER_INIT, "error");
}

/**
 * @tc.name: OnInnerDestroy_001
 * @tc.desc: Test OnInnerDestroy with matching contextId and agentId
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnInnerDestroy_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    sourceScene_->OnInnerDestroy(TEST_CONTEXT_ID, TEST_AGENT_ID, AgentType::SRC_AGENT);
}

/**
 * @tc.name: OnInnerDestroy_002
 * @tc.desc: Test OnInnerDestroy with non-matching contextId/agentId
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnInnerDestroy_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->OnInnerDestroy(TEST_CONTEXT_ID + 10, TEST_AGENT_ID + 10, AgentType::SRC_AGENT);
}

/**
 * @tc.name: OnInnerDestroy_003
 * @tc.desc: Test OnInnerDestroy with null connDev

 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnInnerDestroy_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->connDev_.reset();

    sourceScene_->OnInnerDestroy(TEST_CONTEXT_ID, TEST_AGENT_ID, AgentType::SRC_AGENT);
}

/**
 * @tc.name: OnInnerEvent_001
 * @tc.desc: Test OnInnerEvent with EVENT_WFD_NOTIFY_RTSP_PLAYED and empty mac
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnInnerEvent_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    SharingEvent event;
    auto msg = std::make_shared<WfdSceneEventMsg>();
    msg->mac = "";
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_PLAYED;
    event.eventMsg = msg;

;

    sourceScene_->OnInnerEvent(event);
}

/**
 * @tc.name: OnInnerEvent_002
 * @tc.desc: Test OnInnerEvent with EVENT_WFD_NOTIFY_RTSP_PLAYED and valid mac
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnInnerEvent_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    SharingEvent event;
    auto msg = std::make_shared<WfdSceneEventMsg>();
    msg->mac = "AA.BB.CC.DD.EE.FF";
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_PLAYED;
    event.eventMsg = msg;

    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    sourceScene_->OnInnerEvent(event);
}

/**
 * @tc.name: OnInnerEvent_003
 * @tc.desc: Test OnInnerEvent with EVENT_WFD_NOTIFY_RTSP_TEARDOWN and empty mac
 * @tc.type: FUNC
 */
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

/**
 * @tc.name: OnInnerEvent_004
 * @tc.desc: Test OnInnerEvent with EVENT_WFD_NOTIFY_RTSP_TEARDOWN and valid mac
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnInnerEvent_004, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    SharingEvent event;
    auto msg = std::make_shared<WfdSceneEventMsg>();
    msg->mac = "AA.BB.CC.DD.EE.FF";
    msg->type = EventType::EVENT_WFD_NOTIFY_RTSP_TEARDOWN;
    event.eventMsg = msg;
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup()).Times(1)
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    BackupConnDev();
    sourceScene_->OnInnerEvent(event);
    RestoreConnDev();
}

/**
 * @tc.name: OnInnerEvent_005
 * @tc.desc: Test OnInnerEvent with null eventMsg
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnInnerEvent_005, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    SharingEvent event;
    event.eventMsg = nullptr;
    sourceScene_->OnInnerEvent(event);
}

/**
 * @tc.name: OnRemoteDied_001
 * @tc.desc: Test OnRemoteDied
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnRemoteDied_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), ReleaseScene(_));
    sourceScene_->OnRemoteDied();
}

/**
 * @tc.name: OnRemoteDied_002
 * @tc.desc: Test OnRemoteDied with null sharing adapter
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnRemoteDied_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    sourceScene_->sharingAdapter_.reset();
    sourceScene_->OnRemoteDied();
}

/**
 * @tc.name: HandleRemoveDevice_001
 * @tc.desc: Test HandleRemoveDevice with valid device
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleRemoveDevice_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<WfdSourceRemoveDeviceReq> msg = std::make_shared<WfdSourceRemoveDeviceReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "AA.BB.CC.DD.EE.FF";
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup()).Times(1)
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    BackupConnDev();
    int32_t ret = sourceScene_->HandleRemoveDevice(msg, reply);
    RestoreConnDev();
    EXPECT_EQ(ret, TEST_SUCCESS);
}

/**
 * @tc.name: HandleRemoveDevice_002
 * @tc.desc: Test HandleRemoveDevice with empty deviceId - should return error
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleRemoveDevice_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<WfdSourceRemoveDeviceReq> msg = std::make_shared<WfdSourceRemoveDeviceReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "";
    int32_t ret = sourceScene_->HandleRemoveDevice(msg, reply);
    EXPECT_EQ(ret, TEST_FAILURE);
}

/**
 * @tc.name: HandleRemoveDevice_003
 * @tc.desc: Test HandleRemoveDevice with null sharing adapter
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, HandleRemoveDevice_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    sourceScene_->sharingAdapter_.reset();
    std::shared_ptr<WfdSourceRemoveDeviceReq> msg = std::make_shared<WfdSourceRemoveDeviceReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    msg->deviceId = "AA.BB.CC.DD.EE.FF";

    int32_t ret = sourceScene_->HandleRemoveDevice(msg, reply);
    EXPECT_EQ(ret, TEST_FAILURE);
}

/**
 * @tc.name: Release_001
 * @tc.desc: Test Release with no connected device
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, Release_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    sourceScene_->Release();
}

/**
 * @tc.name: Release_002
 * @tc.desc: Test Release with connected device
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, Release_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .Times(2)
        .WillRepeatedly(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));

    BackupConnDev();
    sourceScene_->Release();
    RestoreConnDev();
}

/**
 * @tc.name: Release_003
 * @tc.desc: Test Release with null sharing adapter
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, Release_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    sourceScene_->sharingAdapter_.reset();
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    sourceScene_->Release();
}

/**
 * @tc.name: OnDomainMsg_001
 * @tc.desc: Test OnDomainMsg
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnDomainMsg_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<BaseDomainMsg> msg = std::make_shared<BaseDomainMsg>();
    sourceScene_->OnDomainMsg(msg);
}

/**
 * @tc.name: OnRequest_001
 * @tc.desc: Test OnRequest with WfdSourceStartDiscoveryReq
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnRequest_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);

    std::shared_ptr<WfdSourceStartDiscoveryReq> msg = std::make_shared<WfdSourceStartDiscoveryReq>();
    std::shared_ptr<BaseMsg> reply;
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), GetP2pEnableStatus(_))
        .WillOnce(DoAll(SetArgReferee<0>(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_NONE)),
        Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
    EXPECT_CALL(*(sourceScene_->mockIpcAdapter_), SendRequest(_, _))
        .WillOnce(Return(TEST_SUCCESS));

    sourceScene_->OnRequest(msg, reply);
}

/**
 * @tc.name: OnRequest_002
 * @tc.desc: Test OnRequest with WfdSourceStopDiscoveryReq
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnRequest_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    std::shared_ptr<WfdSourceStopDiscoveryReq> msg = = std::make_shared<WfdSourceStopDiscoveryReq>();
    std::shared_ptr<BaseMsg> reply;
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), StopDiscoverDevices())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    sourceScene_->OnRequest(msg, reply);
}

/**
 * @tc.name: OnRequest_003
 * @tc.desc: Test OnRequest with WfdSourceAddDeviceReq
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnRequest_003, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    std::shared_ptr<WfdSourceAddDeviceReq> msg = std::make_shared<WfdSourceAddDeviceReq>();
    std::shared_ptr<BaseMsg> reply;
    msg->screenId = 0;
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), P2pConnect(_))
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    sourceScene_->OnRequest(msg, reply);
}

/**
 * @tc.name: OnRequest_004
 * @tc.desc: Test OnRequest with WfdSourceRemoveDeviceReq
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnRequest_004, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    std::shared_ptr<WfdSourceRemoveDeviceReq> msg = std::make_shared<WfdSourceRemoveDeviceReq>();
    std::shared_ptr<BaseMsg> reply;
    msg->deviceId = "AA.BB.CC.DD.EE.FF";
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), ForwardEvent(_, _, _, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(TEST_SUCCESS));
    EXPECT_CALL(*(sourceScene_->mockWifiP2pInstance_), RemoveGroup())
        .WillOnce(Return(Wifi::ErrCode::WIFI_OPT_SUCCESS));
    BackupConnDev();
    sourceScene_->OnRequest(msg, reply);
    RestoreConnDev();
}

/**
 * @tc.name: OnRequest_005
 * @tc.desc: Test OnRequest with DestroyScreenCaptureReq
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnRequest_005, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    std::shared_ptr<DestroyScreenCaptureReq> msg = std::make_shared<DestroyScreenCaptureReq>();
    std::shared_ptr<BaseMsg> reply;
    EXPECT_CALL(*(sourceScene_->mockSharingAdapter_), DestroyAgent(_, _))
        .WillOnce(Return(TEST_SUCCESS));
    sourceScene_->OnRequest(msg, reply);
}

/**
 * @tc.name: OnRequest_006
 * @tc.desc: Test OnRequest with unknown message type
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnRequest_006, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    std::shared_ptr<BaseMsg> msg = std::make_shared<BaseMsg>();
    std::shared_ptr<BaseMsg> reply;
    sourceScene_->OnRequest(msg, reply);
}

/**
 * @tc.name: OnRequest_007
 * @tc.desc: Test OnRequest with null message
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnRequest_007, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    std::shared_ptr<BaseMsg> msg = nullptr;
    std::shared_ptr<BaseMsg> reply;
    sourceScene_->OnRequest(msg, reply);
}

/**
 * @tc.name: OnP2pPersistentGroupsChanged_001
 * @tc.desc: Test OnP2pPersistentGroupsChanged callback
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pPersistentGroupsChanged_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);
    sourceScene_->wfdP2pCallback_->OnP2pPersistentGroupsChanged();
}

/**
 * @tc.name: OnP2pPrivatePeersChanged_001
 * @tc.desc: Test OnP2pPrivatePeersChanged callback
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pPrivatePeersChanged_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);
    std::string priWfdInfo = "test_wfd_info";
    sourceScene_->wfdP2pCallback_->OnP2pPrivatePeersChanged(priWfdInfo);
}

/**
 * @tc.name: OnP2pServicesChanged_001
 * @tc.desc: Test OnP2pServicesChanged callback
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pServicesChanged_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);
    std::vector<Wifi::WifiP2pServiceInfo> srvInfo;
    sourceScene_->wfdP2pCallback_->OnP2pServicesChanged(srvInfo);
}

/**
 * @tc.name: OnP2pGcJoinGroup_001
 * @tc.desc: Test OnP2pGcJoinGroup callback
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pGcJoinGroup_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);
    OHOS::Wifi::GcInfo info;
    sourceScene_->wfdP2pCallback_->OnP2pGcJoinGroup(info);
}

/**
 * @tc.name: OnP2pGcLeaveGroup_001
 * @tc.desc: Test OnP2pGcLeaveGroup callback
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pGcLeaveGroup_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);
    OHOS::Wifi::GcInfo info;
    sourceScene_->wfdP2pCallback_->OnP2pGcLeaveGroupGroup(info);
}

/**
 * @tc.name: OnP2pDiscoveryChanged_001
 * @tc.desc: Test OnP2pDiscoveryChanged callback with true
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pDiscoveryChanged_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);
    sourceScene_->wfdP2pCallback_->OnP2pDiscoveryChanged(true);
}

/**
 * @tc.name: OnP2pDiscoveryChanged_002
 * @tc.desc: Test OnP2pDiscoveryChanged callback with false
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pDiscoveryChanged_002, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);
    sourceScene_->wfdP2pCallback_->OnP2pDiscoveryChanged(false);
}

/**
 * @tc.name: OnP2pActionResult_001
 * @tc.desc: Test OnP2pActionResult callback
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pActionResult_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);
    sourceScene_->wfdP2pCallback_->OnP2pActionResult(Wifi::P2pActionCallback::CREATE_GROUP,
                                                    Wifi::ErrCode::WIFI_OPT_SUCCESS);
}

/**
 * @tc.name: OnConfigChanged_001
 * @tc.desc: Test OnConfigChanged callback
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnConfigChanged_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);
    char data[] = "test_data";
    sourceScene_->wfdP2pCallback_->OnConfigChanged(Wifi::CfgType::CFG_TYPE_MAX, data, sizeof(data));
}

/**
 * @tc.name: OnP2pChrErrCodeReport_001
 * @tc.desc: Test OnP2pChrErrCodeReport callback
 * @tc.type: FUNC
 */
HWTEST_F(WfdSourceSceneTest, OnP2pChrErrCodeReport_001, TestSize.Level1)
{
    ASSERT_TRUE(sourceScene_ != nullptr);
    ASSERT_TRUE(sourceScene_->wfdP2pCallback_ != nullptr);
    sourceScene_->wfdP2pCallback_->OnP2pChrErrCodeReport(0);
}

} // namespace Sharing
} // namespace OHOS

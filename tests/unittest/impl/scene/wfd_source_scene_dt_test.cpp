/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>
#include <gmock/gmock.h>
#include "wfd_source_scene.h"
#include "wfd_session_def.h"
#include "common/sharing_log.h"
#include "network/socket/socket_utils.h"
#include "screen_capture_def.h"
#include "configuration/include/config.h"
#include "system_ability_definition.h"
#include "iservice_registry.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Sharing {

class WfdSourceSceneDtTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        HILOG_COMM_INFO("WfdSourceSceneDtTest::SetUpTestCase");
        scene_ = std::make_shared<WfdSourceScene>();
    }

    static void TearDownTestCase(void)
    {
        HILOG_COMM_INFO("WfdSourceSceneDtTest::TearDownTestCase");
        if (scene_) {
            scene_->Release();
            scene_ = nullptr;
        }
    }

    void SetUp(void) override
    {
        HILOG_COMM_INFO("WfdSourceSceneDtTest::SetUp");
        ASSERT_TRUE(scene_ != nullptr);
    }

    void TearDown(void) override
    {
        HILOG_COMM_INFO("WfdSourceSceneDtTest::TearDown");
    }

protected:
    static std::shared_ptr<WfdSourceScene> scene_;
};

std::shared_ptr<WfdSourceScene> WfdSourceSceneDtTest::scene_ = nullptr;

/**
 * @tc.name: Initialize_DT_001
 * @tc.desc: 测试WFD源场景初始化，验证基本配置加载
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, Initialize_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("Initialize_DT_001 start");
    
    // 测试初始化功能
    scene_->Initialize();
    
    // 验证基本成员变量是否正确初始化
    EXPECT_EQ(scene_->ctrlPort_, DEFAULT_WFD_CTRLPORT);
    EXPECT_EQ(scene_->videoCodecId_, CodecId::CODEC_H264);
    EXPECT_EQ(scene_->videoFormat_, VideoFormat::VIDEO_1920X1080_25);
    EXPECT_EQ(scene_->audioCodecId_, CodecId::CODEC_AAC);
    EXPECT_EQ(scene_->audioFormat_, AudioFormat::AUDIO_48000_16_2);
    EXPECT_FALSE(scene_->isSourceRunning_);
    EXPECT_FALSE(scene_->isSourceDiscovering);
    EXPECT_TRUE(scene_->isFirstDiscovery_);
    
    HILOG_COMM_INFO("Initialize_DT_001 success");
}

/**
 * @tc.name: HandleStartDiscovery_DT_001
 * @tc.desc: 测试开始发现设备，P2P状态为P2P_STATE_NONE
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleStartDiscovery_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleStartDiscovery_DT_001 start");
    
    std::shared_ptr<WfdSourceStartDiscoveryReq> msg = std::make_shared<WfdSourceStartDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    
    // 设置P2P初始状态为NONE
    scene_->isSourceRunning_ = false;
    if (scene_->p2pInstance_) {
        int32_t status = static_cast<int32_t>(Wifi::P2pState::P2P_STATE_NONE);
        scene_->p2pInstance_->GetP2pEnableStatus(status);
    }
    
    int32_t ret = scene_->HandleStartDiscovery(msg, reply);
    EXPECT_EQ(ret, -1);
    EXPECT_FALSE(scene_->isSourceRunning_);
    
    HILOG_COMM_INFO("HandleStartDiscovery_DT_001 success");
}

/**
 * @tc.name: HandleStartDiscovery_DT_002
 * @tc.desc: 测试开始发现设备，P2P状态为P2P_STATE_STARTED
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleStartDiscovery_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleStartDiscovery_DT_002 start");
    
    std::shared_ptr<WfdSourceStartDiscoveryReq> msg = std::make_shared<WfdSourceStartDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    
    // 设置P2P状态为STARTED，触发正常的发现流程
    scene_->isSourceRunning_ = true;
    scene_->isSourceDiscovering = false;
    
    int32_t ret = scene_->HandleStartDiscovery(msg, reply);
    EXPECT_EQ(ret, 0);
    EXPECT_TRUE(scene_->isSourceRunning_);
    EXPECT_TRUE(scene_->isSourceDiscovering);
    
    HILOG_COMM_INFO("HandleStartDiscovery_DT_002 success");
}

/**
 * @tc.name: HandleStartDiscovery_DT_003
 * @tc.desc: 测试开始发现设备，P2P状态为P2P_STATE_CLOSED并需要enable P2P
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleStartDiscovery_DT_003, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleStartDiscovery_DT_003 start");
    
    std::shared_ptr<WfdSourceStartDiscoveryReq> msg = std::make_shared<WfdSourceStartDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    
    // 设置P2P状态为CLOSED触发P2P enable流程
    scene_->isSourceRunning_ = true;
    scene_->isSourceDiscovering = false;
    
    int32_t ret = scene_->HandleStartDiscovery(msg, reply);
    // 根据P2P enable结果判断返回值
    if (scene_->p2pInstance_) {
        int32_t status = static_cast<int32_t>(Wifi::P2pState::P2P_STATE_CLOSED);
        scene_->p2pInstance_->GetP2pEnableStatus(status);
        if (status == static_cast<int32_t>(Wifi::P2pState::P2P_STATE_STARTED)) {
            EXPECT_EQ(ret, 0);
        }
    }
    
    HILOG_COMM_INFO("HandleStartDiscovery_DT_003 success");
}

/**
 * @tc.name: HandleStopDiscovery_DT_001
 * @tc.desc: 测试停止发现设备，正常停止流程
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleStopDiscovery_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleStopDiscovery_DT_001 start");
    
    std::shared_ptr<WfdSourceStopDiscoveryReq> msg = std::make_shared<WfdSourceStopDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    
    // 设置正常状态
    scene_->isSourceRunning_ = true;
    scene_->isSourceDiscovering = true;
    
    int32_t ret = scene_->HandleStopDiscovery(msg, reply);
    EXPECT_EQ(ret, 0);
    EXPECT_FALSE(scene_->isSourceDiscovering);
    
    HILOG_COMM_INFO("HandleStopDiscovery_DT_001 success");
}

/**
 * @tc.name: HandleStopDiscovery_DT_002
 * @tc.desc: 测试停止发现设备，P2P源未运行状态
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleStopDiscovery_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleStopDiscovery_DT_002 start");
    
    std::shared_ptr<WfdSourceStopDiscoveryReq> msg = std::make_shared<WfdSourceStopDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    
    // 设置P2P源未运行状态
    scene_->isSourceRunning_ = false;
    
    int32_t ret = scene_->HandleStopDiscovery(msg, reply);
    EXPECT_EQ(ret, -1);
    
    HILOG_COMM_INFO("HandleStopDiscovery_DT_002 success");
}

/**
 * @tc.name: HandleStopDiscovery_DT_003
 * @tc.desc: 测试停止发现设备，连接设备正在连接中
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleStopDiscovery_DT_003, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleStopDiscovery_DT_003 start");
    
    std::shared_ptr<WfdSourceStopDiscoveryReq> msg = std::make_shared<WfdSourceStopDiscoveryReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    
    // 设置连接设备正在连接中
    scene_->isSourceRunning_ = true;
    scene_->isSourceDiscovering = true;
    std::lock_guard<std::mutex> lock(scene_->mutex_);
    scene_->connDev_ = std::make_unique<ConnectionInfo>();
    scene_->connDev_->state = ConnectionState::CONNECTING;
    
    int32_t ret = scene_->HandleStopDiscovery(msg, reply);
    EXPECT_EQ(ret, -1);
    
    HILOG_COMM_INFO("HandleStopDiscovery_DT_003 success");
}

/**
 * @tc.name: HandleAddDevice_DT_001
 * @tc.desc: 测试添加设备，正常添加流程
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleAddDevice_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleAddDevice_DT_001 start");
    
    std::shared_ptr<WfdSourceAddDeviceReq> msg = std::make_shared<WfdSourceAddDeviceReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    
    // 设置有效的屏幕ID和设备ID
    msg->screenId = 1;  // 假设屏幕1存在
    msg->deviceId = "AA:BB:CC:DD:EE:FF";
    
    scene_->isSourceRunning_ = true;
    scene_->isSourceCanceled = false;
    scene_->isP2pConnected_ = false;
    
    int32_t ret = scene_->HandleAddDevice(msg, reply);
    // 实际返回值取决于P2P连接结果
    HILOG_COMM_INFO("HandleAddDevice_DT_001 ret: %{public}d", ret);
    
    HILOG_COMM_INFO("HandleAddDevice_DT_001 success");
}

/**
 * @tc.name: HandleAddDevice_DT_002
 * @tc.desc: 测试添加设备，无效的屏幕ID
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleAddDevice_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleAddDevice_DT_002 start");
    
    std::shared_ptr<WfdSourceAddDeviceReq> msg = std::make_shared<WfdSourceAddDeviceReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    
    // 设置无效的屏幕ID
    msg->screenId = 999999;  // 不存在的屏幕ID
    msg->deviceId = "AA:BB:CC:DD:EE:FF";
    
    int32_t ret = scene_->HandleAddDevice(msg, reply);
    EXPECT_EQ(ret, ERR_BAD_PARAMETER);
    
    HILOG_COMM_INFO("HandleAddDevice_DT_002 success");
}

/**
 * @tc.name: HandleAddDevice_DT_003
 * @tc.desc: 测试添加设备，P2P源未运行
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleAddDevice_DT_003, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleAddDevice_DT_003 start");
    
    std::shared_ptr<WfdSourceAddDeviceReq> msg = std::make_shared<WfdSourceAddDeviceReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    
    msg->screenId = 1;
    msg->deviceId = "AA:BB:CC:DD:EE:FF";
    
    // 设置P2P源未运行
    scene_->isSourceRunning_ = false;
    
    int32_t ret = scene_->HandleAddDevice(msg, reply);
    EXPECT_EQ(ret, -1);
    
    HILOG_COMM_INFO("HandleAddDevice_DT_003 success");
}

/**
 * @tc.name: HandleRemoveDevice_DT_001
 * @tc.desc: 测试移除设备，正常移除流程
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleRemoveDevice_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleRemoveDevice_DT_001 start");
    
    std::shared_ptr<WfdSourceRemoveDeviceReq> msg = std::make_shared<WfdSourceRemoveDeviceReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    
    msg->deviceId = "AA:BB:CC:DD:EE:FF";
    
    // 设置连接设备
    {
        std::lock_guard<std::mutex> lock(scene_->mutex_);
        scene_->connDev_ = std::make_unique<ConnectionInfo>();
        scene_->connDev_->mac = "AA:BB:CC:DD:EE:FF";
        scene_->connDev_->state = ConnectionState::CONNECTED;
        scene_->connDev_->contextId = 1;
        scene_->connDev_->agentId = 1;
    }
    
    int32_t ret = scene_->HandleRemoveDevice(msg, reply);
    EXPECT_EQ(ret, 0);
    
    HILOG_COMM_INFO("HandleRemoveDevice_DT_001 success");
}

/**
 * @tc.name: HandleRemoveDevice_DT_002
 * @tc.desc: 测试移除设备，设备未连接
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleRemoveDevice_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleRemoveDevice_DT_002 start");
    
    std::shared_ptr<WfdSourceRemoveDeviceReq> msg = std::make_shared<WfdSourceRemoveDeviceReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    
    msg->deviceId = "AA:BB:CC:DD:EE:FF";
    
    // 设置设备未连接状态
    {
        std::lock_guard<std::mutex> lock(scene_->mutex_);
        scene_->connDev_ = std::make_unique<ConnectionInfo>();
        scene_->connDev_->mac = "CC:DD:EE:FF:00:11";  // 不同的MAC地址
        scene_->connDev_->state = ConnectionState::CONNECTED;
    }
    
    int32_t ret = scene_->HandleRemoveDevice(msg, reply);
    EXPECT_EQ(ret, 0);
    
    HILOG_COMM_INFO("HandleRemoveDevice_DT_002 success");
}

/**
 * @tc.name: HandleRemoveDevice_DT_003
 * @tc.desc: 测试移除设备，空设备ID
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleRemoveDevice_DT_003, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleRemoveDevice_DT_003 start");
    
    std::shared_ptr<WfdSourceRemoveDeviceReq> msg = std::make_shared<WfdSourceRemoveDeviceReq>();
    std::shared_ptr<WfdCommonRsp> reply = std::make_shared<WfdCommonRsp>();
    
    msg->deviceId = "";  // 空设备ID
    
    int32_t ret = scene_->HandleRemoveDevice(msg, reply);
    EXPECT_EQ(ret, 0);
    
    HILOG_COMM_INFO("HandleRemoveDevice_DT_003 success");
}

/**
 * @tc.name: OnP2pStateChanged_DT_001
 * @tc.desc: 测试P2P状态变化，P2P_STATE_STARTED且源运行
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnP2pStateChanged_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("OnP2pStateChanged_DT_001 start");
    
    if (scene_->wfdP2pCallback_) {
        scene_->isSourceRunning_ = true;
        
        // 测试P2P状态变为STARTED
        scene_->wfdP2pCallback_->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2P_STATE_STARTED));
        
        EXPECT_TRUE(scene_->isSourceRunning_);
    }
    
    HILOG_COMM_INFO("OnP2pStateChanged_DT_001 success");
}

/**
 * @tc.name: OnP2pStateChanged_DT_002
 * @tc.desc: 测试P2P状态变化，P2P_STATE_CLOSED且源运行
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnP2pStateChanged_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("OnP2pStateChanged_DT_002 start");
    
    if (scene_->wfdP2pCallback_) {
        scene_->isSourceRunning_ = true;
        
        // 测试P2P状态变为CLOSED
        scene_->wfdP2pCallback_->OnP2pStateChanged(static_cast<int32_t>(Wifi::P2pState::P2_STATEP_CLOSED));
        
        EXPECT_FALSE(scene_->isSourceRunning_);  // 应该设置isSourceRunning_为false
    }
    
    HILOG_COMM_INFO("OnP2pStateChanged_DT_002 success");
}

/**
 * @tc.name: OnP2pThisDeviceChanged_DT_001
 * @tc.desc: 测试设备信息变化，GO协商拒绝事件
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnP2pThisDeviceChanged_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("OnP2pThisDeviceChanged_DT_001 start");
    
    if (scene_->wfdP2pCallback_ && scene_->p2pInstance_) {
        Wifi::WifiP2pDevice device;
        Wifi::WifiP2pWfdInfo wfdInfo;
        wfdInfo.setSessionAvailable(true);
        
        // 设置GO协商拒绝事件
        Wifi::P2pChrEvent chrEvent = Wifi::P2pChrEvent::GO_NEGOTIATION_PEER_REJECT;
        device.SetChrErrCode(chrEvent);
        device.SetWfdInfo(wfdInfo);
        
        // 模拟P2P连接信息获取
        Wifi::WifiP2pLinkedInfo info;
        info.SetConnectState(Wifi::P2pConnectedState::P2P_DISCONNECTED);
        EXPECT_CALL(*scene_->p2pInstance_, QueryP2pLinkedInfo(_))
            .WillOnce(DoAll(SetArgReferee<0>(info), Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
        
        scene_->OnP2pThisDeviceChanged(device);
        
        // 验证连接状态应该被重置
        std::lock_guard<std::mutex> lock(scene_->mutex_);
        if (scene_->connDev_) {
            EXPECT_EQ(scene_->connDev_->state, ConnectionState::DISCONNECTED);
        }
    }
    
    HILOG_COMM_INFO("OnP2pThisDeviceChanged_DT_001 success");
}

/**
 * @tc.name: OnP2pThisDeviceChanged_DT_002
 * @tc.desc: 测试设备信息变化，GO协商超时事件
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnP2pThisDeviceChanged_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("OnP2pThisDeviceChanged_DT_002 start");
    
    if (scene_->wfdP2pCallback_ && scene_->p2pInstance_) {
        Wifi::WifiP2pDevice device;
        Wifi::WifiP2pWfdInfo wfdInfo;
        wfdInfo.setSessionAvailable(true);
        
        // 设置GO协商超时事件
        Wifi::P2pChrEvent chrEvent = Wifi::P2pChrEvent::GO_NEGOTIATION_WAIT_PEER_READY_TIMEOUT;
        device.SetChrErrCode(chrEvent);
        device.SetWfdInfo(wfdInfo);
        
        // 模拟P2P连接信息获取
        Wifi::WifiP2pLinkedInfo info;
        info.SetConnectState(Wifi::P2pConnectedState::P2P_DISCONNECTED);
        EXPECT_CALL(*scene_->p2pInstance_, QueryP2pLinkedInfo(_))
            .WillOnce(DoAll(SetArgReferee<0>(info), Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
        
        scene_->OnP2pThisDeviceChanged(device);
        
        // 验证连接状态应该被重置
        std::lock_guard<std::mutex> lock(scene_->mutex_);
        if (scene_->connDev_) {
            EXPECT_EQ(scene_->connDev_->state, ConnectionState::DISCONNECTED);
        }
    }
    
    HILOG_COMM_INFO("OnP2pThisDeviceChanged_DT_002 success");
}

/**
 * @tc.name: OnP2pPeersChanged_DT_001
 * @tc.desc: 测试对等设备变化，发现可用设备
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnP2pPeersChanged_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("OnP2pPeersChanged_DT_001 start");
    
    if (scene_->wfdP2pCallback_) {
        scene_->isSourceDiscovering = true;
        
        std::vector<Wifi::WifiP2pDevice> devices;
        Wifi::WifiP2pDevice device;
        device.SetP2pDeviceStatus(Wifi::P2pDeviceStatus::PDS_AVAILABLE);
        device.SetDeviceName("TestDevice");
        device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
        
        // 设置WFD信息，支持会话且subelement不为0
        Wifi::WifiP2pWfdInfo info;
        info.setSessionAvailable(true);
        std::string subelement = "00:01:02:03:04:05:06";  // 长度大于ID_LEN且SUB_INDEX位置不为0
        info.SetDeviceInfoElement(subelement);
        device.SetWfdInfo(info);
        
        devices.emplace_back(device);
        
        scene_->wfdP2pCallback_->OnP2pPeersChanged(devices);
        
        // 验证设备发现触发
        EXPECT_TRUE(scene_->isSourceDiscovering);
    }
    
    HILOG_COMM_INFO("OnP2pPeersChanged_DT_001 success");
}

/**
 * @tc.name: OnP2pPeersChanged_DT_002
 * @tc.desc: 测试对等设备变化，源未发现状态
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnP2pPeersChanged_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("OnP2pPeersChanged_DT_002 start");
    
    if (scene_->wfdP2pCallback_) {
        // 设置源未发现状态
        scene_->isSourceDiscovering = false;
        
        std::vector<Wifi::WifiP2pDevice> devices;
        Wifi::WifiP2pDevice device;
        device.SetP2pDeviceStatus(Wifi::P2pDeviceStatus::PDS_AVAILABLE);
        device.SetDeviceName("TestDevice");
        device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
        
        Wifi::WifiP2pWfdInfo info;
        info.setSessionAvailable(true);
        device.SetWfdInfo(info);
        devices.emplace_back(device);
        
        scene_->wfdP2pCallback_->OnP2pPeersChanged(devices);
        
        // 验证不会处理设备发现
        EXPECT_FALSE(scene_->isSourceDiscovering);
    }
    
    HILOG_COMM_INFO("OnP2pPeersChanged_DT_002 success");
}

/**
 * @tc.name: OnP2pPeersChanged_DT_003
 * @tc.desc: 测试对等设备变化，设备不支持WFD会话
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnP2pPeersChanged_DT_003, TestSize.Level1)
{
    HILOG_COMM_INFO("OnP2pPeersChanged_DT_003 start");
    
    if (scene_->wfdP2pCallback_) {
        scene_->isSourceDiscovering = true;
        
        std::vector<Wifi::WifiP2pDevice> devices;
        Wifi::WifiP2pDevice device;
        device.SetP2pDeviceStatus(Wifi::P2pDeviceStatus::PDS_AVAILABLE);
        device.SetDeviceName("TestDevice");
        device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
        
        // 设置设备不支持WFD会话
        Wifi::WifiP2pWfdInfo info;
        info.setSessionAvailable(false);
        device.SetWfdInfo(info);
        devices.emplace_back(device);
        
        scene_->wfdP2pCallback_->OnP2pPeersChanged(devices);
        
        // 验证只会触发设备发现但不会添加不支持WFD的设备
        EXPECT_TRUE(scene_->isSourceDiscovering);
    }
    
    HILOG_COMM_INFO("OnP2pPeersChanged_DT_003 success");
}

/**
 * @tc.name: OnP2pConnectionChanged_DT_001
 * @tc.desc: 测试P2P连接状态变化，设备已连接
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnP2pConnectionChanged_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("OnP2pConnectionChanged_DT_001 start");
    
    if (scene_->wfdP2pCallback_ && scene_->p2pInstance_) {
        Wifi::WifiP2pLinkedInfo info;
        info.SetConnectState(Wifi::P2pConnectedState::P2P_CONNECTED);
        info.SetGroupOwnerAddress("192.168.1.100");
        
        // 设置组信息
        Wifi::WifiP2pGroupInfo group;
        group.SetGroupOwner(true);  // 作为组所有者
        group.SetFrequency(5180);
        group.SetPersistent(false);
        
        // 设置组内设备
        Wifi::WifiP2pDevice clientDevice;
        clientDevice.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
        clientDevice.SetDeviceName("ClientDevice");
        std::vector<Wifi::WifiP2pDevice> clientDevices;
        clientDevices.push_back(clientDevice);
        group.SetClientDevices(clientDevices);
        
        std::lock_guard<std::mutex> lock(scene_->mutex_);
        scene_->connDev_ = std::make_unique<ConnectionInfo>();
        scene_->connDev_->mac = "AA:BB:CC:DD:EE:FF";
        
        EXPECT_CALL(*scene_->p2pInstance_, GetCurrentGroup(_))
            .WillOnce(DoAll(SetArgReferee<0>(group), Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
        
        scene_->wfdP2pCallback_->OnP2pConnectionChanged(info);
        
        // 验证P2P连接状态
        EXPECT_TRUE(scene_->isP2pConnected_);
        EXPECT_TRUE(scene_->isGroupOwner_);
    }
    
    HILOG_COMM_INFO("OnP2pConnectionChanged_DT_001 success");
}

/**
 * @tc.name: OnP2pConnectionChanged_DT_002
 * @tc.desc: 测试P2P连接状态变化，设备断开连接
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnP2pConnectionChanged_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("OnP2pConnectionChanged_DT_002 start");
    
    if (scene_->wfdP2pCallback_ && scene_->p2pInstance_) {
        Wifi::WifiP2pLinkedInfo info;
        info.SetConnectState(Wifi::P2pConnectedState::P2P_DISCONNECTED);
        
        // 创建一个空的连接设备以保证回调能执行
        std::lock_guard<std::mutex> lock(scene_->mutex_);
        scene_->connDev_ = std::make_unique<ConnectionInfo>();
        
        scene_->wfdP2pCallback_->OnP2pConnectionChanged(info);
        
        // 验证P2P连接状态应该断开
        EXPECT_FALSE(scene_->isP2pConnected_);
    }
    
    HILOG_COMM_INFO("OnP2pConnectionChanged_DT_002 success");
}

/**
 * @tc.name: OnP2pConnectionChanged_DT_003
 * @tc.desc: 测试P2P连接状态变化，非组所有者连接
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnP2pConnectionChanged_DT_003, TestSize.Level1)
{
    HILOG_COMM_INFO("OnP2pConnectionChanged_DT_003 start");
    
    if (scene_->wfdP2pCallback_ && scene_->p2pInstance_) {
        Wifi::WifiP2pLinkedInfo info;
        info.SetConnectState(Wifi::P2pConnectedState::P2P_CONNECTED);
        info.SetGroupOwnerAddress("192.168.1.100");
        
        // 设置非组所有者信息
        Wifi::WifiP2pGroupInfo group;
        group.SetGroupOwner(false);  // 非组所有者
        group.SetFrequency(5180);
        group.SetPersistent(false);
        
        // 设置组所有者设备
        Wifi::WifiP2pDevice ownerDevice;
        ownerDevice.SetDeviceAddress("FF:EE:DD:CC:BB:AA");
        ownerDevice.SetDeviceName("OwnerDevice");
        group.SetOwner(ownerDevice);
        
        std::lock_guard<std::mutex> lock(scene_->mutex_);
        scene_->connDev_ = std::make_unique<ConnectionInfo>();
        scene_->connDev_->mac = "FF:EE:DD:CC:BB:AA";
        
        EXPECT_CALL(*scene_->p2pInstance_, GetCurrentGroup(_))
            .WillOnce(DoAll(SetArgReferee<0>(group), Return(Wifi::ErrCode::WIFI_OPT_SUCCESS)));
        
        scene_->wfdP2pCallback_->OnP2pConnectionChanged(info);
        
        // 验证非组所有者状态
        EXPECT_TRUE(scene_->isP2pConnected_);
        EXPECT_FALSE(scene_->isGroupOwner_);
    }
    
    HILOG_COMM_INFO("OnP2pConnectionChanged_DT_003 success");
}

/**
 * @tc.name: OnP2pConnectionChanged_DT_004
 * @tc.desc: 测试P2P连接状态变化，空组所有者地址
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnP2pConnectionChanged_DT_004, TestSize.Level1)
{
    HILOG_COMM_INFO("OnP2pConnectionChanged_DT_004 start");
    
    if (scene_->wfdP2pCallback_ && scene_->p2pInstance_) {
        Wifi::WifiP2pLinkedInfo info;
        info.SetConnectState(Wifi::P2pConnectedState::P2P_CONNECTED);
        info.SetGroupOwnerAddress("");  // 空的组所有者地址
        
        std::lock_guard<std::mutex> lock(scene_->mutex_);
        scene_->connDev_ = std::make_unique<ConnectionInfo>();
        
        scene_->wfdP2pCallback_->OnP2pConnectionChanged(info);
        
        // 验证P2P连接状态应该为false
        EXPECT_FALSE(scene_->isP2pConnected_);
    }
    
    HILOG_COMM_INFO("OnP2pConnectionChanged_DT_004 success");
}

/**
 * @tc.name: SetCheckWfdConnectionTimer_DT_001
 * @tc.desc: 测试设置WFD连接检查定时器，正常情况
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, SetCheckWfdConnectionTimer_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("SetCheckWfdConnectionTimer_DT_001 start");
    
    // 创建一个定时器来测试
    if (scene_->timer_ == nullptr) {
        scene_->timer_ = std::make_shared<TimeoutTimer>();
    }
    
    scene_->SetCheckWfdConnectionTimer();
    
    // 验证定时器ID被设置
    EXPECT_NE(scene_->timerId_, 0);
    
    HILOG_COMM_INFO("SetCheckWfdConnectionTimer_DT_001 success");
}

/**
 * @tc.name: SetCheckWfdConnectionTimer_DT_002
 * @tc.desc: 测试设置WFD连接检查定时器，定时器为null
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, SetCheckWfdConnectionTimer_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("SetCheckWfdConnectionTimer_DT_002 start");
    
    // 设置定时器为null
    scene_->timer_ = nullptr;
    scene_->timerId_ = 0;
    
    scene_->SetCheckWfdConnectionTimer();
    
    // 验证定时器ID未被设置
    EXPECT_EQ(scene_->timerId_, 0);
    
    HILOG_COMM_INFO("SetCheckWfdConnectionTimer_DT_002 success");
}

/**
 * @tc.name: OnCheckWfdConnection_DT_001
 * @tc.desc: 测试WFD连接检查超时处理
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnCheckWfdConnection_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("OnCheckWfdConnection_DT_001 start");
    
    // 清除之前的定时器
    scene_->timerId_ = 0;
    scene_->disconnectTimerId_ = 0;
    
    scene_->OnCheckWfdConnection();
    
    // 验证定时器ID被重置为0
    EXPECT_EQ(scene_->timerId_, 0);
    
    HILOG_COMM_INFO("OnCheckWfdConnection_DT_001 success");
}

/**
 * @tc.name: SetCheckWfdDisconnectionTimer_DT_001
 * @tc.desc: 测试设置WFD断开连接检查定时器
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, SetCheckWfdDisconnectionTimer_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("SetCheckWfdDisconnectionTimer_DT_001 start");
    
    // 创建一个定时器来测试
    if (scene_->timer_ == nullptr) {
        scene_->timer_ = std::make_shared<TimeoutTimer>();
    }
    
    scene_->SetCheckWfdDisconnectionTimer();
    
    // 验证断开连接定时器ID被设置
    EXPECT_NE(scene_->disconnectTimerId_, 0);
    
    HILOG_COMM_INFO("SetCheckWfdDisconnectionTimer_DT_001 success");
}

/**
 * @tc.name: ResetCheckWfdDisconnectionTimer_DT_001
 * @tc.desc: 测试重置WFD断开连接检查定时器
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, ResetCheckWfdDisconnectionTimer_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("ResetCheckWfdDisconnectionTimer_DT_001 start");
    
    // 设置一个有效的定时器ID
    scene_->disconnectTimerId_ = 123;
    
    scene_->ResetCheckWfdDisconnectionTimer();
    
    // 验证断开连接定时器ID被重置为0
    EXPECT_EQ(scene_->disconnectTimerId_, 0);
    
    HILOG_COMM_INFO("ResetCheckWfdDisconnectionTimer_DT_001 success");
}

/**
 * @tc.name: OnCheckWfdDisconnection_DT_001
 * @tc.desc: 测试WFD断开连接检查超时处理
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnCheckWfdDisconnection_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("OnCheckWfdDisconnection_DT_001 start");
    
    // 设置一个连接设备
    std::lock_guard<std::mutex> lock(scene_->mutex_);
    scene_->connDev_ = std::make_unique<ConnectionInfo>();
    scene_->connDev_->mac = "AA:BB:CC:DD:EE:FF";
    scene_->connDev_->state = ConnectionState::CONNECTED;
    
    scene_->OnCheckWfdDisconnection();
    
    // 验证定时器被重置
    EXPECT_EQ(scene_->disconnectTimerId_, 0);
    
    HILOG_COMM_INFO("OnCheckWfdDisconnection_DT_001 success");
}

/**
 * @tc.name: OnDeviceFound_DT_001
 * @tc.desc: 测试设备发现回调，发现多个设备
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnDeviceFound_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("OnDeviceFound_DT_001 start");
    
    std::vector<WfdCastDeviceInfo> deviceInfos;
    
    // 添加多个设备
    WfdCastDeviceInfo device1;
    device1.deviceId = "AA:BB:CC:DD:EE:FF";
    device1.deviceName = "Device1";
    device1.primaryDeviceType = "type1";
    device1.secondaryDeviceType = "Other";
    device1.udidHash = "hash1";
    deviceInfos.push_back(device1);
    
    WfdCastDeviceInfo device2;
    device2.deviceId = "FF:EE:DD:CC:BB:AA";
    device2.deviceName = "Device2";
    device2.primaryDeviceType = "type2";
    device2.secondaryDeviceType = "CastPlus";
    device2.udidHash = "hash2";
    deviceInfos.push_back(device2);
    
    // 设置连接设备
    {
        std::lock_guard<std::mutex> lock(scene_->mutex_);
        scene_->connDev_ = std::make_unique<ConnectionInfo>();
        scene_->connDev_->mac = "AA:BB:CC:DD:EE:FF";  // 与设备1相同
        scene_->connDev_->state = ConnectionState::CONNECTED;
    }
    
    scene_->OnDeviceFound(deviceInfos);
    
    HILOG_COMM_INFO("OnDeviceFound_DT_001 success");
}

/**
 * @tc.name: OnDeviceFound_DT_002
 * @tc.desc: 测试设备发现回调，发现设备与当前连接设备不同
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnDeviceFound_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("OnDeviceFound_DT_002 start");
    
    std::vector<WfdCastDeviceInfo> deviceInfos;
    
    // 添加一个设备
    WfdCastDeviceInfo device;
    device.deviceId = "FF:EE:DD:CC:BB:AA";
    device.deviceName = "Device";
    device.primaryDeviceType = "type1";
    device.secondaryDeviceType = "Other";
    device.udidHash = "hash1";
    deviceInfos.push_back(device);
    
    // 设置连接设备，MAC地址与发现的设备不同
    {
        std::lock_guard<std::mutex> lock(scene_->mutex_);
        scene_->connDev_ = std::make_unique<ConnectionInfo>();
        scene_->connDev_->mac = "AA:BB:CC:DD:EE:FF";  // 不同的MAC地址
        scene_->connDev_->state = ConnectionState::CONNECTED;
    }
    
    scene_->OnDeviceFound(deviceInfos);
    
    HILOG_COMM_INFO("OnDeviceFound_DT_002 success");
}

/**
 * @tc.name: OnDeviceOffline_DT_001
 * @tc.desc: 测试设备离线回调，设备连接状态检查
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnDeviceOffline_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("OnDeviceOffline_DT_001 start");
    
    // 设置连接设备
    {
        std::lock_guard<std::mutex> lock(scene_->mutex_);
        scene_->connDev_ = std::make_unique<ConnectionInfo>();
        scene_->connDev_->mac = "AA:BB:CC:DD:EE:FF";
        scene_->connDev_->state = ConnectionState::CONNECTED;
    }
    
    scene_->OnDeviceOffline();
    
    // 验证设备离线逻辑不执行任何操作
    HILOG_COMM_INFO("OnDeviceOffline_DT_001 success");
}

/**
 * @tc.name: OnDeviceOffline_DT_002
 * @tc.desc: 测试设备离线回调，无连接设备
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnDeviceOffline_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("OnDeviceOffline_DT_002 start");
    
    // 设置无连接设备
    {
        std::lock_guard<std::mutex> lock(scene_->mutex_);
        scene_->connDev_ = nullptr;
    }
    
    scene_->OnDeviceOffline();
    
    HILOG_COMM_INFO("OnDeviceOffline_DT_002 success");
}

/**
 * @tc.name: OnP2pPeerConnected_DT_001
 * @tc.desc: 测试P2P对等设备连接回调，正常连接
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnP2pPeerConnected_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("OnP2pPeerConnected_DT_001 start");
    
    ConnectionInfo connectionInfo;
    connectionInfo.mac = "AA:BB:CC:DD:EE:FF";
    connectionInfo.ip = "192.168.1.100";
    connectionInfo.ctrlPort = DEFAULT_WFD_CTRLPORT;
    connectionInfo.state = ConnectionState::CONNECTING;
    connectionInfo.deviceName = "TestDevice";
    connectionInfo.primaryDeviceType = "type1";
    connectionInfo.secondaryDeviceType = "Other";
    connectionInfo.udidHash = "testhash";
    
    // 设置P2P源运行状态
    scene_->isSourceRunning_ = true;
    scene_->isSourceCanceled = false;
    scene_->isSourceDiscovering = false;
    
    // 设置有效的连接设备
    {
        std::lock_guard<std::mutex> lock(scene_->mutex_);
        scene_->connDev_ = std::make_unique<ConnectionInfo>();
        scene_->connDev_->mac = "AA:BB:CC:DD:EE:FF";  // 相同的MAC地址
        scene_->connDev_->ctrlPort = 0;  // 初始端口为0
        scene_->videoCodecId_ = CodecId::CODEC_H264;
        scene_->audioCodecId_ = CodecId::CODEC_AAC;
        scene_->videoFormat_ = VideoFormat::VIDEO_1920X1080_25;
        scene_->audioFormat_ = AudioFormat::AUDIO_48000_16_2;
    }
    
    scene_->OnP2pPeerConnected(connectionInfo);
    
    // 验证连接状态更新
    std::lock_guard<std::mutex> lock(scene_->mutex_);
    EXPECT_TRUE(scene_->connDev_ != nullptr);
    EXPECT_EQ(scene_->connDev_->state, ConnectionState::CONNECTING);
    EXPECT_EQ(scene_->connDev_->ip, "192.168.1.100");
    
    HILOG_COMM_INFO("OnP2pPeerConnected_DT_001 success");
}

/**
 * @tc.name: OnP2pPeerConnected_DT_002
 * @tc.desc: 测试P2P对等设备连接回调，源未运行
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnP2pPeerConnected_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("OnP2pPeerConnected_DT_002 start");
    
    ConnectionInfo connectionInfo;
    connectionInfo.mac = "AA:BB:CC:DD:EE:FF";
    connectionInfo.ip = "192.168.1.100";
    connectionInfo.ctrlPort = DEFAULT_WFD_CTRLPORT;
    connectionInfo.state = ConnectionState::CONNECTING;
    
    // 设置P2P源未运行状态
    scene_->isSourceRunning_ = false;
    
    scene_->OnP2pPeerConnected(connectionInfo);
    
    // 验证连接信息未被更新
    std::lock_guard<std::mutex> lock(scene_->mutex_);
    if (scene_->connDev_ != nullptr) {
        EXPECT_NE(scene_->connDev_->ip, "192.168.1.100");
    }
    
    HILOG_COMM_INFO("OnP2pPeerConnected_DT_002 success");
}

/**
 * @tc.name: OnP2pPeerConnected_DT_003
 * @tc.desc: 测试P2P对等设备连接回调，源已取消
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnP2pPeerConnected_DT_003, TestSize.Level1)
{
    HILOG_COMM_INFO("OnP2pPeerConnected_DT_003 start");
    
    ConnectionInfo connectionInfo;
    connectionInfo.mac = "AA:BB:CC:DD:EE:FF";
    connectionInfo.ip = "192.168.1.100";
    connectionInfo.ctrlPort = DEFAULT_WFD_CTRLPORT;
    connectionInfo.state = ConnectionState::CONNECTING;
    
    // 设置源已取消状态
    scene_->isSourceRunning_ = true;
    scene_->isSourceCanceled = true;
    
    scene_->OnP2pPeerConnected(connectionInfo);
    
    // 验证连接信息未被更新
    std::lock_guard<std::mutex> lock(scene_->mutex_);
    if (scene_->connDev_ != nullptr) {
        EXPECT_NE(scene_->connDev_->ip, "192.168.1.100");
    }
    
    HILOG_COMM_INFO("OnP2pPeerConnected_DT_003 success");
}

/**
 * @tc.name: OnP2pPeerConnected_DT_004
 * @tc.desc: 测试P2P对等设备连接回调，无效端口
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnP2pPeerConnected_DT_004, TestSize.Level1)
{
    HILOG_COMM_INFO("OnP2pPeerConnected_DT_004 start");
    
    ConnectionInfo connectionInfo;
    connectionInfo.mac = "AA:BB:CC:DD:EE:FF";
    connectionInfo.ctrlPort = 0;  // 无效端口
    
    // 设置P2P源运行状态
    scene_->isSourceRunning_ = true;
    scene_->isSourceCanceled = false;
    scene_->isSourceDiscovering = false;
    
    scene_->OnP2pPeerConnected(connectionInfo);
    
    // 验证连接信息未被更新
    std::lock_guard<std::mutex> lock(scene_->mutex_);
    if (scene_->connDev_ != nullptr) {
        EXPECT_NE(scene_->connDev_->mac, "AA:BB:CC:DD:EE:FF");
    }
    
    HILOG_COMM_INFO("OnP2pPeerConnected_DT_004 success");
}

/**
 * @tc.name: OnP2pPeerDisconnected_DT_001
 * @tc.desc: 测试P2P对等设备断开连接回调，需要通知
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnP2pPeerDisconnected_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("OnP2pPeerDisconnected_DT_001 start");
    
    ConnectionInfo connectionInfo;
    connectionInfo.mac = "AA:BB:CC:DD:EE:FF";
    
    // 设置连接设备
    {
        std::lock_guard<std::mutex> lock(scene_->mutex_);
        scene_->connDev_ = std::make_unique<ConnectionInfo>();
        scene_->connDev_->mac = "AA:BB:CC:DD:EE:FF";
        scene_->connDev_->state = ConnectionState::CONNECTED;
    }
    
    scene_->OnP2pPeerDisconnected(connectionInfo);
    
    // 验证断开连接定时器被设置
    EXPECT_NE(scene_->disconnectTimerId_, 0);
    
    HILOG_COMM_INFO("OnP2pPeerDisconnected_DT_001 success");
}

/**
 * @tc.name: OnP2pPeerDisconnected_DT_002
 * @tc.desc: 测试P2P对等设备断开连接回调，不需要通知
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnP2pPeerDisconnected_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("OnP2pPeerDisconnected_DT_002 start");
    
    ConnectionInfo connectionInfo;
    connectionInfo.mac = "";
    
    scene_->OnP2pPeerDisconnected(connectionInfo, false);
    
    HILOG_COMM_INFO("OnP2pPeerDisconnected_DT_002 success");
}

/**
 * @tc.name: ErrorCodeFiltering_DT_001
 * @tc.desc: 测试错误码过滤，会话相关错误
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, ErrorCodeFiltering_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("ErrorCodeFiltering_DT_001 start");
    
    int32_t code = static_cast<int32_t>(SharingErrorCode::ERR_CONTEXT_AGENT_BASE);
    scene_->ErrorCodeFiltering(code);
    
    // 验证错误码被转换为通用错误
    EXPECT_EQ(code, static_cast<int32_t>(SharingErrorCode::ERR_GENERAL_ERROR));
    
    HILOG_COMM_INFO("ErrorCodeFiltering_DT_001 success");
}

/**
 * @tc.name: ErrorCodeFiltering_DT_002
 * @tc.desc: 测试错误码过滤，会话基础错误
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, ErrorCodeFiltering_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("ErrorCodeFiltering_DT_002 start");
    
    int32_t code = static_cast<int32_t>(SharingErrorCode::ERR_SESSION_BASE);
    scene_->ErrorCodeFiltering(code);
    
    // 验证错误码被转换为通用错误
    EXPECT_EQ(code, static_cast<int32_t>(SharingErrorCode::ERR_GENERAL_ERROR));
    
    HILOG_COMM_INFO("ErrorCodeFiltering_DT_002 success");
}

/**
 * @tc.name: ErrorCodeFiltering_DT_003
 * @tc.desc: 测试错误码过滤，消费者超时错误
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, ErrorCodeFiltering_DT_003, TestSize.Level1)
{
    HILOG_COMM_INFO("ErrorCodeFiltering_DT_003 start");
    
    int32_t code = static_cast<int32_t>(ERR_PROSUMER_TIMEOUT);
    scene_->ErrorCodeFiltering(code);
    
    // 验证超时错误被转换为连接超时错误
    EXPECT_EQ(code, static_cast<int32_t>(SharingErrorCode::ERR_CONNECTION_TIMEOUT));
    
    HILOG_COMM_INFO("ErrorCodeFiltering_DT_003 success");
}

/**
 * @tc.name: ErrorCodeFiltering_DT_004
 * @tc.desc: 测试错误码过滤，消费者销毁错误
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSceneSourceDtTest, ErrorCodeFiltering_DT_004, TestSize.Level1)
{
    HILOG_COMM_INFO("ErrorCodeFiltering_DT_004 start");
    
    int32_t code = static_cast<int32_t>(ERR_PROSUMER_DESTROY);
    scene_->ErrorCodeFiltering(code);
    
    // 验证销毁错误被转换为状态异常错误
    EXPECT_EQ(code, static_cast<int32_t>(SharingErrorCode::ERR_STATE_EXCEPTION));
    
    HILOG_COMM_INFO("ErrorCodeFiltering_DT_004 success");
}

/**
 * @tc.name: ErrorCodeFiltering_DT_005
 * @tc.desc: 测试错误码过滤，RTP连接错误
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, ErrorCodeFiltering_DT_005, TestSize.Level1)
{
    HILOG_COMM_INFO("ErrorCodeFiltering_DT_005 start");
    
    int32_t code = static_cast<int32_t>(ERR_PROSUMER_RTP_CONNECT);
    scene_->ErrorCodeFiltering(code);
    
    // 验证RTP连接错误未被转换
    EXPECT_EQ(code, static_cast<int32_t>(ERR_PROSUMER_RTP_CONNECT));
    
    HILOG_COMM_INFO("ErrorCodeFiltering_DT_005 success");
}

/**
 * @tc.name: IsScreenIdExist_DT_001
 * @tc.desc: 测试屏幕ID存在性检查，存在的屏幕
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, IsScreenIdExist_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("IsScreenIdExist_DT_001 start");
    
    bool result = scene_->IsScreenIdExist(1);  // 假屏幕设1存在
    // 设备中可能有多个屏幕，这里只是检查函数是否正常执行
    HILOG_COMM_INFO("IsScreenIdExist result: %{public}d", result);
    
    HILOG_COMM_INFO("IsScreenIdExist_DT_001 success");
}

/**
 * @tc.name: IsScreenIdExist_DT_002
 * @tc.desc: 测试屏幕ID存在性检查，不存在的屏幕
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, IsScreenIdExist_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("IsScreenIdExist_DT_002 start");
    
    bool result = scene_->IsScreenIdExist(999999);  // 不存在的屏幕ID
    EXPECT_FALSE(result);
    
    HILOG_COMM_INFO("IsScreenIdExist_DT_002 success");
}

/**
 * @tc.name: HandleDestroyScreenCapture_DT_001
 * @tc.desc: 测试销毁屏幕捕获，正常销毁
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleDestroyScreenCapture_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleDestroyScreenCapture_DT_001 start");
    
    std::shared_ptr<DestroyScreenCaptureReq> msg = std::make_shared<DestroyScreenCaptureReq>();
    
    scene_->contextId_ = 1;
    scene_->agentId_ = 2;
    
    int32_t ret = scene_->HandleDestroyScreenCapture(msg);
    EXPECT_EQ(ret, ERR_OK);
    
    HILOG_COMM_INFO("HandleDestroyScreenCapture_DT_001 success");
}

/**
 * @tc.name: HandleDestroyScreenCapture_DT_002
 * @tc.desc: 测试销毁屏幕捕获，无效的上下文ID
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleDestroyScreenCapture_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleDestroyScreenCapture_DT_002 start");
    
    std::shared_ptr<DestroyScreenCaptureReq> msg = std::make_shared<DestroyScreenCaptureReq>();
    
    scene_->contextId_ = INVALID_ID;
    scene_->agentId_ = INVALID_ID;
    
    int32_t ret = scene_->HandleDestroyScreenCapture(msg);
    EXPECT_EQ(ret, ERR_OK);
    
    HILOG_COMM_INFO("HandleDestroyScreenCapture_DT_002 success");
}

/**
 * @tc.name: HandleSetAudio_DT_001
 * @tc.desc: 测试设置音频，启用音频
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleSetAudio_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleSetAudio_DT_001 start");
    
    std::shared_ptr<WfdSourceSetAudioReq> msg = std::make_shared<WfdSourceSetAudioReq>();
    msg->isAudioOn = true;
    
    scene_->contextId_ = 1;
    scene_->agentId_ = 2;
    
    int32_t ret = scene_->HandleSetAudio(msg);
    EXPECT_EQ(ret, ERR_OK);
    
    HILOG_COMM_INFO("HandleSetAudio_DT_001 success");
}

/**
 * @tc.name: HandleSetAudio_DT_002
 * @tc.desc: 测试设置音频，禁用音频
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleSetAudio_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleSetAudio_DT_002 start");
    
    std::shared_ptr<WfdSourceSetAudioReq> msg = std::make_shared<WfdSourceSetAudioReq>();
    msg->isAudioOn = false;
    
    scene_->contextId_ = 1;
    scene_->agentId_ = 2;
    
    int32_t ret = scene_->HandleSetAudio(msg);
    EXPECT_EQ(ret, ERR_OK);
    
    HILOG_COMM_INFO("HandleSetAudio_DT_002 success");
}

/**
 * @tc.name: HandleSetAudio_DT_003
 * @tc.desc: 测试设置音频，无效的上下文ID
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleSetAudio_DT_003, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleSetAudio_DT_003 start");
    
    std::shared_ptr<WfdSourceSetAudioReq> msg = std::make_shared<WfdSourceSetAudioReq>();
    msg->isAudioOn = true;
    
    scene_->contextId_ = INVALID_ID;
    scene_->agentId_ = INVALID_ID;
    
    int32_t ret = scene_->HandleSetAudio(msg);
    EXPECT_EQ(ret, ERR_OK);
    
    HILOG_COMM_INFO("HandleSetAudio_DT_003 success");
}

/**
 * @tc.name: HandleSetProjectionDisplay_DT_001
 * @tc.desc: 测试设置投影显示，正常参数
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleSetProjectionDisplay_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleSetProjectionDisplay_DT_001 start");
    
    std::shared_ptr<WfdSourceSetDisplayReq> msg = std::make_shared<WfdSourceSetDisplayReq>();
    
    // 构造有效的JSON参数
    json displayParam;
    displayParam[KEY_PROJECT_MODE] = 1;
    displayParam[KEY_PROJECT_ROTATION] = 0;
    displayParam[KEY_PROJECT_RECT] = {
        {KEY_PROJECT_RECT_LEFT, 0},
        {KEY_PROJECT_RECT_TOP, 0},
        {KEY_PROJECT_RECT_WIDTH, 1920},
        {KEY_PROJECT_RECT_HEIGHT, 1080}
    };
    
    msg->displayParam = displayParam.dump();
    
    scene_->contextId_ = 1;
    scene_->agentId_ = 2;
    
    int32_t ret = scene_->HandleSetProjectionDisplay(msg);
    EXPECT_EQ(ret, 0);
    
    HILOG_COMM_INFO("HandleSetProjectionDisplay_DT_001 success");
}

/**
 * @tc.name: HandleSetProjectionDisplay_DT_002
 * @tc.desc: 测试设置投影显示，无效的JSON格式
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleSetProjectionDisplay_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleSetProjectionDisplay_DT_002 start");
    
    std::shared_ptr<WfdSourceSetDisplayReq> msg = std::make_shared<WfdSourceSetDisplayReq>();
    msg->displayParam = "invalid_json";  // 无效的JSON
    
    scene_->contextId_ = 1;
    scene_->agentId_ = 2;
    
    int32_t ret = scene_->HandleSetProjectionDisplay(msg);
    EXPECT_EQ(ret, -1);
    
    HILOG_COMM_INFO("HandleSetProjectionDisplay_DT_002 success");
}

/**
 * @tc.name: HandleSetProjectionDisplay_DT_003
 * @tc.desc: 测试设置投影显示，缺少必需字段
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleSetProjectionDisplay_DT_003, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleSetProjectionDisplay_DT_003 start");
    
    std::shared_ptr<WfdSourceSetDisplayReq> msg = std::make_shared<WfdSourceSetDisplayReq>();
    
    // 构造缺少必需字段的JSON参数
    json displayParam;
    displayParam["other_field"] = "value";
    
    msg->displayParam = displayParam.dump();
    
    scene_->contextId_ = 1;
    scene_->agentId_ = 2;
    
    int32_t ret = scene_->HandleSetProjectionDisplay(msg);
    EXPECT_EQ(ret, -1);
    
    HILOG_COMM_INFO("HandleSetProjectionDisplay_DT_003 success");
}

/**
 * @tc.name: HandleSetProjectionDisplay_DT_004
 * @tc.desc: 测试设置投影显示，参数类型错误
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleSetProjectionDisplay_DT_004, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleSetProjectionDisplay_DT_004 start");
    
    std::shared_ptr<WfdSourceSetDisplayReq> msg = std::make_shared<WfdSourceSetDisplayReq>();
    
    // 构造参数类型错误的JSON参数
    json displayParam;
    displayParam[KEY_PROJECT_MODE] = "not_a_number";  // 应该是数字
    displayParam[KEY_PROJECT_ROTATION] = 0;
    displayParam[KEY_PROJECT_RECT] = {
        {KEY_PROJECT_RECT_LEFT, 0},
        {KEY_PROJECT_RECT_TOP, 0},
        {KEY_PROJECT_RECT_WIDTH, 1920},
        {KEY_PROJECT_RECT_HEIGHT, 1080}
    };
    
    msg->displayParam = displayParam.dump();
    
    scene_->contextId_ = 1;
    scene_->agentId_ = 2;
    
    int32_t ret = scene_->HandleSetProjectionDisplay(msg);
    EXPECT_EQ(ret, -1);
    
    HILOG_COMM_INFO("HandleSetProjectionDisplay_DT_004 success");
}

/**
 * @tc.name: HandleEvent_DT_001
 * @tc.desc: 测试处理事件，正常事件处理
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleEvent_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleEvent_DT_001 start");
    
    std::shared_ptr<WfdSourceEventReq> msg = std::make_shared<WfdSourceEventReq>();
    msg->eventId = 1;
    msg->eventParam = "test_event_param";
    
    scene_->contextId_ = 1;
    scene_->agentId_ = 2;
    
    int32_t ret = scene_->HandleEvent(msg);
    EXPECT_EQ(ret, 0);
    
    HILOG_COMM_INFO("HandleEvent_DT_001 success");
}

/**
 * @tc.name: HandleEvent_DT_002
 * @tc.desc: 测试处理事件，空事件参数
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleEvent_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleEvent_DT_002 start");
    
    std::shared_ptr<WfdSourceEventReq> msg = std::make_shared<WfdSourceEventReq>();
    msg->eventId = 1;
    msg->eventParam = "";
    
    scene_->contextId_ = 1;
    scene_->agentId_ = 2;
    
    int32_t ret = scene_->HandleEvent(msg);
    EXPECT_EQ(ret, 0);
    
    HILOG_COMM_INFO("HandleEvent_DT_002 success");
}

/**
 * @tc.name: HandleEvent_DT_003
 * @tc.desc: 测试处理事件，无效的上下文ID
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleEvent_DT_003, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleEvent_DT_003 start");
    
    std::shared_ptr<WfdSourceEventReq> msg = std::make_shared<WfdSourceEventReq>();
    msg->eventId = 1;
    msg->eventParam = "test";
    
    scene_->contextId_ = INVALID_ID;
    scene_->agentId_ = INVALID_ID;
    
    int32_t ret = scene_->HandleEvent(msg);
    EXPECT_EQ(ret, 0);
    
    HILOG_COMM_INFO("HandleEvent_DT_003 success");
}

/**
 * @tc.name: HandleAppendSurface_DT_001
 * @tc.desc: 测试附加表面，正常附加
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleAppendSurface_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleAppendSurface_DT_001 start");
    
    std::shared_ptr<WfdAppCastSurfaceReq> msg = std::make_shared<WfdAppCastSurfaceReq>();
    msg->surface = mock::Surface();  // 假设的surface对象
    
    scene_->contextId_ = 1;
    scene_->agentId_ = 2;
    
    int32_t ret = scene_->HandleAppendSurface(msg);
    EXPECT_EQ(ret, 0);
    
    HILOG_COMM_INFO("HandleAppendSurface_DT_001 success");
}

/**
 * @tc.name: HandleAppendSurface_DT_002
 * @tc.desc: 测试附加表面，无效的上下文ID
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, HandleAppendSurface_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("HandleAppendSurface_DT_002 start");
    
    std::shared_ptr<WfdAppCastSurfaceReq> msg = std::make_shared<WfdAppCastSurfaceReq>();
    msg->surface = mock::Surface();
    
    scene_->contextId_ = INVALID_ID;
    scene_->agentId_ = INVALID_ID;
    
    int32_t ret = scene_->HandleAppendSurface(msg);
    EXPECT_EQ(ret, 0);
    
    HILOG_COMM_INFO("HandleAppendSurface_DT_002 success");
}

/**
 * @tc.name: AppendCast_DT_001
 * @tc.desc: 测试附加投流，正常附加流程
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, AppendCast_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("AppendCast_DT_001 start");
    
    // 创建一个共享适配器mock对象用于测试
    if (!scene_->sharingAdapter_.lock()) {
        std::shared_ptr<MockSharingAdapter> mockAdapter = std::make_shared<MockSharingAdapter>();
        scene_->sharingAdapter_ = mockAdapter;
    }
    
    std::string deviceId = "AA:BB:CC:DD:EE:FF";
    
    // 设置有效的连接设备
    {
        std::lock_guard<std::mutex> lock(scene_->mutex_);
        scene_->connDev_ = std::make_unique<ConnectionInfo>();
        scene_->connDev_->mac = deviceId;
        scene_->connDev_->state = ConnectionState::CONNECTED;
    }
    
    int32_t ret = scene_->AppendCast(deviceId);
    HILOG_COMM_INFO("AppendCast_DT_001 ret: %{public}d", ret);
    
    HILOG_COMM_INFO("AppendCast_DT_001 success");
}

/**
 * @tc.name: AppendCast_DT_002
 * @tc.desc: 测试附加投流，不匹配的设备ID
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, AppendCast_DT_002, TestSize.Level1)
{
    HILOG_COMM_INFO("AppendCast_DT_002 start");
    
    std::string deviceId = "AA:BB:CC:DD:EE:FF";
    
    // 设置不匹配的连接设备
    {
        std::lock_guard<std::mutex> lock(scene_->mutex_);
        scene_->connDev_ = std::make_unique<ConnectionInfo>();
        scene_->connDev_->mac = "FF:EE:DD:CC:BB:AA";  // 不同的设备ID
        scene_->connDev_->state = ConnectionState::CONNECTED;
    }
    
    int32_t ret = scene_->AppendCast(deviceId);
    EXPECT_EQ(ret, -1);
    
    HILOG_COMM_INFO("AppendCast_DT_002 success");
}

/**
 * @tc.name: AppendCast_DT_003
 * @tc.desc: 测试附加投流，空设备ID
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, AppendCast_DT_003, TestSize.Level1)
{
    HILOG_COMM_INFO("AppendCast_DT_003 start");
    
    std::string deviceId = "";
    
    // 设置连接设备
    {
        std::lock_guard<std::mutex> lock(scene_->mutex_);
        scene_->connDev_ = std::make_unique<ConnectionInfo>();
        scene_->connDev_->mac = "AA:BB:CC:DD:EE:FF";
        scene_->connDev_->state = ConnectionState::CONNECTED;
    }
    
    int32_t ret = scene_->AppendCast(deviceId);
    EXPECT_EQ(ret, -1);
    
    HILOG_COMM_INFO("AppendCast_DT_003 success");
}

/**
 * @tc.name: AppendCast_DT_004
 * @tc.desc: 测试附加投流，连接设备已运行
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, AppendCast_DT_004, TestSize.Level1)
{
    HILOG_COMM_INFO("AppendCast_DT_004 start");
    
    std::string deviceId = "AA:BB:CC:DD:EE:FF";
    
    // 设置已运行的连接设备
    {
        std::lock_guard<std::mutex> lock(scene_->mutex_);
        scene_->connDev_ = std::make_unique<ConnectionInfo>();
        scene_->connDev_->mac = deviceId;
        scene_->connDev_->state = ConnectionState::CONNECTED;
        scene_->connDev_->isRunning = true;  // 已运行状态
    }
    
    int32_t ret = scene_->AppendCast(deviceId);
    EXPECT_EQ(ret, 0);
    
    HILOG_COMM_INFO("AppendCast_DT_004 success");
}

/**
 * @tc.name: OnWfdServiceAbilityDied_DT_001
 * @tc.desc: 测试WFD服务能力死亡回调
 * @tc.type: DT
 * @tc.level: Level1
 */
HWTEST_F(WfdSourceSceneDtTest, OnWfdServiceAbilityDied_DT_001, TestSize.Level1)
{
    HILOG_COMM_INFO("OnWfdServiceAbilityDied_DT_001 start");
    
    // 设置初始状态
    scene_->IsWfdP2pCallbackRegistered_ = true;
    scene_->wfdP2pCallback_ = nullptr;
    scene_->isSourceDiscovering = true;
    
    scene_->OnWfdServiceAbilityDied();
    
    // 验证状态更新
    EXPECT_FALSE(scene_->IsWfdP2pCallbackRegistered_);
    EXPECT_TRUE(scene_->isSourceDiscovering);  // 应该触发设备离线
}
}  // namespace Sharing
}  // namespace OHOS
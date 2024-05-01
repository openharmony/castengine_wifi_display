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

#include "domain_unit_test.h"
#include <iostream>
#include "common/sharing_log.h"
#include "dm_kit.h"
#include "domain_manager.h"
#include "domain_rpc_client.h"
#include "domain_rpc_death_recipient.h"
#include "domain_rpc_manager.h"
#include "domain_rpc_service.h"
#include "domain_rpc_service_death_listener.h"
#include "domain_rpc_service_proxy.h"
#include "domain_rpc_service_stub.h"
#include "event/event_manager.h"
#include "if_system_ability_manager.h"
#include "interaction/interaction_manager.h"
#include "interaction/scene/scene_format.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

constexpr uint32_t TEST_SERVICE_SA_ID = 3301;

void DomainUnitTest::SetUpTestCase() {}
void DomainUnitTest::TearDownTestCase() {}
void DomainUnitTest::SetUp() {}
void DomainUnitTest::TearDown() {}

class DomainManagerListenerImpl : public DomainManagerListener {
public:
    int32_t OnDomainMsg(std::shared_ptr<BaseDomainMsg> msg)
    {
        (void)msg;
        return 0;
    }
};

class IDomainPeerListenerImpl : public IDomainPeerListener {
public:
    void OnPeerConnected(std::string remoteId)
    {
        (void)remoteId;
    }
    void OnPeerDisconnected(std::string remoteId)
    {
        (void)remoteId;
    }
    void OnDomainRequest(std::string remoteId, std::shared_ptr<BaseDomainMsg> baseMsg)
    {
        (void)remoteId;
        (void)baseMsg;
    }
};

class DomainRpcDeathListenerImpl : public DomainRpcDeathListener {
public:
    void OnRemoteDied(std::string deviceId)
    {
        (void)deviceId;
    }
};

class IDomainStubListenerImpl : public IDomainStubListener {
public:
    void OnDomainRequest(std::shared_ptr<BaseDomainMsg> msg)
    {
        (void)msg;
    }
};

namespace {
HWTEST_F(DomainUnitTest, DomainManager_Test_001, testing::ext::TestSize.Level1)
{
    std::string remoteId = "dsads";
    auto ret = DomainManager::GetInstance()->DelPeer(remoteId);
    EXPECT_NE(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainManager_Test_002, testing::ext::TestSize.Level1)
{
    std::shared_ptr<ITransmitMgr> mgr = nullptr;
    std::shared_ptr<IDomainPeer> caller = nullptr;
    auto ret = DomainManager::GetInstance()->AddPeer(mgr, caller);
    EXPECT_NE(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainManager_Test_003, testing::ext::TestSize.Level1)
{
    std::string remoteId = "sdsdsd";
    auto baseMsg = std::make_shared<BaseDomainMsg>();
    EXPECT_NE(baseMsg, nullptr);
    DomainManager::GetInstance()->OnDomainRequest(remoteId, baseMsg);
}

HWTEST_F(DomainUnitTest, DomainManager_Test_004, testing::ext::TestSize.Level1)
{
    auto listener = std::make_shared<DomainManagerListenerImpl>();
    EXPECT_NE(listener, nullptr);
    DomainManager::GetInstance()->SetListener(listener);
}

HWTEST_F(DomainUnitTest, DomainManager_Test_005, testing::ext::TestSize.Level1)
{
    auto listener = std::make_shared<DomainManagerListenerImpl>();
    EXPECT_NE(listener, nullptr);
    std::shared_ptr<ITransmitMgr> mgr = nullptr;
    auto ret = DomainManager::GetInstance()->AddServiceManager(mgr);
    EXPECT_NE(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainManager_Test_006, testing::ext::TestSize.Level1)
{
    std::string remoteId = "sdsdsd";
    auto baseMsg = std::make_shared<BaseDomainMsg>();
    EXPECT_NE(baseMsg, nullptr);
    auto ret = DomainManager::GetInstance()->SendDomainRequest(remoteId, baseMsg);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainManager_Test_007, testing::ext::TestSize.Level1)
{
    auto listener = std::make_shared<DomainManagerListenerImpl>();
    EXPECT_NE(listener, nullptr);
    std::string remoteId = "sdsdsd";
    auto ret = DomainManager::GetInstance()->FindMgrByRemoteId(remoteId);
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(DomainUnitTest, DomainManager_Test_008, testing::ext::TestSize.Level1)
{
    auto listener = std::make_shared<DomainManagerListenerImpl>();
    EXPECT_NE(listener, nullptr);
    std::string remoteId = "sdsdsd";
    std::shared_ptr<ITransmitMgr> mgr = nullptr;
    DomainType type = DOMAIN_TYPE_RPC;
    DomainManager::GetInstance()->peerTypeMap_.emplace(remoteId, type);
    DomainManager::GetInstance()->transmitMgrs_.emplace(type, mgr);
    auto ret = DomainManager::GetInstance()->FindMgrByRemoteId(remoteId);
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(DomainUnitTest, DomainRpcClient_Test_009, testing::ext::TestSize.Level1)
{
    auto rpcClient = std::make_shared<DomainRpcClient>();
    EXPECT_NE(rpcClient, nullptr);
}

HWTEST_F(DomainUnitTest, DomainRpcClient_Test_010, testing::ext::TestSize.Level1)
{
    auto rpcClient = std::make_shared<DomainRpcClient>();
    EXPECT_NE(rpcClient, nullptr);
    rpcClient->Initialize();
}

HWTEST_F(DomainUnitTest, DomainRpcClient_Test_011, testing::ext::TestSize.Level1)
{
    auto rpcClient = std::make_shared<DomainRpcClient>();
    EXPECT_NE(rpcClient, nullptr);
    std::string remoteId = "sdsdsd";
    rpcClient->OnRemoteDied(remoteId);
}

HWTEST_F(DomainUnitTest, DomainRpcClient_Test_012, testing::ext::TestSize.Level1)
{
    auto rpcClient = std::make_shared<DomainRpcClient>();
    EXPECT_NE(rpcClient, nullptr);
    auto msg = std::make_shared<BaseDomainMsg>();
    EXPECT_NE(msg, nullptr);
    rpcClient->OnDomainRequest(msg);
}

HWTEST_F(DomainUnitTest, DomainRpcClient_Test_013, testing::ext::TestSize.Level1)
{
    auto rpcClient = std::make_shared<DomainRpcClient>();
    EXPECT_NE(rpcClient, nullptr);
    auto ret = rpcClient->CreateListenerObject();
    EXPECT_NE(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcClient_Test_014, testing::ext::TestSize.Level1)
{
    auto rpcClient = std::make_shared<DomainRpcClient>();
    EXPECT_NE(rpcClient, nullptr);
    sptr<IDomainRpcService> peerProxy = nullptr;
    rpcClient->SetDomainProxy(peerProxy);
}

HWTEST_F(DomainUnitTest, DomainRpcClient_Test_015, testing::ext::TestSize.Level1)
{
    auto rpcClient = std::make_shared<DomainRpcClient>();
    EXPECT_NE(rpcClient, nullptr);
    auto ret = rpcClient->GetSubProxy(0);
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(DomainUnitTest, DomainRpcClient_Test_016, testing::ext::TestSize.Level1)
{
    auto rpcClient = std::make_shared<DomainRpcClient>();
    EXPECT_NE(rpcClient, nullptr);
    std::string remoteId = "sdsdsd";
    auto ret = rpcClient->GetDomainProxy(remoteId);
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(DomainUnitTest, DomainRpcClient_Test_017, testing::ext::TestSize.Level1)
{
    auto rpcClient = std::make_shared<DomainRpcClient>();
    EXPECT_NE(rpcClient, nullptr);
    auto listener = std::make_shared<IDomainPeerListenerImpl>();
    EXPECT_NE(listener, nullptr);
    rpcClient->SetPeerListener(listener);
}

HWTEST_F(DomainUnitTest, DomainRpcClient_Test_018, testing::ext::TestSize.Level1)
{
    auto rpcClient = std::make_shared<DomainRpcClient>();
    EXPECT_NE(rpcClient, nullptr);
    std::string remoteId = "sdsdsd";
    auto msg = std::make_shared<BaseDomainMsg>();
    EXPECT_NE(msg, nullptr);
    auto ret = rpcClient->SendDomainRequest(remoteId, msg);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcClient_Test_019, testing::ext::TestSize.Level1)
{
    auto rpcClient = std::make_shared<DomainRpcClient>();
    EXPECT_NE(rpcClient, nullptr);
    std::shared_ptr<IDomainPeerListener> listener = nullptr;
    rpcClient->SetPeerListener(listener);
}

HWTEST_F(DomainUnitTest, DomainRpcDeathRecipient_Test_020, testing::ext::TestSize.Level1)
{
    std::string deviceId = "sdsdsd";
    auto domainRpcDeathRecipient = std::make_shared<DomainRpcDeathRecipient>(deviceId);
    EXPECT_NE(domainRpcDeathRecipient, nullptr);
}

HWTEST_F(DomainUnitTest, DomainRpcDeathRecipient_Test_021, testing::ext::TestSize.Level1)
{
    std::string deviceId = "sdsdsd";
    auto domainRpcDeathRecipient = std::make_shared<DomainRpcDeathRecipient>(deviceId);
    EXPECT_NE(domainRpcDeathRecipient, nullptr);
    auto listener = std::make_shared<DomainRpcDeathListenerImpl>();
    domainRpcDeathRecipient->SetDeathListener(listener);
}

HWTEST_F(DomainUnitTest, DomainRpcDeathRecipient_Test_022, testing::ext::TestSize.Level1)
{
    std::string deviceId = "sdsdsd";
    auto domainRpcDeathRecipient = std::make_shared<DomainRpcDeathRecipient>(deviceId);
    EXPECT_NE(domainRpcDeathRecipient, nullptr);
    auto listener = std::make_shared<DomainRpcDeathListenerImpl>();
    domainRpcDeathRecipient->SetDeathListener(listener);
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> object = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(object, nullptr);
    domainRpcDeathRecipient->OnRemoteDied(object);
}

HWTEST_F(DomainUnitTest, DomainRpcManager_Test_023, testing::ext::TestSize.Level1)
{
    std::string peerId = "1212";
    auto ret = DomainRpcManager::GetInstance()->IsPeerExist(peerId);
    EXPECT_NE(ret, true);
}

HWTEST_F(DomainUnitTest, DomainRpcManager_Test_024, testing::ext::TestSize.Level1)
{
    std::string peerId = "1212";
    DomainRpcManager::GetInstance()->rpcClients_.emplace(peerId, nullptr);
    auto ret = DomainRpcManager::GetInstance()->IsPeerExist(peerId);
    EXPECT_EQ(ret, true);
}

HWTEST_F(DomainUnitTest, DomainRpcManager_Test_025, testing::ext::TestSize.Level1)
{
    std::string remoteId = "1212";
    auto msg = std::make_shared<BaseDomainMsg>();
    EXPECT_NE(msg, nullptr);
    auto ret = DomainRpcManager::GetInstance()->SendDomainRequest(remoteId, msg);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcManager_Test_026, testing::ext::TestSize.Level1)
{
    std::string remoteId = "1212";
    auto ret = DomainRpcManager::GetInstance()->DelRpcClient(remoteId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcManager_Test_027, testing::ext::TestSize.Level1)
{
    std::string remoteId = "1212";
    DomainRpcManager::GetInstance()->rpcClients_.emplace(remoteId, nullptr);
    auto ret = DomainRpcManager::GetInstance()->DelRpcClient(remoteId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcManager_Test_028, testing::ext::TestSize.Level1)
{
    std::string remoteId = "1212";
    auto ret = DomainRpcManager::GetInstance()->AddRpcClient(remoteId);
    EXPECT_NE(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcManager_Test_029, testing::ext::TestSize.Level1)
{
    DomainRpcService domainRpcService{100, true};
    auto ret = DomainRpcManager::GetInstance()->AddDomainRpcService(&domainRpcService);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcManager_Test_030, testing::ext::TestSize.Level1)
{
    std::string remoteId = "1212";
    sptr<IDomainRpcService> peerProxy = nullptr;
    auto ret = DomainRpcManager::GetInstance()->AddRpcClient(remoteId, peerProxy);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcManager_Test_031, testing::ext::TestSize.Level1)
{
    std::string remoteId = "1212";
    DomainRpcManager::GetInstance()->OnPeerConnected(remoteId);
}

HWTEST_F(DomainUnitTest, DomainRpcManager_Test_032, testing::ext::TestSize.Level1)
{
    std::string remoteId = "1212";
    DomainRpcManager::GetInstance()->OnPeerDisconnected(remoteId);
}

HWTEST_F(DomainUnitTest, DomainRpcManager_Test_033, testing::ext::TestSize.Level1)
{
    std::string remoteId = "1212";
    auto msg = std::make_shared<BaseDomainMsg>();
    EXPECT_NE(msg, nullptr);
    DomainRpcManager::GetInstance()->OnDomainRequest(remoteId, msg);
}

HWTEST_F(DomainUnitTest, DomainRpcManager_Test_034, testing::ext::TestSize.Level1)
{
    auto ret = DomainRpcManager::GetInstance()->Init();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcManager_Test_035, testing::ext::TestSize.Level1)
{
    auto ret = DomainRpcManager::GetInstance()->DeInit();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcManager_Test_036, testing::ext::TestSize.Level1)
{
    auto ret = DomainRpcManager::GetInstance()->ClearRpcClient();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcServiceDeathListener_Test_037, testing::ext::TestSize.Level1)
{
    auto domainRpcServiceDeathListener = std::make_shared<DomainRpcServiceDeathListener>();
    EXPECT_NE(domainRpcServiceDeathListener, nullptr);
}

HWTEST_F(DomainUnitTest, DomainRpcServiceDeathListener_Test_038, testing::ext::TestSize.Level1)
{
    std::string deviceId = "1212";
    auto domainRpcServiceDeathListener = std::make_shared<DomainRpcServiceDeathListener>();
    EXPECT_NE(domainRpcServiceDeathListener, nullptr);
    domainRpcServiceDeathListener->OnRemoteDied(deviceId);
}

HWTEST_F(DomainUnitTest, DomainRpcServiceDeathListener_Test_039, testing::ext::TestSize.Level1)
{
    auto domainRpcServiceDeathListener = std::make_shared<DomainRpcServiceDeathListener>();
    EXPECT_NE(domainRpcServiceDeathListener, nullptr);
    int32_t systemAbilityId = 0;
    bool runOnCreate = true;
    auto service = std::make_shared<DomainRpcService>(systemAbilityId, runOnCreate);
    EXPECT_NE(service, nullptr);
    domainRpcServiceDeathListener->SetService(service);
}

HWTEST_F(DomainUnitTest, DomainRpcServiceProxy_Test_040, testing::ext::TestSize.Level1)
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> impl = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(impl, nullptr);
    auto domainRpcServiceProxy = std::make_shared<DomainRpcServiceProxy>(impl);
    EXPECT_NE(domainRpcServiceProxy, nullptr);
}

HWTEST_F(DomainUnitTest, DomainRpcServiceProxy_Test_041, testing::ext::TestSize.Level1)
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> impl = systemAbilityManager->GetSystemAbility(SHARING_SERVICE_TEMP_SA_ID);
    EXPECT_NE(impl, nullptr);
    sptr<IRemoteObject> object = systemAbilityManager->GetSystemAbility(SHARING_SERVICE_TEMP_SA_ID);
    EXPECT_NE(object, nullptr);
    auto domainRpcServiceProxy = std::make_shared<DomainRpcServiceProxy>(impl);
    EXPECT_NE(domainRpcServiceProxy, nullptr);
    auto ret = domainRpcServiceProxy->SetListenerObject(object);
    EXPECT_NE(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcServiceProxy_Test_042, testing::ext::TestSize.Level1)
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> impl = systemAbilityManager->GetSystemAbility(SHARING_SERVICE_TEMP_SA_ID);
    EXPECT_NE(impl, nullptr);
    auto domainRpcServiceProxy = std::make_shared<DomainRpcServiceProxy>(impl);
    EXPECT_NE(domainRpcServiceProxy, nullptr);
    auto msg = std::make_shared<BaseDomainMsg>();
    EXPECT_NE(msg, nullptr);
    auto replyMsg = std::make_shared<BaseDomainMsg>();
    EXPECT_NE(replyMsg, nullptr);
    auto ret = domainRpcServiceProxy->DoRpcCommand(msg, replyMsg);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcServiceProxy_Test_043, testing::ext::TestSize.Level1)
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> impl = systemAbilityManager->GetSystemAbility(SHARING_SERVICE_TEMP_SA_ID);
    EXPECT_NE(impl, nullptr);
    auto domainRpcServiceProxy = std::make_shared<DomainRpcServiceProxy>(impl);
    EXPECT_NE(domainRpcServiceProxy, nullptr);
    int32_t type = 0;
    auto ret = domainRpcServiceProxy->GetSubSystemAbility(type);
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(DomainUnitTest, DomainRpcServiceStub_Test_044, testing::ext::TestSize.Level1)
{
    auto domainRpcServiceStub = std::make_shared<DomainRpcServiceStub>();
    EXPECT_NE(domainRpcServiceStub, nullptr);
}

HWTEST_F(DomainUnitTest, DomainRpcServiceStub_Test_045, testing::ext::TestSize.Level1)
{
    auto domainRpcServiceStub = std::make_shared<DomainRpcServiceStub>();
    EXPECT_NE(domainRpcServiceStub, nullptr);
    int32_t type = 0;
    auto ret = domainRpcServiceStub->GetSubSystemAbility(type);
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(DomainUnitTest, DomainRpcServiceStub_Test_046, testing::ext::TestSize.Level1)
{
    auto domainRpcServiceStub = std::make_shared<DomainRpcServiceStub>();
    EXPECT_NE(domainRpcServiceStub, nullptr);
    auto listener = std::make_shared<IDomainStubListenerImpl>();
    EXPECT_NE(listener, nullptr);
    domainRpcServiceStub->SetStubListener(listener);
}

HWTEST_F(DomainUnitTest, DomainRpcServiceStub_Test_047, testing::ext::TestSize.Level1)
{
    auto domainRpcServiceStub = std::make_shared<DomainRpcServiceStub>();
    EXPECT_NE(domainRpcServiceStub, nullptr);
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> object = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(object, nullptr);
    auto ret = domainRpcServiceStub->SetListenerObject(object);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcServiceStub_Test_048, testing::ext::TestSize.Level1)
{
    auto domainRpcServiceStub = std::make_shared<DomainRpcServiceStub>();
    EXPECT_NE(domainRpcServiceStub, nullptr);
    auto msg = std::make_shared<BaseDomainMsg>();
    EXPECT_NE(msg, nullptr);
    auto replyMsg = std::make_shared<BaseDomainMsg>();
    EXPECT_NE(replyMsg, nullptr);
    auto ret = domainRpcServiceStub->DoRpcCommand(msg, replyMsg);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcServiceStub_Test_049, testing::ext::TestSize.Level1)
{
    auto domainRpcServiceStub = std::make_shared<DomainRpcServiceStub>();
    EXPECT_NE(domainRpcServiceStub, nullptr);
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto ret = domainRpcServiceStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcServiceStub_Test_050, testing::ext::TestSize.Level1)
{
    auto domainRpcServiceStub = std::make_shared<DomainRpcServiceStub>();
    EXPECT_NE(domainRpcServiceStub, nullptr);
    std::string deviceId = "sdsdsdsd";
    domainRpcServiceStub->CreateDeathListener(deviceId);
}

HWTEST_F(DomainUnitTest, DomainRpcServiceStub_Test_051, testing::ext::TestSize.Level1)
{
    auto domainRpcServiceStub = std::make_shared<DomainRpcServiceStub>();
    EXPECT_NE(domainRpcServiceStub, nullptr);
    MessageParcel data;
    MessageParcel reply;
    auto ret = domainRpcServiceStub->DoRpcCommand(data, reply);
    EXPECT_NE(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcServiceStub_Test_052, testing::ext::TestSize.Level1)
{
    auto domainRpcServiceStub = std::make_shared<DomainRpcServiceStub>();
    EXPECT_NE(domainRpcServiceStub, nullptr);
    MessageParcel data;
    MessageParcel reply;
    auto ret = domainRpcServiceStub->GetSystemAbility(data, reply);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcServiceStub_Test_053, testing::ext::TestSize.Level1)
{
    auto domainRpcServiceStub = std::make_shared<DomainRpcServiceStub>();
    EXPECT_NE(domainRpcServiceStub, nullptr);
    MessageParcel data;
    MessageParcel reply;
    auto ret = domainRpcServiceStub->SetListenerObject(data, reply);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcService_Test_054, testing::ext::TestSize.Level1)
{
    int32_t systemAbilityId = 0;
    bool runOnCreate = true;
    auto domainRpcService = std::make_shared<DomainRpcService>(systemAbilityId, runOnCreate);
    EXPECT_NE(domainRpcService, nullptr);
}

HWTEST_F(DomainUnitTest, DomainRpcService_Test_055, testing::ext::TestSize.Level1)
{
    int32_t systemAbilityId = 0;
    bool runOnCreate = true;
    auto domainRpcService = std::make_shared<DomainRpcService>(systemAbilityId, runOnCreate);
    EXPECT_NE(domainRpcService, nullptr);
    std::string remoteId = "sdsdsdsd";
    domainRpcService->DelPeerProxy(remoteId);
}

HWTEST_F(DomainUnitTest, DomainRpcService_Test_056, testing::ext::TestSize.Level1)
{
    int32_t systemAbilityId = 0;
    bool runOnCreate = true;
    auto domainRpcService = std::make_shared<DomainRpcService>(systemAbilityId, runOnCreate);
    EXPECT_NE(domainRpcService, nullptr);
    std::string remoteId = "sdsdsdsd";
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> object = systemAbilityManager->GetSystemAbility(SHARING_SERVICE_TEMP_SA_ID);
    EXPECT_NE(object, nullptr);
    domainRpcService->DomainRpcStubs_.emplace(remoteId, object);
    domainRpcService->DelPeerProxy(remoteId);
}

HWTEST_F(DomainUnitTest, DomainRpcService_Test_057, testing::ext::TestSize.Level1)
{
    int32_t systemAbilityId = 0;
    bool runOnCreate = true;
    auto domainRpcService = std::make_shared<DomainRpcService>(systemAbilityId, runOnCreate);
    EXPECT_NE(domainRpcService, nullptr);
    std::string deviceId = "sdsdsdsd";
    domainRpcService->CreateDeathListener(deviceId);
}

HWTEST_F(DomainUnitTest, DomainRpcService_Test_058, testing::ext::TestSize.Level1)
{
    int32_t systemAbilityId = 0;
    bool runOnCreate = true;
    auto domainRpcService = std::make_shared<DomainRpcService>(systemAbilityId, runOnCreate);
    EXPECT_NE(domainRpcService, nullptr);
    auto listen = std::make_shared<IDomainPeerListenerImpl>();
    EXPECT_NE(listen, nullptr);
    domainRpcService->SetPeerListener(listen);
}

HWTEST_F(DomainUnitTest, DomainRpcService_Test_059, testing::ext::TestSize.Level1)
{
    int32_t systemAbilityId = 0;
    bool runOnCreate = true;
    auto domainRpcService = std::make_shared<DomainRpcService>(systemAbilityId, runOnCreate);
    EXPECT_NE(domainRpcService, nullptr);
    std::string remoteId = "sdsdsd";
    auto baseMsg = std::make_shared<BaseDomainMsg>();
    EXPECT_NE(baseMsg, nullptr);
    auto ret = domainRpcService->SendDomainRequest(remoteId, baseMsg);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcService_Test_060, testing::ext::TestSize.Level1)
{
    int32_t systemAbilityId = 0;
    bool runOnCreate = true;
    auto domainRpcService = std::make_shared<DomainRpcService>(systemAbilityId, runOnCreate);
    EXPECT_NE(domainRpcService, nullptr);
    std::string remoteId = "sdsdsd";
    auto baseMsg = std::make_shared<BaseDomainMsg>();
    EXPECT_NE(baseMsg, nullptr);
    auto replyMsg = std::make_shared<BaseDomainMsg>();
    EXPECT_NE(replyMsg, nullptr);
    auto ret = domainRpcService->DoRpcCommand(baseMsg, replyMsg);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DomainUnitTest, DomainRpcService_Test_061, testing::ext::TestSize.Level1)
{
    int32_t systemAbilityId = 0;
    bool runOnCreate = true;
    auto domainRpcService = std::make_shared<DomainRpcService>(systemAbilityId, runOnCreate);
    EXPECT_NE(domainRpcService, nullptr);
    int32_t type = 0;
    auto ret = domainRpcService->GetSubSystemAbility(type);
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(DomainUnitTest, DomainRpcService_Test_062, testing::ext::TestSize.Level1)
{
    int32_t systemAbilityId = 0;
    bool runOnCreate = true;
    auto domainRpcService = std::make_shared<DomainRpcService>(systemAbilityId, runOnCreate);
    EXPECT_NE(domainRpcService, nullptr);
    domainRpcService->OnDump();
}

HWTEST_F(DomainUnitTest, DomainRpcService_Test_063, testing::ext::TestSize.Level1)
{
    int32_t systemAbilityId = 0;
    bool runOnCreate = true;
    auto domainRpcService = std::make_shared<DomainRpcService>(systemAbilityId, runOnCreate);
    EXPECT_NE(domainRpcService, nullptr);
    domainRpcService->OnStop();
}

HWTEST_F(DomainUnitTest, DomainRpcService_Test_064, testing::ext::TestSize.Level1)
{
    int32_t systemAbilityId = 0;
    bool runOnCreate = true;
    auto domainRpcService = std::make_shared<DomainRpcService>(systemAbilityId, runOnCreate);
    EXPECT_NE(domainRpcService, nullptr);
    domainRpcService->OnStart();
}

HWTEST_F(DomainUnitTest, DmKitInitDMCallback_Test_065, testing::ext::TestSize.Level1)
{
    auto dmKitInitDMCallback = std::make_shared<DmKitInitDMCallback>();
    EXPECT_NE(dmKitInitDMCallback, nullptr);
}

HWTEST_F(DomainUnitTest, DmKitInitDMCallback_Test_066, testing::ext::TestSize.Level1)
{
    auto dmKitInitDMCallback = std::make_shared<DmKitInitDMCallback>();
    EXPECT_NE(dmKitInitDMCallback, nullptr);
    dmKitInitDMCallback->OnRemoteDied();
}

HWTEST_F(DomainUnitTest, DmKit_Test_067, testing::ext::TestSize.Level1)
{
    DmKit::InitDeviceManager();
}

HWTEST_F(DomainUnitTest, DmKit_Test_068, testing::ext::TestSize.Level1)
{
    DmKit::InitDeviceManager();
    const std::string networkId = "1212";
    auto ret = DmKit::GetIpAddrByNetworkId(networkId);
    EXPECT_EQ(ret, "");
}

HWTEST_F(DomainUnitTest, DmKit_Test_069, testing::ext::TestSize.Level1)
{
    DmKit::InitDeviceManager();
    auto &ret = DmKit::GetTrustedDevicesInfo();
    EXPECT_EQ(ret.size(), 0);
}

HWTEST_F(DomainUnitTest, DmKit_Test_070, testing::ext::TestSize.Level1)
{
    DmKit::InitDeviceManager();
    auto &ret = DmKit::GetLocalDevicesInfo();
    EXPECT_EQ(ret.deviceTypeId, 0);
}

} // namespace
} // namespace Sharing
} // namespace OHOS
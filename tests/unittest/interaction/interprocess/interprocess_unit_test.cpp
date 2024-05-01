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

#include "interprocess_unit_test.h"
#include <iostream>
#include "client_factory.h"
#include "common/sharing_log.h"
#include "event/event_manager.h"
#include "if_system_ability_manager.h"
#include "inter_ipc_client.h"
#include "inter_ipc_client_stub.h"
#include "inter_ipc_death_recipient.h"
#include "inter_ipc_proxy.h"
#include "inter_ipc_service.h"
#include "inter_ipc_service_death_listener.h"
#include "inter_ipc_service_stub.h"
#include "inter_ipc_stub.h"
#include "inter_ipc_stub_death_listener.h"
#include "inter_ipc_sub_stub.h"
#include "interaction/interaction_manager.h"
#include "interaction/scene/scene_format.h"
#include "ipc_msg_adapter.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

constexpr uint32_t TEST_SERVICE_SA_ID = 3301;

void InterProcessUnitTest::SetUpTestCase() {}
void InterProcessUnitTest::TearDownTestCase() {}
void InterProcessUnitTest::SetUp() {}
void InterProcessUnitTest::TearDown() {}

class InterIpcDeathListenerImpl : public InterIpcDeathListener {
public:
    void OnRemoteDied(std::string key)
    {
        (void)key;
    }
};

class IInterIpcStubListenerImpl : public IInterIpcStubListener {
public:
    void OnRemoteDied() {}
    void OnIpcRequest(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply)
    {
        (void)msg;
        (void)reply;
    }
};

class MsgAdapterListenerImpl : public MsgAdapterListener {
public:
    void OnRemoteDied() {}
    void OnRequest(std::shared_ptr<BaseMsg> msg, std::shared_ptr<BaseMsg> &reply)
    {
        (void)msg;
        (void)reply;
    }
};

namespace {

HWTEST_F(InterProcessUnitTest, ClientFactory_Test_001, testing::ext::TestSize.Level1)
{
    auto clientFactory = std::make_shared<ClientFactory>();
    EXPECT_NE(clientFactory, nullptr);
}

HWTEST_F(InterProcessUnitTest, ClientFactory_Test_002, testing::ext::TestSize.Level1)
{
    std::string key = "DADADASDSD";
    std::string clientClassName = "WfdSourceImpl";
    std::string serverClassName = "WfdSourceScene";
    auto ipcClient = ClientFactory::GetInstance().CreateClient(key, clientClassName, serverClassName);
    EXPECT_NE(ipcClient, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcClientStub_Test_003, testing::ext::TestSize.Level1)
{
    auto ipcClient = std::make_shared<InterIpcClientStub>();
    EXPECT_NE(ipcClient, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcClientStub_Test_004, testing::ext::TestSize.Level1)
{
    auto ipcClient = std::make_shared<InterIpcClientStub>();
    EXPECT_NE(ipcClient, nullptr);
    ipcClient->OnRemoteDied();
}

HWTEST_F(InterProcessUnitTest, InterIpcClientStub_Test_005, testing::ext::TestSize.Level1)
{
    SHARING_LOGD("ut trace start.");
    auto ipcClient = std::make_shared<InterIpcClientStub>();
    EXPECT_NE(ipcClient, nullptr);
    std::string key = "dadsadad";
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> object = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(object, nullptr);
    auto ret = ipcClient->SetListenerObject(key, object);
    EXPECT_EQ(ret, 0);
    SHARING_LOGD("ut trace stop.");
}

HWTEST_F(InterProcessUnitTest, InterIpcClientStub_Test_006, testing::ext::TestSize.Level1)
{
    auto ipcClient = std::make_shared<InterIpcClientStub>();
    EXPECT_NE(ipcClient, nullptr);
    auto msg = std::make_shared<BaseMsg>();
    EXPECT_NE(msg, nullptr);
    auto replyMsg = std::make_shared<BaseMsg>();
    EXPECT_NE(replyMsg, nullptr);
    auto ret = ipcClient->DoIpcCommand(msg, replyMsg);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(InterProcessUnitTest, InterIpcClient_Test_007, testing::ext::TestSize.Level1)
{
    auto ipcClient = std::make_shared<InterIpcClient>();
    EXPECT_NE(ipcClient, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcClient_Test_008, testing::ext::TestSize.Level1)
{
    auto ipcClient = std::make_shared<InterIpcClient>();
    EXPECT_NE(ipcClient, nullptr);
    auto ret = ipcClient->CreateListenerObject();
    EXPECT_EQ(ret, -1);
}

HWTEST_F(InterProcessUnitTest, InterIpcClient_Test_009, testing::ext::TestSize.Level1)
{
    auto ipcClient = std::make_shared<InterIpcClient>();
    EXPECT_NE(ipcClient, nullptr);
    std::string key = "dadsadad";
    ipcClient->OnRemoteDied(key);
}

HWTEST_F(InterProcessUnitTest, InterIpcClient_Test_010, testing::ext::TestSize.Level1)
{
    auto ipcClient = std::make_shared<InterIpcClient>();
    EXPECT_NE(ipcClient, nullptr);
    sptr<IInterIpc> standardProxy = nullptr;
    ipcClient->Initialize(standardProxy);
}

HWTEST_F(InterProcessUnitTest, InterIpcClient_Test_011, testing::ext::TestSize.Level1)
{
    auto ipcClient = std::make_shared<InterIpcClient>();
    EXPECT_NE(ipcClient, nullptr);
    auto ret = ipcClient->GetSharingProxy();
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcClient_Test_012, testing::ext::TestSize.Level1)
{
    auto ipcClient = std::make_shared<InterIpcClient>();
    EXPECT_NE(ipcClient, nullptr);
    std::string key = "DADADASDSD";
    std::string clientClassName = "WfdSourceImpl";
    auto ret = ipcClient->GetSubProxy(key, clientClassName);
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcClient_Test_013, testing::ext::TestSize.Level1)
{
    auto ipcClient = std::make_shared<InterIpcClient>();
    EXPECT_NE(ipcClient, nullptr);
    std::string key = "DADADASDSD";
    std::string clientClassName = "WfdSourceImpl";
    std::string serverClassName = "WfdSourceScene";
    auto ret = ipcClient->CreateSubService(key, clientClassName, serverClassName);
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcClient_Test_014, testing::ext::TestSize.Level1)
{
    auto ipcClient = std::make_shared<InterIpcClient>();
    EXPECT_NE(ipcClient, nullptr);
    std::string key = "DADADASDSD";
    ipcClient->SetKey(key);
}

HWTEST_F(InterProcessUnitTest, InterIpcClient_Test_015, testing::ext::TestSize.Level1)
{
    auto ipcClient = std::make_shared<InterIpcClient>();
    EXPECT_NE(ipcClient, nullptr);
    auto ret = ipcClient->GetMsgAdapter();
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcClient_Test_016, testing::ext::TestSize.Level1)
{
    auto ipcClient = std::make_shared<InterIpcClient>();
    EXPECT_NE(ipcClient, nullptr);
    auto msg = std::make_shared<BaseMsg>();
    EXPECT_NE(msg, nullptr);
    auto replyMsg = std::make_shared<BaseMsg>();
    EXPECT_NE(replyMsg, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcClient_Test_017, testing::ext::TestSize.Level1)
{
    auto ipcClient = std::make_shared<InterIpcClient>();
    EXPECT_NE(ipcClient, nullptr);
    auto replyMsg = std::make_shared<BaseMsg>();
    EXPECT_NE(replyMsg, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcDeathRecipient_Test_018, testing::ext::TestSize.Level1)
{
    std::string key = "DSDSDSDS";
    auto ipcDeathRecipient = std::make_shared<InterIpcDeathRecipient>(key);
    EXPECT_NE(ipcDeathRecipient, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcDeathRecipient_Test_019, testing::ext::TestSize.Level1)
{
    std::string key = "DSDSDSDS";
    auto ipcDeathRecipient = std::make_shared<InterIpcDeathRecipient>(key);
    EXPECT_NE(ipcDeathRecipient, nullptr);
    auto listener = std::make_shared<InterIpcDeathListenerImpl>();
    EXPECT_NE(listener, nullptr);
    ipcDeathRecipient->SetDeathListener(listener);
}

HWTEST_F(InterProcessUnitTest, InterIpcDeathRecipient_Test_020, testing::ext::TestSize.Level1)
{
    std::string key = "DSDSDSDS";
    auto ipcDeathRecipient = std::make_shared<InterIpcDeathRecipient>(key);
    EXPECT_NE(ipcDeathRecipient, nullptr);
    auto listener = std::make_shared<InterIpcDeathListenerImpl>();
    EXPECT_NE(listener, nullptr);
    ipcDeathRecipient->SetDeathListener(listener);
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> object = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(object, nullptr);
    ipcDeathRecipient->OnRemoteDied(object);
}

HWTEST_F(InterProcessUnitTest, InterIpcProxy_Test_021, testing::ext::TestSize.Level1)
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> impl = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(impl, nullptr);
    auto ipcProxy = std::make_shared<InterIpcProxy>(impl);
    EXPECT_NE(ipcProxy, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcProxy_Test_022, testing::ext::TestSize.Level1)
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> impl = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(impl, nullptr);
    std::string key = "DSDSDSDS";
    sptr<IRemoteObject> object = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(object, nullptr);
    auto ipcProxy = std::make_shared<InterIpcProxy>(impl);
    EXPECT_NE(ipcProxy, nullptr);
    auto ret = ipcProxy->SetListenerObject(key, object);
    EXPECT_NE(ret, 0);
}

HWTEST_F(InterProcessUnitTest, InterIpcProxy_Test_023, testing::ext::TestSize.Level1)
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> impl = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(impl, nullptr);
    auto ipcProxy = std::make_shared<InterIpcProxy>(impl);
    EXPECT_NE(ipcProxy, nullptr);
    auto msg = std::make_shared<BaseMsg>();
    EXPECT_NE(msg, nullptr);
    auto replyMsg = std::make_shared<BaseMsg>();
    EXPECT_NE(replyMsg, nullptr);
    auto ret = ipcProxy->DoIpcCommand(msg, replyMsg);
    EXPECT_NE(ret, 0);
}

HWTEST_F(InterProcessUnitTest, InterIpcProxy_Test_024, testing::ext::TestSize.Level1)
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> impl = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(impl, nullptr);
    auto ipcProxy = std::make_shared<InterIpcProxy>(impl);
    EXPECT_NE(ipcProxy, nullptr);
    std::string key = "DADADASDSD";
    std::string clientClassName = "WfdSourceImpl";
    auto ret = ipcProxy->GetSubSystemAbility(key, clientClassName);
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcServiceDeathListener_Test_025, testing::ext::TestSize.Level1)
{
    auto ipcListener = std::make_shared<InterIpcServiceDeathListener>();
    EXPECT_NE(ipcListener, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcServiceDeathListener_Test_026, testing::ext::TestSize.Level1)
{
    auto ipcListener = std::make_shared<InterIpcServiceDeathListener>();
    EXPECT_NE(ipcListener, nullptr);
    std::string key = "DADADASDSD";
    ipcListener->OnRemoteDied(key);
}

HWTEST_F(InterProcessUnitTest, InterIpcServiceDeathListener_Test_027, testing::ext::TestSize.Level1)
{
    auto ipcListener = std::make_shared<InterIpcServiceDeathListener>();
    EXPECT_NE(ipcListener, nullptr);
    auto interIpcServiceStub = std::make_shared<InterIpcServiceStub>();
    EXPECT_NE(interIpcServiceStub, nullptr);
    ipcListener->SetService(interIpcServiceStub);
}

HWTEST_F(InterProcessUnitTest, InterIpcServiceStub_Test_028, testing::ext::TestSize.Level1)
{
    auto ipcServiceStub = std::make_shared<InterIpcServiceStub>();
    EXPECT_NE(ipcServiceStub, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcServiceStub_Test_029, testing::ext::TestSize.Level1)
{
    auto ipcServiceStub = std::make_shared<InterIpcServiceStub>();
    EXPECT_NE(ipcServiceStub, nullptr);
    std::string key = "DADADASDSD";
    ipcServiceStub->DelPeerProxy(key);
}

HWTEST_F(InterProcessUnitTest, InterIpcServiceStub_Test_030, testing::ext::TestSize.Level1)
{
    auto ipcServiceStub = std::make_shared<InterIpcServiceStub>();
    EXPECT_NE(ipcServiceStub, nullptr);
    std::string key = "DADADASDSD";
    ipcServiceStub->CreateDeathListener(key);
}

HWTEST_F(InterProcessUnitTest, InterIpcServiceStub_Test_031, testing::ext::TestSize.Level1)
{
    auto ipcServiceStub = std::make_shared<InterIpcServiceStub>();
    EXPECT_NE(ipcServiceStub, nullptr);
    std::string key = "dadsadad";
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> object = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(object, nullptr);
    auto ret = ipcServiceStub->SetListenerObject(key, object);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(InterProcessUnitTest, InterIpcServiceStub_Test_032, testing::ext::TestSize.Level1)
{
    auto ipcServiceStub = std::make_shared<InterIpcServiceStub>();
    EXPECT_NE(ipcServiceStub, nullptr);
    auto msg = std::make_shared<BaseMsg>();
    EXPECT_NE(msg, nullptr);
    auto replyMsg = std::make_shared<BaseMsg>();
    EXPECT_NE(replyMsg, nullptr);
    auto ret = ipcServiceStub->DoIpcCommand(msg, replyMsg);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(InterProcessUnitTest, InterIpcServiceStub_Test_033, testing::ext::TestSize.Level1)
{
    auto ipcServiceStub = std::make_shared<InterIpcServiceStub>();
    EXPECT_NE(ipcServiceStub, nullptr);
    std::string key = "DADADASDSD";
    std::string clientClassName = "WfdSourceImpl";
    auto ret = ipcServiceStub->GetSubSystemAbility(key, clientClassName);
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcService_Test_034, testing::ext::TestSize.Level1)
{
    int32_t systemAbilityId = 100;
    bool runOnCreate = true;
    auto interIpcService = std::make_shared<InterIpcService>(systemAbilityId, runOnCreate);
    EXPECT_NE(interIpcService, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcService_Test_035, testing::ext::TestSize.Level1)
{
    int32_t systemAbilityId = 100;
    bool runOnCreate = true;
    auto interIpcService = std::make_shared<InterIpcService>(systemAbilityId, runOnCreate);
    EXPECT_NE(interIpcService, nullptr);
    interIpcService->OnDump();
}

HWTEST_F(InterProcessUnitTest, InterIpcService_Test_036, testing::ext::TestSize.Level1)
{
    int32_t systemAbilityId = 100;
    bool runOnCreate = true;
    auto interIpcService = std::make_shared<InterIpcService>(systemAbilityId, runOnCreate);
    EXPECT_NE(interIpcService, nullptr);
    interIpcService->OnStop();
}

HWTEST_F(InterProcessUnitTest, InterIpcService_Test_037, testing::ext::TestSize.Level1)
{
    int32_t systemAbilityId = 100;
    bool runOnCreate = true;
    auto interIpcService = std::make_shared<InterIpcService>(systemAbilityId, runOnCreate);
    EXPECT_NE(interIpcService, nullptr);
    interIpcService->OnStart();
}

HWTEST_F(InterProcessUnitTest, InterIpcService_Test_038, testing::ext::TestSize.Level1)
{
    int32_t systemAbilityId = 100;
    bool runOnCreate = true;
    auto interIpcService = std::make_shared<InterIpcService>(systemAbilityId, runOnCreate);
    EXPECT_NE(interIpcService, nullptr);
    interIpcService->RecordProcessIdByKvStore();
}

HWTEST_F(InterProcessUnitTest, InterIpcStubDeathListener_Test_039, testing::ext::TestSize.Level1)
{
    auto ipcStub = std::make_shared<InterIpcStub>();
    EXPECT_NE(ipcStub, nullptr);
    auto ipcListener = std::make_shared<InterIpcStubDeathListener>(ipcStub);
    EXPECT_NE(ipcListener, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcStubDeathListener_Test_040, testing::ext::TestSize.Level1)
{
    auto ipcStub = std::make_shared<InterIpcStub>();
    EXPECT_NE(ipcStub, nullptr);
    auto ipcListener = std::make_shared<InterIpcStubDeathListener>(ipcStub);
    EXPECT_NE(ipcListener, nullptr);
    std::string key = "DADADASDSD";
    ipcListener->OnRemoteDied(key);
}

HWTEST_F(InterProcessUnitTest, InterIpcStub_Test_041, testing::ext::TestSize.Level1)
{
    auto interIpcStub = std::make_shared<InterIpcStub>();
    EXPECT_NE(interIpcStub, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcStub_Test_042, testing::ext::TestSize.Level1)
{
    auto interIpcStub = std::make_shared<InterIpcStub>();
    EXPECT_NE(interIpcStub, nullptr);
    interIpcStub->OnRemoteDied();
}

HWTEST_F(InterProcessUnitTest, InterIpcStub_Test_043, testing::ext::TestSize.Level1)
{
    auto interIpcStub = std::make_shared<InterIpcStub>();
    EXPECT_NE(interIpcStub, nullptr);
    auto listener = std::make_shared<IInterIpcStubListenerImpl>();
    EXPECT_NE(listener, nullptr);
    interIpcStub->SetStubListener(listener);
}

HWTEST_F(InterProcessUnitTest, InterIpcStub_Test_044, testing::ext::TestSize.Level1)
{
    auto interIpcStub = std::make_shared<InterIpcStub>();
    EXPECT_NE(interIpcStub, nullptr);
    std::string tokenId = "1212121212";
    auto msg = std::make_shared<BaseMsg>();
    EXPECT_NE(msg, nullptr);
    auto replyMsg = std::make_shared<BaseMsg>();
    EXPECT_NE(replyMsg, nullptr);
    auto ret = interIpcStub->SendIpcRequest(tokenId, msg, replyMsg);
    EXPECT_NE(ret, 0);
}

HWTEST_F(InterProcessUnitTest, InterIpcStub_Test_045, testing::ext::TestSize.Level1)
{
    auto interIpcStub = std::make_shared<InterIpcStub>();
    EXPECT_NE(interIpcStub, nullptr);
    std::string key = "1212121212";
    const sptr<IRemoteObject> object = nullptr;
    auto ret = interIpcStub->SetListenerObject(key, object);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(InterProcessUnitTest, InterIpcStub_Test_046, testing::ext::TestSize.Level1)
{
    auto interIpcStub = std::make_shared<InterIpcStub>();
    EXPECT_NE(interIpcStub, nullptr);
    auto msg = std::make_shared<BaseMsg>();
    EXPECT_NE(msg, nullptr);
    auto replyMsg = std::make_shared<BaseMsg>();
    EXPECT_NE(replyMsg, nullptr);
    auto ret = interIpcStub->DoIpcCommand(msg, replyMsg);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(InterProcessUnitTest, InterIpcStub_Test_047, testing::ext::TestSize.Level1)
{
    auto interIpcStub = std::make_shared<InterIpcStub>();
    EXPECT_NE(interIpcStub, nullptr);
    uint32_t code = 100;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto ret = interIpcStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(InterProcessUnitTest, InterIpcStub_Test_048, testing::ext::TestSize.Level1)
{
    auto interIpcStub = std::make_shared<InterIpcStub>();
    EXPECT_NE(interIpcStub, nullptr);
    std::string key = "DADADASDSD";
    std::string clientClassName = "WfdSourceImpl";
    auto ret = interIpcStub->GetSubSystemAbility(key, clientClassName);
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcStub_Test_049, testing::ext::TestSize.Level1)
{
    auto interIpcStub = std::make_shared<InterIpcStub>();
    EXPECT_NE(interIpcStub, nullptr);
    std::string key = "DADADASDSD";
    interIpcStub->DelPeerProxy(key);
}

HWTEST_F(InterProcessUnitTest, InterIpcStub_Test_050, testing::ext::TestSize.Level1)
{
    auto interIpcStub = std::make_shared<InterIpcStub>();
    EXPECT_NE(interIpcStub, nullptr);
    std::string key = "DADADASDSD";
    interIpcStub->CreateDeathListener(key);
}

HWTEST_F(InterProcessUnitTest, InterIpcStub_Test_051, testing::ext::TestSize.Level1)
{
    auto interIpcStub = std::make_shared<InterIpcStub>();
    EXPECT_NE(interIpcStub, nullptr);
    MessageParcel data;
    MessageParcel reply;
    auto ret = interIpcStub->DoIpcCommand(data, reply);
    EXPECT_NE(ret, 0);
}

HWTEST_F(InterProcessUnitTest, InterIpcStub_Test_052, testing::ext::TestSize.Level1)
{
    auto interIpcStub = std::make_shared<InterIpcStub>();
    EXPECT_NE(interIpcStub, nullptr);
    MessageParcel data;
    MessageParcel reply;
    auto ret = interIpcStub->GetSystemAbility(data, reply);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(InterProcessUnitTest, InterIpcStub_Test_053, testing::ext::TestSize.Level1)
{
    auto interIpcStub = std::make_shared<InterIpcStub>();
    EXPECT_NE(interIpcStub, nullptr);
    MessageParcel data;
    MessageParcel reply;
    auto ret = interIpcStub->SetListenerObject(data, reply);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(InterProcessUnitTest, InterIpcSubStub_Test_054, testing::ext::TestSize.Level1)
{
    auto interIpcSubStub = std::make_shared<InterIpcSubStub>();
    EXPECT_NE(interIpcSubStub, nullptr);
}

HWTEST_F(InterProcessUnitTest, InterIpcSubStub_Test_055, testing::ext::TestSize.Level1)
{
    auto interIpcSubStub = std::make_shared<InterIpcSubStub>();
    EXPECT_NE(interIpcSubStub, nullptr);
    interIpcSubStub->OnRemoteDied();
}

HWTEST_F(InterProcessUnitTest, InterIpcSubStub_Test_056, testing::ext::TestSize.Level1)
{
    auto interIpcSubStub = std::make_shared<InterIpcSubStub>();
    EXPECT_NE(interIpcSubStub, nullptr);
    std::string key = "fdafdsafdsafd";
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> object = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(object, nullptr);
    auto ret = interIpcSubStub->SetListenerObject(key, object);
    EXPECT_NE(ret, 0);
}

HWTEST_F(InterProcessUnitTest, InterIpcSubStub_Test_057, testing::ext::TestSize.Level1)
{
    auto interIpcSubStub = std::make_shared<InterIpcSubStub>();
    EXPECT_NE(interIpcSubStub, nullptr);
    auto msg = std::make_shared<BaseMsg>();
    EXPECT_NE(msg, nullptr);
    auto replyMsg = std::make_shared<BaseMsg>();
    EXPECT_NE(replyMsg, nullptr);
    auto ret = interIpcSubStub->DoIpcCommand(msg, replyMsg);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(InterProcessUnitTest, IpcMsgAdapter_Test_058, testing::ext::TestSize.Level1)
{
    auto ipcMsgAdapter = std::make_shared<IpcMsgAdapter>();
    EXPECT_NE(ipcMsgAdapter, nullptr);
}

HWTEST_F(InterProcessUnitTest, IpcMsgAdapter_Test_059, testing::ext::TestSize.Level1)
{
    auto ipcMsgAdapter = std::make_shared<IpcMsgAdapter>();
    EXPECT_NE(ipcMsgAdapter, nullptr);
    auto msg = std::make_shared<BaseMsg>();
    EXPECT_NE(msg, nullptr);
    auto replyMsg = std::make_shared<BaseMsg>();
    EXPECT_NE(replyMsg, nullptr);
    auto ret = ipcMsgAdapter->OnRequest(msg, replyMsg);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(InterProcessUnitTest, IpcMsgAdapter_Test_060, testing::ext::TestSize.Level1)
{
    auto ipcMsgAdapter = std::make_shared<IpcMsgAdapter>();
    EXPECT_NE(ipcMsgAdapter, nullptr);
    auto msg = std::make_shared<BaseMsg>();
    EXPECT_NE(msg, nullptr);
    auto replyMsg = std::make_shared<BaseMsg>();
    EXPECT_NE(replyMsg, nullptr);
    auto ret = ipcMsgAdapter->SendRequest(msg, replyMsg);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(InterProcessUnitTest, IpcMsgAdapter_Test_061, testing::ext::TestSize.Level1)
{
    auto ipcMsgAdapter = std::make_shared<IpcMsgAdapter>();
    EXPECT_NE(ipcMsgAdapter, nullptr);
    ipcMsgAdapter->OnRemoteDied();
}

HWTEST_F(InterProcessUnitTest, IpcMsgAdapter_Test_062, testing::ext::TestSize.Level1)
{
    auto ipcMsgAdapter = std::make_shared<IpcMsgAdapter>();
    EXPECT_NE(ipcMsgAdapter, nullptr);
    EXPECT_NE(ipcMsgAdapter, nullptr);
    auto msg = std::make_shared<BaseMsg>();
    EXPECT_NE(msg, nullptr);
    auto replyMsg = std::make_shared<BaseMsg>();
    EXPECT_NE(replyMsg, nullptr);
    ipcMsgAdapter->OnIpcRequest(msg, replyMsg);
}

HWTEST_F(InterProcessUnitTest, IpcMsgAdapter_Test_063, testing::ext::TestSize.Level1)
{
    auto ipcMsgAdapter = std::make_shared<IpcMsgAdapter>();
    EXPECT_NE(ipcMsgAdapter, nullptr);
    sptr<IInterIpc> proxy = nullptr;
    ipcMsgAdapter->SetPeerProxy(proxy);
}

HWTEST_F(InterProcessUnitTest, IpcMsgAdapter_Test_064, testing::ext::TestSize.Level1)
{
    auto ipcMsgAdapter = std::make_shared<IpcMsgAdapter>();
    EXPECT_NE(ipcMsgAdapter, nullptr);
    auto interIpcStub = std::make_shared<InterIpcStub>();
    EXPECT_NE(interIpcStub, nullptr);
    ipcMsgAdapter->SetLocalStub(interIpcStub.get());
}

HWTEST_F(InterProcessUnitTest, IpcMsgAdapter_Test_065, testing::ext::TestSize.Level1)
{
    auto ipcMsgAdapter = std::make_shared<IpcMsgAdapter>();
    EXPECT_NE(ipcMsgAdapter, nullptr);
    std::string key = "sdsdsd";
    ipcMsgAdapter->SetCallingKey(key);
    auto ret = ipcMsgAdapter->GetCallingKey();
    EXPECT_EQ(ret, key);
}

HWTEST_F(InterProcessUnitTest, IpcMsgAdapter_Test_066, testing::ext::TestSize.Level1)
{
    auto ipcMsgAdapter = std::make_shared<IpcMsgAdapter>();
    EXPECT_NE(ipcMsgAdapter, nullptr);
    int32_t pid = 100;
    ipcMsgAdapter->SetPeerPid(pid);
    auto ret = ipcMsgAdapter->GetPeerPid();
    EXPECT_EQ(ret, pid);
}

} // namespace
} // namespace Sharing
} // namespace OHOS
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

#include "scene_unit_test.h"
#include <iostream>
#include <limits>
#include "common/sharing_log.h"
#include "event/event_manager.h"
#include "if_system_ability_manager.h"
#include "interaction/interaction_manager.h"
#include "interaction/ipc_codec/ipc_codec.h"
#include "interaction/ipc_codec/ipc_msg_decoder.h"
#include "interaction/ipc_codec/ipc_msg_encoder.h"
#include "interaction/scene/scene_format.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

using namespace testing::ext;
using namespace OHOS::Sharing;
using namespace OHOS::Sharing::IPC_CODEC;

namespace OHOS {
namespace Sharing {

constexpr uint32_t TEST_SERVICE_SA_ID = 3301;

void SceneUnitTest::SetUpTestCase() {}
void SceneUnitTest::TearDownTestCase() {}
void SceneUnitTest::SetUp() {}
void SceneUnitTest::TearDown() {}
struct TestMessageParcel {
    int32_t value = 0;
    int32_t IpcSerialize(MessageParcel &pIpcMsg)
    {
        return pIpcMsg.WriteInt32(value) ? 0 : 1;
    }
    int32_t IpcDeserialize(MessageParcel &pIpcMsg)
    {
        return pIpcMsg.ReadInt32(value) ? 0 : 1;
    }
};

namespace {
HWTEST_F(SceneUnitTest, InteractionManager_Test_001, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    manager.Init();
}

HWTEST_F(SceneUnitTest, InteractionManager_Test_002, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    const std::string &key = "1";
    uint32_t interactionId = 2;
    manager.AddInteractionKey(key, interactionId);
    manager.RemoveInteraction(interactionId);
}

HWTEST_F(SceneUnitTest, InteractionManager_Test_003, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    const std::string &key = "1";
    uint32_t interactionId = 2;
    manager.AddInteractionKey(key, interactionId);
    manager.DestroyInteraction(interactionId);
}

HWTEST_F(SceneUnitTest, InteractionManager_Test_004, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    Interaction::Ptr ptr = manager.CreateInteraction("WfdSinkScene");
    EXPECT_NE(ptr, nullptr);
}

HWTEST_F(SceneUnitTest, InteractionManager_Test_005, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    Interaction::Ptr ptr = manager.CreateInteraction("WfdSinkScene1");
    EXPECT_EQ(ptr, nullptr);
}

HWTEST_F(SceneUnitTest, InteractionManager_Test_006, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    Interaction::Ptr ptr = manager.CreateInteraction("WfdSourceScene");
    EXPECT_NE(ptr, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<EventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_CONFIGURE_INTERACT;
    int32_t ret = ptr->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, InteractionManager_Test_007, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    Interaction::Ptr ptr = manager.CreateInteraction("WfdSourceScene");
    EXPECT_NE(ptr, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<EventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_INTERACTIONMGR_DESTROY_INTERACTION;
    int32_t ret = ptr->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, InteractionManager_Test_008, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    Interaction::Ptr ptr = manager.CreateInteraction("WfdSourceScene");
    EXPECT_NE(ptr, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<EventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_INTERACTIONMGR_REMOVE_INTERACTION;
    int32_t ret = ptr->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, InteractionManager_Test_009, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    Interaction::Ptr ptr = manager.CreateInteraction("WfdSourceScene");
    EXPECT_NE(ptr, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<EventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EVENT_INTERACTION_DECODER_DIED;
    int32_t ret = ptr->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, InteractionManager_Test_010, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    const std::string &key = "1";
    uint32_t interactionId = 2;
    manager.AddInteractionKey(key, interactionId);
    manager.DelInteractionKey(key);
}

HWTEST_F(SceneUnitTest, InteractionManager_Test_011, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    const std::string &key = "1";
    uint32_t interactionId = 2;
    manager.AddInteractionKey(key, interactionId);
    auto ret = manager.GetInteractionId(key);
    EXPECT_EQ(ret, interactionId);
}

HWTEST_F(SceneUnitTest, InteractionManager_Test_012, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    auto domainMsg = std::make_shared<BaseDomainMsg>();
    EXPECT_NE(domainMsg, nullptr);
    auto ret = manager.SendDomainMsg(domainMsg);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, InteractionManager_Test_013, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    auto domainMsg = std::make_shared<BaseDomainMsg>();
    EXPECT_NE(domainMsg, nullptr);
    auto ret = manager.OnDomainMsg(domainMsg);
    EXPECT_NE(ret, 0);
}

HWTEST_F(SceneUnitTest, InteractionManager_Test_014, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    const std::string &key = "1";
    uint32_t interactionId = 2;
    manager.AddInteractionKey(key, interactionId);
    auto ret = manager.GetInteraction(key);
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(SceneUnitTest, InteractionManager_Test_015, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    Interaction::Ptr ptr = manager.CreateInteraction("WfdSourceScene");
    EXPECT_NE(ptr, nullptr);
    const std::string &key = "1";
    manager.AddInteractionKey(key, ptr->GetId());
    auto ret = manager.GetInteraction(key);
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(SceneUnitTest, InteractionManager_Test_016, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    Interaction::Ptr ptr = manager.CreateInteraction("WfdSourceScene");
    EXPECT_NE(ptr, nullptr);
    auto ret = manager.GetInteraction(ptr->GetId());
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(SceneUnitTest, Interaction_Test_017, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
}

HWTEST_F(SceneUnitTest, Interaction_Test_018, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
}

HWTEST_F(SceneUnitTest, Interaction_Test_019, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    auto ret = interaction->GetIpcAdapter();
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(SceneUnitTest, Interaction_Test_020, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    auto adapter = std::make_shared<IpcMsgAdapter>();
    EXPECT_NE(adapter, nullptr);
    interaction->SetIpcAdapter(adapter);
}

HWTEST_F(SceneUnitTest, Interaction_Test_021, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    auto ret = interaction->GetScene();
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(SceneUnitTest, Interaction_Test_022, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    auto ret = interaction->GetRequestId();
    EXPECT_EQ(ret, 1);
}

HWTEST_F(SceneUnitTest, Interaction_Test_023, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    const std::string &key = "1";
    interaction->SetRpcKey(key);
    auto ret = interaction->GetRpcKey();
    EXPECT_EQ(ret, key);
}

HWTEST_F(SceneUnitTest, Interaction_Test_024, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    interaction->Destroy();
}

HWTEST_F(SceneUnitTest, Interaction_Test_025, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    const std::string &className = "WfdSourceScene";
    auto ret = interaction->CreateScene(className);
    EXPECT_EQ(ret, true);
}

HWTEST_F(SceneUnitTest, Interaction_Test_026, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    const std::string &className = "WfdSourceScene1";
    auto ret = interaction->CreateScene(className);
    EXPECT_EQ(ret, false);
}

HWTEST_F(SceneUnitTest, Interaction_Test_027, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<InteractionEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_INTERACTION_MSG_ERROR;
    event.eventMsg->dstId = 0;
    auto ret = interaction->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_028, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    const std::string &className = "WfdSourceScene";
    auto ret0 = interaction->CreateScene(className);
    EXPECT_EQ(ret0, true);
    SharingEvent event;
    event.eventMsg = std::make_shared<InteractionEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_INTERACTION_MSG_ERROR;
    event.eventMsg->dstId = 0;
    event.eventMsg->errorCode = ERR_NETWORK_ERROR;
    auto ret = interaction->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_029, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    const std::string &className = "WfdSourceScene";
    auto ret0 = interaction->CreateScene(className);
    EXPECT_EQ(ret0, true);
    SharingEvent event;
    event.eventMsg = std::make_shared<InteractionEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_INTERACTION_STATE_AGENT_DESTROYED;
    event.eventMsg->dstId = 0;
    event.eventMsg->errorCode = ERR_NETWORK_ERROR;
    auto ret = interaction->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_030, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    const std::string &className = "WfdSourceScene";
    auto ret0 = interaction->CreateScene(className);
    EXPECT_EQ(ret0, true);
    SharingEvent event;
    event.eventMsg = std::make_shared<InteractionEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_INTERACTION_STATE_CONTEXT_DESTROYED;
    event.eventMsg->dstId = 0;
    event.eventMsg->errorCode = ERR_NETWORK_ERROR;
    auto ret = interaction->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_031, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    const std::string &className = "WfdSourceScene";
    auto ret0 = interaction->CreateScene(className);
    EXPECT_EQ(ret0, true);
    SharingEvent event;
    event.eventMsg = std::make_shared<InteractionEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_INTERACTION_ACCELERATION_DONE;
    event.eventMsg->dstId = 0;
    event.eventMsg->errorCode = ERR_NETWORK_ERROR;
    auto ret = interaction->HandleEvent(event);
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_032, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    const std::string &className = "WfdSourceScene";
    auto ret0 = interaction->CreateScene(className);
    EXPECT_EQ(ret0, true);
    auto domainMsg = std::make_shared<BaseDomainMsg>();
    EXPECT_NE(domainMsg, nullptr);
    interaction->OnDomainMsg(domainMsg);
}

HWTEST_F(SceneUnitTest, Interaction_Test_033, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    const std::string &className = "WfdSourceScene";
    auto ret0 = interaction->CreateScene(className);
    EXPECT_EQ(ret0, true);
    auto domainMsg = std::make_shared<BaseDomainMsg>();
    EXPECT_NE(domainMsg, nullptr);
    interaction->ForwardDomainMsg(domainMsg);
}

HWTEST_F(SceneUnitTest, Interaction_Test_034, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    interaction->ReleaseScene(1);
}

HWTEST_F(SceneUnitTest, Interaction_Test_035, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    interaction->OnSceneNotifyDestroyed(1);
}

HWTEST_F(SceneUnitTest, Interaction_Test_036, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    interaction->OnSceneNotifyDestroyed(1);
}

HWTEST_F(SceneUnitTest, Interaction_Test_037, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<InteractionEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_INTERACTION_ACCELERATION_DONE;
    event.eventMsg->dstId = 0;
    event.eventMsg->errorCode = ERR_NETWORK_ERROR;
    auto ret = interaction->ForwardEvent(1, 1, event, true);
    EXPECT_NE(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_038, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    SharingEvent event;
    event.eventMsg = std::make_shared<InteractionEventMsg>();
    ASSERT_TRUE(event.eventMsg != nullptr);
    event.eventMsg->type = EVENT_INTERACTION_ACCELERATION_DONE;
    event.eventMsg->dstId = 0;
    event.eventMsg->errorCode = ERR_NETWORK_ERROR;
    auto ret = interaction->ForwardEvent(1, 1, event, false);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_039, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    uint32_t contextId = 0;
    auto ret = interaction->CreateContext(contextId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_040, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    uint32_t contextId = 0;
    auto ret = interaction->DestroyContext(contextId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_041, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    uint32_t contextId = 0;
    uint32_t agentId = 0;
    auto ret = interaction->DestroyAgent(contextId, agentId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_042, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    uint32_t contextId = 0;
    uint32_t agentId = 0;
    auto ret = interaction->CreateAgent(contextId, agentId, SRC_AGENT, "WfdSourceSession");
    EXPECT_NE(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_043, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    uint32_t contextId = 0;
    uint32_t agentId = 0;
    auto ret = interaction->Stop(contextId, agentId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_044, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    uint32_t contextId = 0;
    uint32_t agentId = 0;
    auto ret = interaction->Start(contextId, agentId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_045, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    uint32_t contextId = 0;
    uint32_t agentId = 0;
    auto ret = interaction->Pause(contextId, agentId, MEDIA_TYPE_AV);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_046, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    uint32_t contextId = 0;
    uint32_t agentId = 0;
    auto ret = interaction->Resume(contextId, agentId, MEDIA_TYPE_AV);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_047, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    uint32_t contextId = 0;
    uint32_t agentId = 0;
    auto ret = interaction->Play(contextId, agentId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_048, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    uint32_t contextId = 0;
    uint32_t agentId = 0;
    auto ret = interaction->Close(contextId, agentId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_049, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    uint32_t contextId = 0;
    uint32_t agentId = 0;
    float volume = 0;
    auto ret = interaction->SetVolume(contextId, agentId, volume);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_050, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    uint32_t contextId = 0;
    uint32_t agentId = 0;
    uint64_t surfaceId = 0;
    bool keyFrame = false;
    auto ret = interaction->SetKeyPlay(contextId, agentId, surfaceId, keyFrame);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_051, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    uint32_t contextId = 0;
    uint32_t agentId = 0;
    uint64_t surfaceId = 0;
    bool keyFrame = true;
    auto ret = interaction->SetKeyPlay(contextId, agentId, surfaceId, keyFrame);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_052, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    uint32_t contextId = 0;
    uint32_t agentId = 0;
    uint64_t surfaceId = 0;
    bool keyRedirect = true;
    auto ret = interaction->SetKeyRedirect(contextId, agentId, surfaceId, keyRedirect);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_053, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    uint32_t contextId = 0;
    uint32_t agentId = 0;
    uint64_t surfaceId = 0;
    bool keyRedirect = false;
    auto ret = interaction->SetKeyRedirect(contextId, agentId, surfaceId, keyRedirect);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_054, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    uint32_t contextId = 0;
    uint32_t agentId = 0;
    uint64_t surfaceId = 0;
    auto ret = interaction->RemoveSurface(contextId, agentId, surfaceId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_055, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    uint32_t contextId = 0;
    uint32_t agentId = 0;
    sptr<Surface> surfacePtr = nullptr;
    auto ret = interaction->AppendSurface(contextId, agentId, surfacePtr);
    EXPECT_NE(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_056, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    int32_t windowId = 0;
    auto ret = interaction->DestroyWindow(windowId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_057, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    int32_t windowId = 0;
    WindowProperty windowProperty;
    auto ret = interaction->CreateWindow(windowId, windowProperty);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_058, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    int32_t windowId = 0;
    auto ret = interaction->Hide(windowId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_059, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    int32_t windowId = 0;
    auto ret = interaction->Show(windowId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_060, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    int32_t windowId = 0;
    bool isFull = true;
    auto ret = interaction->SetFullScreen(windowId, isFull);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_061, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    int32_t windowId = 0;
    int32_t x = 0;
    int32_t y = 0;
    auto ret = interaction->MoveTo(windowId, x, y);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_062, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    int32_t windowId = 0;
    sptr<Surface> surfacePtr = nullptr;
    auto ret = interaction->GetSurface(windowId, surfacePtr);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_063, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    int32_t windowId = 0;
    int32_t width = 0;
    int32_t height = 0;
    auto ret = interaction->ReSize(windowId, width, height);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, Interaction_Test_064, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto interaction = std::make_shared<Interaction>();
    EXPECT_NE(interaction, nullptr);
    auto eventMsg = std::make_shared<EventMsg>();
    EXPECT_NE(eventMsg, nullptr);
    auto ret = interaction->NotifyEvent(eventMsg);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_065, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto sceneFormat = std::make_shared<SceneFormat>();
    EXPECT_NE(sceneFormat, nullptr);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_066, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto sceneFormat = std::make_shared<SceneFormat>();
    EXPECT_NE(sceneFormat, nullptr);
    sceneFormat->GetFormatMap();
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_067, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto sceneFormat = std::make_shared<SceneFormat>();
    EXPECT_NE(sceneFormat, nullptr);
    const std::string_view &key = "1";
    bool value = false;
    auto ret = sceneFormat->PutBoolValue(key, value);
    EXPECT_EQ(ret, true);
    bool valueNew = false;
    sceneFormat->GetBoolValue(key, valueNew);
    EXPECT_EQ(valueNew, value);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_068, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto sceneFormat = std::make_shared<SceneFormat>();
    EXPECT_NE(sceneFormat, nullptr);
    const std::string_view &key = "1";
    int32_t value = 2;
    auto ret = sceneFormat->PutIntValue(key, value);
    EXPECT_EQ(ret, true);
    int32_t valueNew = 0;
    sceneFormat->GetIntValue(key, valueNew);
    EXPECT_EQ(valueNew, value);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_069, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto sceneFormat = std::make_shared<SceneFormat>();
    EXPECT_NE(sceneFormat, nullptr);
    const std::string_view &key = "1";
    float value = 2.0;
    auto ret = sceneFormat->PutFloatValue(key, value);
    EXPECT_EQ(ret, true);
    float valueNew = 0;
    sceneFormat->GetFloatValue(key, valueNew);
    EXPECT_EQ(valueNew, value);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_070, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto sceneFormat = std::make_shared<SceneFormat>();
    EXPECT_NE(sceneFormat, nullptr);
    const std::string_view &key = "1";
    double value = 2.0;
    auto ret = sceneFormat->PutDoubleValue(key, value);
    EXPECT_EQ(ret, true);
    double valueNew = 0;
    sceneFormat->GetDoubleValue(key, valueNew);
    EXPECT_EQ(valueNew, value);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_071, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto sceneFormat = std::make_shared<SceneFormat>();
    EXPECT_NE(sceneFormat, nullptr);
    const std::string_view &key = "1";
    int64_t value = 2;
    auto ret = sceneFormat->PutInt64Value(key, value);
    EXPECT_EQ(ret, true);
    int64_t valueNew = 0;
    sceneFormat->GetInt64Value(key, valueNew);
    EXPECT_EQ(valueNew, value);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_072, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto sceneFormat = std::make_shared<SceneFormat>();
    EXPECT_NE(sceneFormat, nullptr);
    const std::string_view &key = "1";
    uint16_t value = 2;
    auto ret = sceneFormat->PutUint16Value(key, value);
    EXPECT_EQ(ret, true);
    uint16_t valueNew = 0;
    sceneFormat->GetUint16Value(key, valueNew);
    EXPECT_EQ(valueNew, value);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_073, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto sceneFormat = std::make_shared<SceneFormat>();
    EXPECT_NE(sceneFormat, nullptr);
    const std::string_view &key = "1";
    uint32_t value = 2;
    auto ret = sceneFormat->PutUint32Value(key, value);
    EXPECT_EQ(ret, true);
    uint32_t valueNew = 0;
    sceneFormat->GetUint32Value(key, valueNew);
    EXPECT_EQ(valueNew, value);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_074, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto sceneFormat = std::make_shared<SceneFormat>();
    EXPECT_NE(sceneFormat, nullptr);
    const std::string_view &key = "1";
    uint64_t value = 2;
    auto ret = sceneFormat->PutUint64Value(key, value);
    EXPECT_EQ(ret, true);
    uint64_t valueNew = 0;
    sceneFormat->GetUint64Value(key, valueNew);
    EXPECT_EQ(valueNew, value);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_075, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto sceneFormat = std::make_shared<SceneFormat>();
    EXPECT_NE(sceneFormat, nullptr);
    const std::string_view &key = "1";
    const std::string_view value = "2";
    auto ret = sceneFormat->PutStringValue(key, value);
    EXPECT_EQ(ret, true);
    std::string valueNew;
    sceneFormat->GetStringValue(key, valueNew);
    EXPECT_EQ(valueNew, value);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_076, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto sceneFormat = std::make_shared<SceneFormat>();
    EXPECT_NE(sceneFormat, nullptr);
    const std::string_view &key = "1";
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> value = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(value, nullptr);
    auto ret = sceneFormat->PutObjectValue(key, value);
    EXPECT_EQ(ret, true);
    sptr<IRemoteObject> valueNew;
    sceneFormat->GetObjectValue(key, valueNew);
    EXPECT_EQ(valueNew, value);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_077, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    SceneFormat sceneFormatIn;
    SceneFormat sceneFormatOut;
    const std::string_view key = "1";
    bool valueBool = false;
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> valueObject = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(valueObject, nullptr);
    sceneFormatIn.PutBoolValue(key, valueBool);
    auto ret1 = SceneFormatParcel::Marshalling(parcel, sceneFormatIn);
    EXPECT_EQ(ret1, true);
    auto ret2 = SceneFormatParcel::Unmarshalling(parcel, sceneFormatOut);
    EXPECT_EQ(ret2, true);
    bool valueGetBool = false;
    bool ret = sceneFormatOut.GetBoolValue(key, valueGetBool);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(valueGetBool, valueBool);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_078, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    bool attr = false;
    bool attrGet = true;
    auto ret = IpcEncodeBindAttr(parcel, attr);
    EXPECT_EQ(ret, true);
    auto ret1 = IpcDecodeBindAttr(parcel, attrGet);
    EXPECT_EQ(ret1, true);
    EXPECT_EQ(attr, attrGet);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_079, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    int8_t attr = 1;
    int8_t attrGet = 0;
    auto ret = IpcEncodeBindAttr(parcel, attr);
    EXPECT_EQ(ret, true);
    auto ret1 = IpcDecodeBindAttr(parcel, attrGet);
    EXPECT_EQ(ret1, true);
    EXPECT_EQ(attr, attrGet);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_080, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    int16_t attr = 1;
    int16_t attrGet = 0;
    auto ret = IpcEncodeBindAttr(parcel, attr);
    EXPECT_EQ(ret, true);
    auto ret1 = IpcDecodeBindAttr(parcel, attrGet);
    EXPECT_EQ(ret1, true);
    EXPECT_EQ(attr, attrGet);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_081, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    int32_t attr = 1;
    int32_t attrGet = 0;
    auto ret = IpcEncodeBindAttr(parcel, attr);
    EXPECT_EQ(ret, true);
    auto ret1 = IpcDecodeBindAttr(parcel, attrGet);
    EXPECT_EQ(ret1, true);
    EXPECT_EQ(attr, attrGet);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_082, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    int64_t attr = 1;
    int64_t attrGet = 0;
    auto ret = IpcEncodeBindAttr(parcel, attr);
    EXPECT_EQ(ret, true);
    auto ret1 = IpcDecodeBindAttr(parcel, attrGet);
    EXPECT_EQ(ret1, true);
    EXPECT_EQ(attr, attrGet);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_083, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    uint8_t attr = 1;
    uint8_t attrGet = 0;
    auto ret = IpcEncodeBindAttr(parcel, attr);
    EXPECT_EQ(ret, true);
    auto ret1 = IpcDecodeBindAttr(parcel, attrGet);
    EXPECT_EQ(ret1, true);
    EXPECT_EQ(attr, attrGet);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_084, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    uint16_t attr = 1;
    uint16_t attrGet = 0;
    auto ret = IpcEncodeBindAttr(parcel, attr);
    EXPECT_EQ(ret, true);
    auto ret1 = IpcDecodeBindAttr(parcel, attrGet);
    EXPECT_EQ(ret1, true);
    EXPECT_EQ(attr, attrGet);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_085, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    uint32_t attr = 1;
    uint32_t attrGet = 0;
    auto ret = IpcEncodeBindAttr(parcel, attr);
    EXPECT_EQ(ret, true);
    auto ret1 = IpcDecodeBindAttr(parcel, attrGet);
    EXPECT_EQ(ret1, true);
    EXPECT_EQ(attr, attrGet);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_086, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    uint64_t attr = 1;
    uint64_t attrGet = 0;
    auto ret = IpcEncodeBindAttr(parcel, attr);
    EXPECT_EQ(ret, true);
    auto ret1 = IpcDecodeBindAttr(parcel, attrGet);
    EXPECT_EQ(ret1, true);
    EXPECT_EQ(attr, attrGet);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_087, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    float attr = 1.0;
    float attrGet = 0;
    auto ret = IpcEncodeBindAttr(parcel, attr);
    EXPECT_EQ(ret, true);
    auto ret1 = IpcDecodeBindAttr(parcel, attrGet);
    EXPECT_EQ(ret1, true);
    EXPECT_EQ(attr, attrGet);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_088, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    double attr = 1.0;
    double attrGet = 0;
    auto ret = IpcEncodeBindAttr(parcel, attr);
    EXPECT_EQ(ret, true);
    auto ret1 = IpcDecodeBindAttr(parcel, attrGet);
    EXPECT_EQ(ret1, true);
    EXPECT_EQ(attr, attrGet);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_089, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    std::string attr = "12121";
    std::string attrGet = "";
    auto ret = IpcEncodeBindAttr(parcel, attr);
    EXPECT_EQ(ret, true);
    auto ret1 = IpcDecodeBindAttr(parcel, attrGet);
    EXPECT_EQ(ret1, true);
    EXPECT_EQ(attr, attrGet);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_090, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> attr = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(attr, nullptr);
    sptr<IRemoteObject> attrGet = nullptr;
    auto ret = IpcEncodeBindAttr(parcel, attr);
    EXPECT_EQ(ret, true);
    auto ret1 = IpcDecodeBindAttr(parcel, attrGet);
    EXPECT_EQ(ret1, true);
    EXPECT_EQ(attr, attrGet);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_091, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    TestMessageParcel attr{5};
    TestMessageParcel attrGet;
    auto ret = IpcEncodeBindAttr(parcel, attr);
    EXPECT_EQ(ret, true);
    auto ret1 = IpcDecodeBindAttr(parcel, attrGet);
    EXPECT_EQ(ret1, true);
    EXPECT_EQ(attr.value, attrGet.value);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_092, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    std::vector<TestMessageParcel> attrVec;
    std::vector<TestMessageParcel> attrVecGet;
    TestMessageParcel attr{5};
    attrVec.push_back(attr);
    auto ret = IpcEncodeBindAttr(parcel, attrVec);
    EXPECT_EQ(ret, true);
    auto ret1 = IpcDecodeBindAttr(parcel, attrVecGet);
    EXPECT_EQ(ret1, true);
    EXPECT_EQ(attrVec.size(), attrVecGet.size());
    EXPECT_EQ(attrVec[0].value, attrVecGet[0].value);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_093, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel ipcMsg;
    auto wfdCommonRsp = std::make_shared<WfdCommonRsp>();
    EXPECT_NE(wfdCommonRsp, nullptr);
    wfdCommonRsp->ret = 100;
    auto wfdCommonRspGet = std::make_shared<WfdCommonRsp>();
    EXPECT_NE(wfdCommonRspGet, nullptr);
    auto baseMsg = std::static_pointer_cast<BaseMsg>(wfdCommonRsp);
    auto baseMsgGet = std::static_pointer_cast<BaseMsg>(wfdCommonRspGet);
    auto ret = IpcMsgEncoder::GetInstance().MsgEncode(ipcMsg, baseMsg);
    EXPECT_EQ(ret, 0);
    auto ret1 = IpcMsgDecoder::GetInstance().MsgDecode(baseMsgGet, ipcMsg);
    EXPECT_EQ(ret1, 0);
    auto wfdCommonRspGetNew = std::static_pointer_cast<WfdCommonRsp>(baseMsgGet);
    EXPECT_EQ(wfdCommonRsp->ret, wfdCommonRspGetNew->ret);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_094, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    SceneFormat sceneFormatIn;
    SceneFormat sceneFormatOut;
    const std::string_view key = "2";
    int32_t valueInt32 = 2;

    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> valueObject = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(valueObject, nullptr);

    sceneFormatIn.PutIntValue(key, valueInt32);
    auto ret1 = SceneFormatParcel::Marshalling(parcel, sceneFormatIn);
    EXPECT_EQ(ret1, true);
    auto ret2 = SceneFormatParcel::Unmarshalling(parcel, sceneFormatOut);
    EXPECT_EQ(ret2, true);

    int32_t valueGetInt32 = 0;
    auto ret = sceneFormatOut.GetIntValue(key, valueGetInt32);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(valueGetInt32, valueInt32);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_095, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    SceneFormat sceneFormatIn;
    SceneFormat sceneFormatOut;
    const std::string_view key = "3";
    float valueFloat = 3.14f;

    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> valueObject = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(valueObject, nullptr);

    sceneFormatIn.PutFloatValue(key, valueFloat);
    auto ret1 = SceneFormatParcel::Marshalling(parcel, sceneFormatIn);
    EXPECT_EQ(ret1, true);
    auto ret2 = SceneFormatParcel::Unmarshalling(parcel, sceneFormatOut);
    EXPECT_EQ(ret2, true);

    float valueGetFloat = 0.0f;
    auto ret = sceneFormatOut.GetFloatValue(key, valueGetFloat);
    EXPECT_EQ(ret, true);
    auto isEqual = std::abs(valueGetFloat - valueFloat) < std::numeric_limits<float>::epsilon();
    EXPECT_EQ(isEqual, true);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_096, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    SceneFormat sceneFormatIn;
    SceneFormat sceneFormatOut;
    const std::string_view key = "4";
    double valueDouble = 3.14;

    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> valueObject = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(valueObject, nullptr);

    sceneFormatIn.PutDoubleValue(key, valueDouble);
    auto ret1 = SceneFormatParcel::Marshalling(parcel, sceneFormatIn);
    EXPECT_EQ(ret1, true);
    auto ret2 = SceneFormatParcel::Unmarshalling(parcel, sceneFormatOut);
    EXPECT_EQ(ret2, true);

    double valueGetDouble = 0;
    auto ret = sceneFormatOut.GetDoubleValue(key, valueGetDouble);
    EXPECT_EQ(ret, true);
    auto isEqual = std::abs(valueGetDouble - valueDouble) < std::numeric_limits<float>::epsilon();
    EXPECT_EQ(isEqual, true);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_097, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    SceneFormat sceneFormatIn;
    SceneFormat sceneFormatOut;
    const std::string_view key = "5";
    int64_t valueInt64 = std::numeric_limits<std::int64_t>::max();

    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> valueObject = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(valueObject, nullptr);

    sceneFormatIn.PutInt64Value(key, valueInt64);
    auto ret1 = SceneFormatParcel::Marshalling(parcel, sceneFormatIn);
    EXPECT_EQ(ret1, true);
    auto ret2 = SceneFormatParcel::Unmarshalling(parcel, sceneFormatOut);
    EXPECT_EQ(ret2, true);

    int64_t valueGetInt64 = 0;
    auto ret = sceneFormatOut.GetInt64Value(key, valueGetInt64);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(valueGetInt64, valueInt64);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_098, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    SceneFormat sceneFormatIn;
    SceneFormat sceneFormatOut;
    const std::string_view key = "6";
    uint16_t valueUInt16 = std::numeric_limits<std::uint16_t>::max();

    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> valueObject = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(valueObject, nullptr);

    sceneFormatIn.PutUint16Value(key, valueUInt16);
    auto ret1 = SceneFormatParcel::Marshalling(parcel, sceneFormatIn);
    EXPECT_EQ(ret1, true);
    auto ret2 = SceneFormatParcel::Unmarshalling(parcel, sceneFormatOut);
    EXPECT_EQ(ret2, true);

    uint16_t valueGetUInt16 = 0;
    auto ret = sceneFormatOut.GetUint16Value(key, valueGetUInt16);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(valueGetUInt16, valueUInt16);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_099, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    SceneFormat sceneFormatIn;
    SceneFormat sceneFormatOut;
    const std::string_view key = "7";
    uint32_t valueUint32 = std::numeric_limits<std::uint32_t>::max();

    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> valueObject = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(valueObject, nullptr);

    sceneFormatIn.PutUint32Value(key, valueUint32);
    auto ret1 = SceneFormatParcel::Marshalling(parcel, sceneFormatIn);
    EXPECT_EQ(ret1, true);
    auto ret2 = SceneFormatParcel::Unmarshalling(parcel, sceneFormatOut);
    EXPECT_EQ(ret2, true);

    uint32_t valueGetUint32 = 0;
    auto ret = sceneFormatOut.GetUint32Value(key, valueGetUint32);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(valueGetUint32, valueUint32);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_100, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    SceneFormat sceneFormatIn;
    SceneFormat sceneFormatOut;
    const std::string_view key = "8";
    uint64_t valueUint64 = std::numeric_limits<std::uint64_t>::max();

    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> valueObject = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(valueObject, nullptr);

    sceneFormatIn.PutUint64Value(key, valueUint64);
    auto ret1 = SceneFormatParcel::Marshalling(parcel, sceneFormatIn);
    EXPECT_EQ(ret1, true);
    auto ret2 = SceneFormatParcel::Unmarshalling(parcel, sceneFormatOut);
    EXPECT_EQ(ret2, true);

    uint64_t valueGetUint64 = 0;
    auto ret = sceneFormatOut.GetUint64Value(key, valueGetUint64);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(valueGetUint64, valueUint64);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_101, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    SceneFormat sceneFormatIn;
    SceneFormat sceneFormatOut;
    const std::string_view key = "9";

    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> valueObject = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(valueObject, nullptr);

    sceneFormatIn.PutObjectValue(key, valueObject);
    auto ret1 = SceneFormatParcel::Marshalling(parcel, sceneFormatIn);
    EXPECT_EQ(ret1, true);
    auto ret2 = SceneFormatParcel::Unmarshalling(parcel, sceneFormatOut);
    EXPECT_EQ(ret2, true);

    sptr<IRemoteObject> valueGetObject = nullptr;
    auto ret = sceneFormatOut.GetObjectValue(key, valueGetObject);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(valueGetObject, valueObject);
}

HWTEST_F(SceneUnitTest, SceneFormat_Test_102, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    MessageParcel parcel;
    SceneFormat sceneFormatIn;
    SceneFormat sceneFormatOut;
    const std::string_view key = "10";
    const std::string_view valueString = "string";

    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);
    sptr<IRemoteObject> valueObject = systemAbilityManager->GetSystemAbility(TEST_SERVICE_SA_ID);
    EXPECT_NE(valueObject, nullptr);

    sceneFormatIn.PutStringValue(key, valueString);
    auto ret1 = SceneFormatParcel::Marshalling(parcel, sceneFormatIn);
    EXPECT_EQ(ret1, true);
    auto ret2 = SceneFormatParcel::Unmarshalling(parcel, sceneFormatOut);
    EXPECT_EQ(ret2, true);

    std::string valueGetString = "";
    auto ret = sceneFormatOut.GetStringValue(key, valueGetString);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(valueGetString, valueString);
}

} // namespace
} // namespace Sharing
} // namespace OHOS
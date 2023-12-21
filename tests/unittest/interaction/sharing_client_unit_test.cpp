/*
 * Copyright (c) 2023 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#include <gtest/gtest.h>
#include <iostream>
#include "common/sharing_log.h"
#include "event/event_manager.h"
#include "interaction/interaction_manager.h"
#include "interaction/interprocess/client_factory.h"
#include "interaction/sceneimpl/wfd/wfd_client.h"

using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {
class DemoVoiceClientListener : public IVoiceChatListener {
    void OnError(SharingErrorCode errorCode) final
    {
        SHARING_LOGD("onError");
    }

    void OnInfo(uint32_t id, VoiceChatInfoType infoType, std::string msg) final
    {
        SHARING_LOGD("onInfo: %{public}s", msg.c_str());
    }
};

class UnitTestWfdClientCallback : public IWfdListener {
    virtual void OnWfdError(WfdErrorType errorType, int32_t errorCode)
    {
        SHARING_LOGD("onWfdError");
    }

    virtual void OnWfdInfo(std::string testMsg)
    {
        SHARING_LOGD("onWfdInfo: %{public}s", testMsg.c_str());
    }
};

class SharingClientUnitTest : public testing::Test {};

namespace {
HWTEST_F(SharingClientUnitTest, InteractionManager01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    (void)manager;
}

HWTEST_F(SharingClientUnitTest, InteractionManager02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    Interaction::Ptr ptr = manager.CreateInteraction("WfdScene");
    EXPECT_NE(ptr, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<EventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.type = MAKE_EVENT_TYPE_DESC(COMMON, BASE);
    ptr->HandleEvent(event);
}

HWTEST_F(SharingClientUnitTest, InteractionManager03, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    Interaction::Ptr ptr = manager.CreateInteraction("WfdClient");
    EXPECT_NE(ptr, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<EventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.type = MAKE_EVENT_TYPE_DESC(COMMON, BASE);
    ptr->HandleEvent(event);
}


HWTEST_F(SharingClientUnitTest, InteractionManager05, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    InteractionManager &manager = InteractionManager::GetInstance();
    Interaction::Ptr ptr = manager.CreateInteraction("RtspClient");
    EXPECT_NE(ptr, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<EventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.type = MAKE_EVENT_TYPE_DESC(COMMON, BASE);
    ptr->HandleEvent(event);
}

HWTEST_F(SharingClientUnitTest, WfdClient01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto listener = std::make_shared<UnitTestWfdClientCallback>();
    EXPECT_NE(listener, nullptr);
    auto subClient = ClientFactory::GetInstance().CreateClient("WfdClient", "WfdScene");
    EXPECT_NE(subClient, nullptr);
    std::shared_ptr<WfdClient> wfdClient = std::static_pointer_cast<WfdClient>(subClient);
    EXPECT_NE(wfdClient, nullptr);
    std::string rspMsg;
    wfdClient->SetWfdListener(listener);
    auto ret = wfdClient->StartSink("127.0.0.1", 1234, rspMsg);
    EXPECT_EQ(ret, (uint32_t)88);
    ret = wfdClient->StopSink();
    EXPECT_EQ(ret, (uint32_t)0);
}

} // namespace
} // namespace Sharing
} // namespace OHOS
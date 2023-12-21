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
#include "event/taskpool.h"

using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {
class EventListenerImpl : public EventListener {
public:
    EventListenerImpl() = default;
    int32_t OnEvent(SharingEvent &event) final
    {
        SHARING_LOGD("onEvent");
        return 0;
    }
};

class SharingEventUnitTest : public testing::Test {};

namespace {
HWTEST_F(SharingEventUnitTest, Event_Base_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    std::shared_ptr<EventEmitter> emitter = std::make_shared<EventEmitter>();
    EXPECT_NE(emitter, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<EventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.type = MAKE_EVENT_TYPE_DESC(COMMON, BASE);
    int32_t ret = emitter->SendEvent(event);
    EXPECT_EQ(ret, 0);
    ret = emitter->SendSyncEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingEventUnitTest, Event_Base_02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    std::shared_ptr<EventListenerImpl> listener = std::make_shared<EventListenerImpl>();
    EXPECT_NE(listener, nullptr);
    auto ret = listener->Register();
    EXPECT_EQ(ret, 0);
    ret = listener->UnRegister();
    EXPECT_EQ(ret, 0);
    EventType eventType = MAKE_EVENT_TYPE_DESC(COMMON, BASE);
    ret = listener->AddAcceptEventType(eventType);
    EXPECT_EQ(ret, 0);
    ret = listener->isAcceptType(eventType);
    EXPECT_EQ(ret, true);
    ret = listener->isAcceptType(MAKE_EVENT_TYPE_DESC(CONTEXT, BASE));
    EXPECT_EQ(ret, false);
    auto typeList = listener->GetAcceptTypes();
    EXPECT_EQ(typeList.size(), (uint32_t)1);
    ret = listener->AddAcceptEventType(MAKE_EVENT_TYPE_DESC(AGENT, BASE));
    EXPECT_EQ(ret, 0);
    typeList = listener->GetAcceptTypes();
    EXPECT_EQ(typeList.size(), (uint32_t)2);
    ret = listener->DelAcceptEventType(eventType);
    EXPECT_EQ(ret, 0);
    typeList = listener->GetAcceptTypes();
    EXPECT_EQ(typeList.size(), (uint32_t)1);
    listener->SetListenerClassType(CLASS_TYPE_INTERACTION);
    ret = listener->GetListenerClassType();
    EXPECT_EQ(ret, CLASS_TYPE_INTERACTION);
}

HWTEST_F(SharingEventUnitTest, Event_Base_03, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    EventMsg::Ptr eventMsg = std::make_shared<EventMsg>();
    eventMsg->type = EVENT_AGENT_BASE;
    eventMsg->toMgr = MODULE_CONTEXT;
    EventChecker::CheckEvent(eventMsg);

    eventMsg->type = EVENT_SESSION_BASE;
    eventMsg->toMgr = MODULE_MEDIACHANNEL;
    EventChecker::CheckEvent(eventMsg);

    eventMsg->type = EVENT_SCHEDULER_BASE;
    eventMsg->toMgr = MODULE_MEDIACHANNEL;
    EventChecker::CheckEvent(eventMsg);

    eventMsg->type = EVENT_SCHEDULER_BASE;
    eventMsg->toMgr = MODULE_CONFIGURE;
    EventChecker::CheckEvent(eventMsg);

    eventMsg->type = (EventType)7000;
    eventMsg->toMgr = MODULE_CONFIGURE;
    EventChecker::CheckEvent(eventMsg);

    eventMsg->type = (EventType)7000;
    eventMsg->toMgr = MODULE_INTERACTION;
    EventChecker::CheckEvent(eventMsg);

    EventChecker::CheckEvent(nullptr);
}

HWTEST_F(SharingEventUnitTest, Event_Manager_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    auto ret = EventManager::GetInstance().Init();
    EXPECT_EQ(ret, 0);

    ret = EventManager::GetInstance().StartEventLoop();
    EXPECT_EQ(ret, 0);

    EventManager::GetInstance().StopEventLoop();

    std::shared_ptr<EventListenerImpl> listener = std::make_shared<EventListenerImpl>();
    std::shared_ptr<EventListenerImpl> listener1 = std::make_shared<EventListenerImpl>();
    std::shared_ptr<EventListenerImpl> listener2 = std::make_shared<EventListenerImpl>();

    ret = EventManager::GetInstance().AddListener(listener);
    EXPECT_EQ(ret, 0);
    ret = EventManager::GetInstance().AddListener(listener1);
    EXPECT_EQ(ret, 0);
    ret = EventManager::GetInstance().AddListener(listener2);
    EXPECT_EQ(ret, 0);

    ret = EventManager::GetInstance().DelListener(listener);
    EXPECT_EQ(ret, 0);

    ret = EventManager::GetInstance().DrainAllListeners();
    EXPECT_EQ(ret, 0);

    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<EventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.type = MAKE_EVENT_TYPE_DESC(COMMON, BASE);
    ret = EventManager::GetInstance().PushEvent(event);
    EXPECT_EQ(ret, 0);

    ret = EventManager::GetInstance().PushSyncEvent(event);
    EXPECT_EQ(ret, 0);
}

int32_t testTask()
{
    SHARING_LOGD("testTask");
    sleep(2);
    return 0;
}
HWTEST_F(SharingEventUnitTest, Task_Pool_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("task_Pool_01");
    std::shared_ptr<TaskPool> taskPool = std::make_shared<TaskPool>();
    EXPECT_NE(taskPool, nullptr);

    taskPool->SetMaxTaskNum(1000);
    auto ret = taskPool->GetMaxTaskNum();
    EXPECT_EQ(ret, (uint32_t)1000);

    ret = taskPool->Start(100);
    EXPECT_EQ(ret, (int32_t)0);

    taskPool->SetTimeoutInterval(1000);

    std::packaged_task<TaskPool::BindedTask> bindedTask(testTask);
    taskPool->PushTask(bindedTask);

    ret = taskPool->GetTaskNum();
    EXPECT_EQ(ret, 1);

    taskPool->Stop();
}
} // namespace
} // namespace Sharing
} // namespace OHOS
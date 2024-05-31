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
        SHARING_LOGD("ontestEvent");
        return 0;
    }
};

class SharingEventUnitTest : public testing::Test {};

namespace {

HWTEST_F(SharingEventUnitTest, Event_Base_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    int32_t ret = -1;
    std::shared_ptr<EventEmitter> emitter = std::make_shared<EventEmitter>();
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    EXPECT_NE(emitter, nullptr);

    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_PRODUCER;
    
    ret = emitter->SendEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingEventUnitTest, Event_Base_02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    int32_t ret = -1;
    std::shared_ptr<EventEmitter> emitter = std::make_shared<EventEmitter>();
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    EXPECT_NE(emitter, nullptr);

    event.description = "test";
    event.emitterType = CLASS_TYPE_INTERACTION;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_INTERACTION;
   
    ret = emitter->SendEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingEventUnitTest, Event_Base_03, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<EventEmitter> emitter = std::make_shared<EventEmitter>();
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    EXPECT_NE(emitter, nullptr);
    SharingEvent event;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_SCHEDULER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    
    ret = emitter->SendEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingEventUnitTest, Event_Base_04, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<EventEmitter> emitter = std::make_shared<EventEmitter>();
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    EXPECT_NE(emitter, nullptr);
    SharingEvent event;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONTEXT;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_CONTEXT;
    
    ret = emitter->SendEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingEventUnitTest, Event_Base_05, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<EventEmitter> emitter = std::make_shared<EventEmitter>();
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    EXPECT_NE(emitter, nullptr);
    SharingEvent event;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_AGENT;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_AGENT;
    
    ret = emitter->SendEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingEventUnitTest, Event_Base_06, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<EventEmitter> emitter = std::make_shared<EventEmitter>();
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    EXPECT_NE(emitter, nullptr);
    SharingEvent event;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_SESSION;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SESSION;
    
    ret = emitter->SendEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingEventUnitTest, Event_Manager_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    EventManager::GetInstance().Init();
    EventManager::GetInstance().StartEventLoop();
    std::shared_ptr<EventListenerImpl> listener = std::make_shared<EventListenerImpl>();
    std::shared_ptr<EventListenerImpl> listener1 = std::make_shared<EventListenerImpl>();
    std::shared_ptr<EventListenerImpl> listener2 = std::make_shared<EventListenerImpl>();

    auto ret = EventManager::GetInstance().AddListener(listener);
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
    ret = EventManager::GetInstance().PushEvent(event);
    EXPECT_EQ(ret, 0);

    ret = EventManager::GetInstance().PushSyncEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingEventUnitTest, Event_Base_07, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<EventEmitter> emitter = std::make_shared<EventEmitter>();
    EXPECT_NE(emitter, nullptr);
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    std::shared_ptr<EventListenerImpl> listener = std::make_shared<EventListenerImpl>();
    SharingEvent event;

    listener->SetListenerClassType(CLASS_TYPE_INTERACTION);
    listener->Register();

    event.description = "test";
    event.emitterType = CLASS_TYPE_INTERACTION;
    contextEventMsgSptr->type = EVENT_CONTEXT_BASE;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_INTERACTION;
    
    ret = emitter->SendSyncEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingEventUnitTest, Event_Base_08, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<EventEmitter> emitter = std::make_shared<EventEmitter>();
    EXPECT_NE(emitter, nullptr);
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    std::shared_ptr<EventListenerImpl> listener = std::make_shared<EventListenerImpl>();
    SharingEvent event;
    
    listener->SetListenerClassType(CLASS_TYPE_SCHEDULER);
    listener->Register();

    event.description = "test";
    event.emitterType = CLASS_TYPE_SCHEDULER;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    
    ret = emitter->SendSyncEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingEventUnitTest, Event_Base_09, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<EventEmitter> emitter = std::make_shared<EventEmitter>();
    EXPECT_NE(emitter, nullptr);
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    std::shared_ptr<EventListenerImpl> listener = std::make_shared<EventListenerImpl>();
    SharingEvent event;
    
    listener->SetListenerClassType(CLASS_TYPE_CONTEXT);
    listener->Register();

    event.description = "test";
    event.emitterType = CLASS_TYPE_CONTEXT;
    contextEventMsgSptr->type = EVENT_CONTEXT_BASE;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_CONTEXT;
    
    ret = emitter->SendSyncEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingEventUnitTest, Event_Base_10, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<EventEmitter> emitter = std::make_shared<EventEmitter>();
    EXPECT_NE(emitter, nullptr);
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    std::shared_ptr<EventListenerImpl> listener = std::make_shared<EventListenerImpl>();
    SharingEvent event;
    
    listener->SetListenerClassType(CLASS_TYPE_AGENT);
    listener->Register();

    event.description = "test";
    event.emitterType = CLASS_TYPE_AGENT;
    contextEventMsgSptr->type = EVENT_CONTEXT_BASE;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_AGENT;
    ret = emitter->SendSyncEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingEventUnitTest, Event_Base_11, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<EventEmitter> emitter = std::make_shared<EventEmitter>();
    EXPECT_NE(emitter, nullptr);
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    std::shared_ptr<EventListenerImpl> listener = std::make_shared<EventListenerImpl>();
    SharingEvent event;
    
    listener->SetListenerClassType(CLASS_TYPE_SESSION);
    listener->Register();
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_SESSION;
    contextEventMsgSptr->type = EVENT_CONTEXT_BASE;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_SESSION;
    
    ret = emitter->SendSyncEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingEventUnitTest, Event_Base_12, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<EventEmitter> emitter = std::make_shared<EventEmitter>();
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    std::shared_ptr<EventListenerImpl> listener = std::make_shared<EventListenerImpl>();
    SharingEvent event;

    listener->SetListenerClassType(CLASS_TYPE_CONSUMER);
    listener->Register();
    EXPECT_NE(emitter, nullptr);

    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    contextEventMsgSptr->type = EVENT_CONTEXT_BASE;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_CONSUMER;
    
    ret = emitter->SendSyncEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingEventUnitTest, Event_Base_13, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<EventEmitter> emitter = std::make_shared<EventEmitter>();
    EXPECT_NE(emitter, nullptr);
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    std::shared_ptr<EventListenerImpl> listener = std::make_shared<EventListenerImpl>();
    SharingEvent event;
    
    listener->SetListenerClassType(CLASS_TYPE_PRODUCER);
    listener->Register();

    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    contextEventMsgSptr->type = EVENT_CONTEXT_BASE;
    event.eventMsg = contextEventMsgSptr;
    event.listenerType = CLASS_TYPE_PRODUCER;
    
    ret = emitter->SendSyncEvent(event);
    EXPECT_EQ(ret, 0);
    EventManager::GetInstance().StartEventLoop();
}

HWTEST_F(SharingEventUnitTest, Event_Base_15, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    EventMsg::Ptr eventMsg = std::make_shared<EventMsg>();
    
    eventMsg->type = EVENT_AGENT_BASE;
    eventMsg->toMgr = MODULE_CONTEXT;
    
    EventChecker::CheckEvent(eventMsg);
}

HWTEST_F(SharingEventUnitTest, Event_Base_16, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    EventMsg::Ptr eventMsg = std::make_shared<EventMsg>();
    
    eventMsg->type = EVENT_SESSION_BASE;
    eventMsg->toMgr = MODULE_MEDIACHANNEL;
    
    EventChecker::CheckEvent(eventMsg);
}

HWTEST_F(SharingEventUnitTest, Event_Base_17, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    EventMsg::Ptr eventMsg = std::make_shared<EventMsg>();
    
    eventMsg->type = EVENT_SCHEDULER_BASE;
    eventMsg->toMgr = MODULE_MEDIACHANNEL;
    
    EventChecker::CheckEvent(eventMsg);
}

HWTEST_F(SharingEventUnitTest, Event_Base_18, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    EventMsg::Ptr eventMsg = std::make_shared<EventMsg>();
    
    eventMsg->type = EVENT_SCHEDULER_BASE;
    eventMsg->toMgr = MODULE_CONFIGURE;
    
    EventChecker::CheckEvent(eventMsg);
}

HWTEST_F(SharingEventUnitTest, Event_Base_19, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    EventMsg::Ptr eventMsg = std::make_shared<EventMsg>();
    
    eventMsg->type = (EventType)7000;
    eventMsg->toMgr = MODULE_CONFIGURE;
    
    EventChecker::CheckEvent(eventMsg);
}

HWTEST_F(SharingEventUnitTest, Event_Base_20, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    EventMsg::Ptr eventMsg = std::make_shared<EventMsg>();
    
    eventMsg->type = (EventType)7000;
    eventMsg->toMgr = MODULE_INTERACTION;
    
    EventChecker::CheckEvent(eventMsg);
}

HWTEST_F(SharingEventUnitTest, Event_Base_21, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    EventChecker::CheckEvent(nullptr);
}

HWTEST_F(SharingEventUnitTest, Event_Base_22, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<EventEmitter> emitter = std::make_shared<EventEmitter>();
    EXPECT_NE(emitter, nullptr);
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    std::shared_ptr<EventListenerImpl> listener = std::make_shared<EventListenerImpl>();
    SharingEvent event;

    ret = listener->Register();
    EXPECT_EQ(ret, 0);
    ret = listener->UnRegister();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingEventUnitTest, Event_Base_23, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<EventEmitter> emitter = std::make_shared<EventEmitter>();
    EXPECT_NE(emitter, nullptr);
    std::shared_ptr<ContextEventMsg> contextEventMsgSptr = std::make_shared<ContextEventMsg>();
    std::shared_ptr<EventListenerImpl> listener = std::make_shared<EventListenerImpl>();
    SharingEvent event;

    EventManager::GetInstance().DrainAllListeners();
    ret = listener->Register();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingEventUnitTest, Event_Base_24, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    bool ret = false;
    std::shared_ptr<EventListenerImpl> listener = std::make_shared<EventListenerImpl>();
    SharingEvent event;

    ret = listener->IsAcceptType(EVENT_COMMON_BASE);
    EXPECT_EQ(ret, false);
}

HWTEST_F(SharingEventUnitTest, Event_Base_25, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    bool tag = false;
    std::shared_ptr<EventListenerImpl> listener = std::make_shared<EventListenerImpl>();
    SharingEvent event;

    ret = listener->AddAcceptEventType(EVENT_COMMON_BASE);
    EXPECT_EQ(ret, 0);
    tag = listener->IsAcceptType(EVENT_COMMON_BASE);
    EXPECT_EQ(tag, true);
}

HWTEST_F(SharingEventUnitTest, Event_Base_26, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    int32_t ret = -1;
    std::shared_ptr<EventListenerImpl> listener = std::make_shared<EventListenerImpl>();
    SharingEvent event;

    ret = listener->AddAcceptEventType(EVENT_COMMON_BASE);
    EXPECT_EQ(ret, 0);
    ret = listener->DelAcceptEventType(EVENT_COMMON_BASE);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingEventUnitTest, Event_Base_27, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    std::shared_ptr<EventListenerImpl> listener = std::make_shared<EventListenerImpl>();
    SharingEvent event;

    listener->GetAcceptTypes();
}

HWTEST_F(SharingEventUnitTest, Event_Base_28, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    std::shared_ptr<EventListenerImpl> listener = std::make_shared<EventListenerImpl>();
    SharingEvent event;

    listener->GetListenerClassType();
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

    taskPool->Stop();
    std::packaged_task<TaskPool::BindedTask> bindedTask(testTask);
    taskPool->PushTask(bindedTask);

    ret = taskPool->GetTaskNum();
    EXPECT_EQ(ret, 1);
}

} // namespace
} // namespace Sharing
} // namespace OHOS
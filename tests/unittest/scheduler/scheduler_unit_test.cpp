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
#include "common/reflect_registration.h"
#include "common/sharing_log.h"
#include "scheduler/scheduler.h"

using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

class SharingSchedulerUnitTest : public testing::Test {};

namespace {
HWTEST_F(SharingSchedulerUnitTest, Scheduler_Manager_01, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    int32_t ret = -1;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->toMgr = MODULE_CONTEXT;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    
    ret = Scheduler::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingSchedulerUnitTest, Scheduler_Manager_02, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    int32_t ret = -1;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->toMgr = MODULE_CONTEXT;
    
    event.eventMsg->type = EVENT_CONTEXTMGR_CREATE;
    ret = Scheduler::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingSchedulerUnitTest, Scheduler_Manager_03, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    int32_t ret = -1;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->toMgr = MODULE_CONTEXT;
    event.eventMsg->type = EVENT_CONTEXTMGR_AGENT_CREATE;
    
    ret = Scheduler::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingSchedulerUnitTest, Scheduler_Manager_04, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    int32_t ret = -1;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->toMgr = MODULE_CONTEXT;
    event.eventMsg->type = EVENT_CONTEXT_AGENT_CREATE;
    event.eventMsg->type = EVENT_CONTEXTMGR_STATE_CHANNEL_DESTROY;
    
    ret = Scheduler::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingSchedulerUnitTest, Scheduler_Manager_05, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    int32_t ret = -1;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->toMgr = MODULE_CONTEXT;
    event.eventMsg->type = EVENT_CONTEXT_STATE_AGENT_DESTROY;
    
    ret = Scheduler::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingSchedulerUnitTest, Scheduler_Manager_06, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    int32_t ret = -1;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->toMgr = MODULE_MEDIACHANNEL;
    event.eventMsg->type = EVENT_CONFIGURE_MEDIACHANNEL;
    
    ret = Scheduler::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingSchedulerUnitTest, Scheduler_Manager_07, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    int32_t ret = -1;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->toMgr = MODULE_MEDIACHANNEL;
    
    ret = Scheduler::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingSchedulerUnitTest, Scheduler_Manager_08, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    int32_t ret = -1;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->toMgr = MODULE_INTERACTION;
    event.eventMsg->type = EVENT_CONFIGURE_INTERACT;
    
    ret = Scheduler::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(SharingSchedulerUnitTest, Scheduler_Manager_9, Function | SmallTest | Level2)
{
    SHARING_LOGD("trace");
    SharingEvent event;
    int32_t ret = -1;
    
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ContextEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->toMgr = MODULE_DATACENTER;
    event.eventMsg->type = EVENT_INTERACTIONMGR_REMOVE_INTERACTION;
    
    ret = Scheduler::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, -1);
}

} // namespace
} // namespace Sharing
} // namespace OHOS
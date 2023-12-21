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

#include <config.h>
#include <gtest/gtest.h>
#include "common/sharing_log.h"
#include "context/context_manager.h"
using namespace testing::ext;

namespace OHOS {
namespace Sharing {
class ContextUnitTest : public testing::Test {};

namespace {
HWTEST_F(ContextUnitTest, TestContext01, Function | SmallTest | Level2)
{
    SharingEvent event;
    auto eventMsg = std::make_shared<ConfigEventMsg>(nullptr, ModuleType::MODULE_CONTEXT);
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_CONFIGURE_CONTEXT;
    event.eventMsg->dstId = 0;
    auto ret = ContextManager::GetInstance().HandleEvent(event);
    ASSERT_TRUE(ret == -1);
}

HWTEST_F(ContextUnitTest, TestContext01_1, Function | SmallTest | Level2)
{
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_CONTEXTMGR_CREATE;
    event.eventMsg->dstId = 0;
    auto ret = ContextManager::GetInstance().HandleEvent(event);
    ASSERT_TRUE(ret != -1);
}

HWTEST_F(ContextUnitTest, TestContext01_2, Function | SmallTest | Level2)
{
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_CONTEXTMGR_DESTROY;
    event.eventMsg->dstId = 0;
    auto ret = ContextManager::GetInstance().HandleEvent(event);
    ASSERT_TRUE(ret == -1);
}

HWTEST_F(ContextUnitTest, TestContext01_3, Function | SmallTest | Level2)
{
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_CONTEXTMGR_AGENT_CREATE;
    event.eventMsg->dstId = 0;

    auto ret = ContextManager::GetInstance().HandleEvent(event);
    ASSERT_TRUE(ret == -1);
}

HWTEST_F(ContextUnitTest, TestContext01_4, Function | SmallTest | Level2)
{
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_CONTEXTMGR_STATE_CHANNEL_DESTROY;
    event.eventMsg->dstId = 0;

    auto ret = ContextManager::GetInstance().HandleEvent(event);
    ASSERT_TRUE(ret == -1);
}

HWTEST_F(ContextUnitTest, TestContext01_5, Function | SmallTest | Level2)
{
    SharingEvent event;
    auto eventMsg = std::make_shared<ContextEventMsg>();
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_MEDIA_PUBLISH_STOP;
    event.eventMsg->dstId = 0;

    auto ret = ContextManager::GetInstance().HandleEvent(event);
    ASSERT_TRUE(ret == -1);
}

HWTEST_F(ContextUnitTest, TestContext01_6, Function | SmallTest | Level2)
{
    Config::GetInstance().Init();
    usleep(1000000); // 1000000 for 1s
    SharingDataGroupByModule::Ptr values = nullptr;
    auto ret = Config::GetInstance().GetConfig("context", values);
    ASSERT_TRUE(ret == 0);

    auto retSinkLimit = ContextManager::GetInstance().GetAgentSinkLimit();
    ASSERT_TRUE(retSinkLimit == 0);

    auto retSrcLimit = ContextManager::GetInstance().GetAgentSrcLimit();
    ASSERT_TRUE(retSrcLimit == 0);

    SharingEvent event;
    auto eventMsg = std::make_shared<ConfigEventMsg>(values, ModuleType::MODULE_CONTEXT);
    event.eventMsg = std::move(eventMsg);
    event.eventMsg->type = EVENT_CONFIGURE_CONTEXT;
    event.eventMsg->dstId = 0;

    auto ret1 = ContextManager::GetInstance().HandleEvent(event);
    ASSERT_TRUE(ret1 == -1);

    auto ret2 = ContextManager::GetInstance().GetAgentSinkLimit();
    ASSERT_TRUE(ret2 == 10);

    auto ret3 = ContextManager::GetInstance().GetAgentSrcLimit();
    ASSERT_TRUE(ret3 == 10);
}

HWTEST_F(ContextUnitTest, TestContext01_6_1, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.eventMsg = nullptr;

    auto ret1 = ContextManager::GetInstance().HandleEvent(event);
    ASSERT_TRUE(ret1 == -1);
}

HWTEST_F(ContextUnitTest, TestContext02, Function | SmallTest | Level2)
{
    auto ret = ContextManager::GetInstance().HandleContextCreate();
    ASSERT_TRUE(ret != 0);
}

HWTEST_F(ContextUnitTest, TestContext03, Function | SmallTest | Level2)
{
    auto ret1 = ContextManager::GetInstance().HandleContextCreate();
    ASSERT_TRUE(ret1 != 0);
    ContextManager::GetInstance().DestroyContext(ret1);
}

HWTEST_F(ContextUnitTest, TestContext04, Function | SmallTest | Level2)
{
    auto ret1 = ContextManager::GetInstance().HandleContextCreate();
    ASSERT_TRUE(ret1 != 0);
    ContextManager::GetInstance().RemoveContext(ret1);
}

HWTEST_F(ContextUnitTest, TestContext05, Function | SmallTest | Level2)
{
    auto ret1 = ContextManager::GetInstance().HandleContextCreate();
    ASSERT_TRUE(ret1 != 0);
    Context::Ptr ptr = ContextManager::GetInstance().GetContextById(ret1);
    ASSERT_TRUE(ptr != nullptr);
}

HWTEST_F(ContextUnitTest, TestContext06, Function | SmallTest | Level2)
{
    auto ret1 = ContextManager::GetInstance().HandleContextCreate();
    ASSERT_TRUE(ret1 != 0);
    Context::Ptr ptr = ContextManager::GetInstance().GetContextById(ret1 + 1);
    ASSERT_TRUE(ptr == nullptr);
}

HWTEST_F(ContextUnitTest, TestContext07, Function | SmallTest | Level2)
{
    auto ret = ContextManager::GetInstance().GetAgentSinkLimit();
    ASSERT_TRUE(ret != 0);
}

HWTEST_F(ContextUnitTest, TestContext08, Function | SmallTest | Level2)
{
    auto ret = ContextManager::GetInstance().GetAgentSrcLimit();
    ASSERT_TRUE(ret != 0);
}

} // namespace
} // namespace Sharing
} // namespace OHOS
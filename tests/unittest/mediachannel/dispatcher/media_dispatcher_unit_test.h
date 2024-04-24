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

#ifndef OHOS_SHARING_MEDIA_DISPATCHER_UNIT_TEST_H
#define OHOS_SHARING_MEDIA_DISPATCHER_UNIT_TEST_H

#include "common/reflect_registration.h"
#include "gtest/gtest.h"
#include "mediachannel/base_consumer.h"
#include "mediachannel/base_producer.h"
namespace OHOS {
namespace Sharing {
class MediaDispatcherUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

class TestConsumer final : public BaseConsumer,
                           public std::enable_shared_from_this<TestConsumer> {
public:
    TestConsumer() {}
    ~TestConsumer() {}
    void UpdateOperation(ProsumerStatusMsg::Ptr &statusMsg)
    {
        (void)statusMsg;
    }
    int32_t Release()
    {
        return 0;
    }
    int32_t HandleEvent(SharingEvent &event)
    {
        (void)event;
        return 0;
    }
};

class TestProducer : public BaseProducer,
                     public std::enable_shared_from_this<TestProducer> {
public:
    TestProducer() {}
    ~TestProducer() {}
    void UpdateOperation(ProsumerStatusMsg::Ptr &statusMsg)
    {
        (void)statusMsg;
    }
    int32_t Release()
    {
        return 0;
    }
    void PublishOneFrame(const MediaData::Ptr &buff)
    {
        (void)buff;
    }
    int32_t HandleEvent(SharingEvent &event)
    {
        (void)event;
        return 0;
    }
};
REGISTER_CLASS_REFLECTOR(TestConsumer);
REGISTER_CLASS_REFLECTOR(TestProducer);
} // namespace Sharing
} // namespace OHOS
#endif
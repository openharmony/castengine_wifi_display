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

#include "media_channel_unit_test.h"
#include <iostream>
#include "../dispatcher/media_dispatcher_unit_test.h"
#include "mediachannel/channel_manager.h"
#include "mediachannel/media_channel.h"
using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

void MediaChannelUnitTest::SetUpTestCase() {}
void MediaChannelUnitTest::TearDownTestCase() {}
void MediaChannelUnitTest::SetUp() {}
void MediaChannelUnitTest::TearDown() {}

class MediaChannelListener : public IMediaChannelListener {
public:
    MediaChannelListener() = default;
    void OnMediaChannelNotify(MediaChannelNotifyStatus status) final
    {
        (void)status;
    }
};

namespace {
HWTEST_F(MediaChannelUnitTest, HandleMediaChannelCreate_001, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelCreate_002, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    mediaChannel->SetContextId(1);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelCreate_003, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    auto dispatcher = mediaChannel->GetDispatcher();
    EXPECT_NE(dispatcher, nullptr);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelCreate_004, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    auto agentID = mediaChannel->GetSinkAgentId();
    EXPECT_EQ(agentID, 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelCreate_005, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    std::string className = "TestProducer";
    auto ret1 = mediaChannel->CreateProducer(className);
    EXPECT_NE(ret1, 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelCreate_006, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    std::string className = "TestProducer1";
    auto ret1 = mediaChannel->CreateProducer(className);
    ASSERT_TRUE(ret1 == INVALID_ID);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelCreate_007, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    auto ret1 = mediaChannel->Release();
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelCreate_008, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    std::string className = "TestConsumer";
    auto ret1 = mediaChannel->CreateConsumer(className);
    EXPECT_EQ(ret1, ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelCreate_009, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    std::string className = "TestConsumer1";
    auto ret1 = mediaChannel->CreateConsumer(className);
    ASSERT_TRUE(ret1 != ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelCreate_010, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    mediaChannel->SendAgentEvent(statusMsg, EVENT_MEDIA_CONSUMER_CREATE);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelCreate_011, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    auto listener = std::make_shared<MediaChannelListener>();
    EXPECT_NE(listener, nullptr);
    std::weak_ptr<MediaChannelListener> weakListener(listener);
    mediaChannel->SetMediaChannelListener(weakListener);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_012, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_CONFIGURE_MEDIACHANNEL;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_013, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_014, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_DESTROY;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_015, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CONSUMER_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_016, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    int32_t ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    uint32_t srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg->type = EventType::EVENT_MEDIA_CONSUMER_CREATE;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_017, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    int32_t ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    uint32_t srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg->type = EventType::EVENT_MEDIA_CONSUMER_DESTROY;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_018, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg->type = EventType::EVENT_MEDIA_CONSUMER_START;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_019, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg->type = EventType::EVENT_MEDIA_CONSUMER_STOP;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_020, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg->type = EventType::EVENT_MEDIA_CONSUMER_PAUSE;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_021, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg->type = EventType::EVENT_MEDIA_CONSUMER_RESUME;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_022, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    int32_t ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    uint32_t srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg->type = EventType::EVENT_MEDIA_PRODUCER_CREATE;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_023, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    int32_t ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    uint32_t srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg->type = EventType::EVENT_MEDIA_PRODUCER_DESTROY;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_024, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg->type = EventType::EVENT_MEDIA_PRODUCER_START;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_025, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    int32_t ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    uint32_t srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg->type = EventType::EVENT_MEDIA_PRODUCER_STOP;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_026, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg->type = EventType::EVENT_MEDIA_PRODUCER_PAUSE;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_027, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg->type = EventType::EVENT_MEDIA_PRODUCER_RESUME;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_028, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg = std::make_shared<ChannelAppendSurfaceEventMsg>();
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_APPENDSURFACE;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_029, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg = std::make_shared<ChannelRemoveSurfaceEventMsg>();
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_REMOVESURFACE;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_030, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg = std::make_shared<ChannelSetSceneTypeEventMsg>();
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_SETSCENETYPE;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_031, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg = std::make_shared<ChannelSetVolumeEventMsg>();
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_SETVOLUME;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_032, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_START;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_033, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_034, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg = std::make_shared<ChannelSetKeyRedirectEventMsg>();
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_KEY_REDIRECT;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, HandleMediaChannelHandleEvent_035, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleEvent(event);
    EXPECT_EQ(ret, 0);
    auto srcID = event.eventMsg->srcId;
    ASSERT_TRUE(srcID > 0);
    auto mediaChannel = ChannelManager::GetInstance().GetMediaChannel(srcID);
    EXPECT_NE(mediaChannel, nullptr);
    event.eventMsg->type = EventType::EVENT_AGENT_STATE_WRITE_WARNING;
    auto ret1 = mediaChannel->HandleEvent(event);
    ASSERT_TRUE(ret1 == 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_036, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleMediaChannelCreate(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_037, Function | SmallTest | Level2)
{
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_CREATE;
    ChannelManager::GetInstance().Init();
    auto ret = ChannelManager::GetInstance().HandleMediaChannelDestroy(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_038, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    mediaChannel->OnWriteTimeout();
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_039, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto contextId = 1;
    mediaChannel->SetContextId(contextId);
    EXPECT_EQ(contextId, mediaChannel->srcContextId_);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_040, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto ret = mediaChannel->GetDispatcher();
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_041, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto ret = mediaChannel->GetSinkAgentId();
    EXPECT_EQ(ret, INVALID_ID);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_042, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    std::string consumer = "TestConsumer";
    auto error = mediaChannel->CreateConsumer(consumer);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
    auto ret = mediaChannel->GetSinkAgentId();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_043, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    std::string consumer = "TestConsumer1";
    auto error = mediaChannel->CreateConsumer(consumer);
    EXPECT_NE(error, SharingErrorCode::ERR_OK);
    auto ret = mediaChannel->GetSinkAgentId();
    EXPECT_EQ(ret, INVALID_ID);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_044, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    std::string producer = "TestProducer";
    auto ret = mediaChannel->CreateProducer(producer);
    EXPECT_NE(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_045, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    std::string producer = "TestProducer1";
    auto ret = mediaChannel->CreateProducer(producer);
    EXPECT_EQ(ret, INVALID_ID);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_046, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto ret = mediaChannel->Release();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_047, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CONSUMER_CREATE;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_048, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CONSUMER_DESTROY;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_049, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CONSUMER_START;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_050, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CONSUMER_STOP;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_051, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CONSUMER_PAUSE;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_052, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CONSUMER_RESUME;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_053, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PRODUCER_CREATE;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_054, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PRODUCER_DESTROY;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_055, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PRODUCER_START;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_056, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PRODUCER_STOP;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_057, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PRODUCER_PAUSE;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_058, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_PRODUCER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PRODUCER_RESUME;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_059, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelAppendSurfaceEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_APPENDSURFACE;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_060, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelRemoveSurfaceEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_REMOVESURFACE;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_061, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelSetSceneTypeEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_SETSCENETYPE;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_062, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelSetVolumeEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_SETVOLUME;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_063, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_START;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_064, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_065, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelSetKeyRedirectEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_CHANNEL_KEY_REDIRECT;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_066, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_INTERACTIONMGR_DESTROY_INTERACTION;
    auto ret = mediaChannel->HandleEvent(event);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_067, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = CONNTROLLER_NOTIFY_ACCELERATION;
    mediaChannel->OnMediaControllerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_068, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = CONNTROLLER_NOTIFY_DECODER_DIED;
    mediaChannel->OnMediaControllerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_069, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = CONNTROLLER_NOTIFY_KEYMOD_START;
    mediaChannel->OnMediaControllerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_070, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = CONNTROLLER_NOTIFY_KEYMOD_STOP;
    mediaChannel->OnMediaControllerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_071, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = 100;
    mediaChannel->OnMediaControllerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_072, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = 100;
    mediaChannel->OnMediaControllerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_073, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = PROSUMER_NOTIFY_INIT_SUCCESS;
    mediaChannel->OnProducerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_074, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = PROSUMER_NOTIFY_START_SUCCESS;
    mediaChannel->OnProducerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_075, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = PROSUMER_NOTIFY_STOP_SUCCESS;
    mediaChannel->OnProducerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_076, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = PROSUMER_NOTIFY_PAUSE_SUCCESS;
    mediaChannel->OnProducerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_077, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = PROSUMER_NOTIFY_RESUME_SUCCESS;
    mediaChannel->OnProducerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_078, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = PROSUMER_NOTIFY_DESTROY_SUCCESS;
    mediaChannel->OnProducerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_079, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = PROSUMER_NOTIFY_ERROR;
    mediaChannel->OnProducerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_080, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = PROSUMER_NOTIFY_PRIVATE_EVENT;
    mediaChannel->OnProducerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_081, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = 100;
    mediaChannel->OnProducerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_082, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = 100;
    mediaChannel->OnProducerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_083, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = PROSUMER_NOTIFY_INIT_SUCCESS;
    mediaChannel->OnConsumerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_084, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = PROSUMER_NOTIFY_START_SUCCESS;
    std::string className = "TestConsumer";
    auto ret = mediaChannel->CreateConsumer(className);
    ASSERT_TRUE(ret == ERR_OK);
    mediaChannel->OnConsumerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_085, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = PROSUMER_NOTIFY_STOP_SUCCESS;
    mediaChannel->OnConsumerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_086, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = PROSUMER_NOTIFY_PAUSE_SUCCESS;
    mediaChannel->OnConsumerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_087, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = PROSUMER_NOTIFY_RESUME_SUCCESS;
    mediaChannel->OnConsumerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_088, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = PROSUMER_NOTIFY_DESTROY_SUCCESS;
    mediaChannel->OnConsumerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_089, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = PROSUMER_NOTIFY_ERROR;
    mediaChannel->OnConsumerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_090, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = PROSUMER_NOTIFY_PRIVATE_EVENT;
    mediaChannel->OnConsumerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_091, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = 100;
    mediaChannel->OnConsumerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_092, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = 100;
    mediaChannel->OnConsumerNotify(statusMsg);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_093, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    EXPECT_NE(statusMsg, nullptr);
    statusMsg->eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(statusMsg->eventMsg, nullptr);
    statusMsg->errorCode = ERR_PROSUMER_VIDEO_CAPTURE;
    statusMsg->status = ERR_PROSUMER_VIDEO_CAPTURE;
    mediaChannel->SendAgentEvent(statusMsg, EVENT_MEDIA_PRODUCER_CREATE);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_094, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto mediaChannelListener = std::make_shared<MediaChannelListener>();
    EXPECT_NE(mediaChannelListener, nullptr);
    mediaChannel->SetMediaChannelListener(mediaChannelListener);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_095, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto mediaController = std::make_shared<MediaController>(1);
    EXPECT_NE(mediaController, nullptr);
    std::string className = "TestConsumer";
    auto ret = mediaChannel->CreateConsumer(className);
    ASSERT_TRUE(ret == ERR_OK);
    auto error = mediaChannel->HandleStopConsumer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_096, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    std::string className = "TestConsumer";
    auto ret = mediaChannel->CreateConsumer(className);
    ASSERT_TRUE(ret == ERR_OK);
    auto error = mediaChannel->HandleStopConsumer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_097, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto mediaController = std::make_shared<MediaController>(1);
    EXPECT_NE(mediaController, nullptr);
    auto error = mediaChannel->HandleStopConsumer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_098, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto error = mediaChannel->HandleStopConsumer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_099, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    std::string className = "TestConsumer";
    auto ret = mediaChannel->CreateConsumer(className);
    ASSERT_TRUE(ret == ERR_OK);
    auto error = mediaChannel->HandleStartConsumer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_100, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto error = mediaChannel->HandleStartConsumer(event);
    EXPECT_NE(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_101, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto mediaController = std::make_shared<MediaController>(1);
    EXPECT_NE(mediaController, nullptr);
    std::string className = "TestConsumer";
    auto ret = mediaChannel->CreateConsumer(className);
    ASSERT_TRUE(ret == ERR_OK);
    auto error = mediaChannel->HandleStopConsumer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_102, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    std::string className = "TestConsumer";
    auto ret = mediaChannel->CreateConsumer(className);
    ASSERT_TRUE(ret == ERR_OK);
    auto error = mediaChannel->HandlePauseConsumer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_103, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto mediaController = std::make_shared<MediaController>(1);
    EXPECT_NE(mediaController, nullptr);
    auto error = mediaChannel->HandlePauseConsumer(event);
    EXPECT_NE(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_104, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    auto channelMsg = std::make_shared<ChannelEventMsg>();
    channelMsg->className = "TestConsumer";
    event.eventMsg = channelMsg;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto error = mediaChannel->HandleCreateConsumer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_105, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    auto channelMsg = std::make_shared<ChannelEventMsg>();
    channelMsg->className = "TestConsumer1";
    event.eventMsg = channelMsg;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto error = mediaChannel->HandleCreateConsumer(event);
    EXPECT_NE(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_106, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    std::string className = "TestConsumer";
    auto ret = mediaChannel->CreateConsumer(className);
    ASSERT_TRUE(ret == ERR_OK);
    auto error = mediaChannel->HandleResumeConsumer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_107, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto error = mediaChannel->HandleResumeConsumer(event);
    EXPECT_NE(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_108, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    std::string className = "TestConsumer";
    auto ret = mediaChannel->CreateConsumer(className);
    ASSERT_TRUE(ret == ERR_OK);
    auto error = mediaChannel->HandleDestroyConsumer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_109, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    std::string className = "TestConsumer";
    auto ret = mediaChannel->CreateConsumer(className);
    ASSERT_TRUE(ret == ERR_OK);
    auto error = mediaChannel->HandleDestroyConsumer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_110, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto error = mediaChannel->HandleDestroyConsumer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_111, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto error = mediaChannel->HandleStopProducer(event);
    EXPECT_NE(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_112, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    auto channelMsg = std::make_shared<ChannelEventMsg>();
    event.eventMsg = channelMsg;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    std::string className = "TestProducer";
    auto ret = mediaChannel->CreateProducer(className);
    EXPECT_NE(ret, 0);
    channelMsg->prosumerId = ret;
    auto error = mediaChannel->HandleStopProducer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_113, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto error = mediaChannel->HandlePauseProducer(event);
    EXPECT_NE(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_114, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    auto channelMsg = std::make_shared<ChannelEventMsg>();
    event.eventMsg = channelMsg;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    std::string className = "TestProducer";
    auto ret = mediaChannel->CreateProducer(className);
    EXPECT_NE(ret, 0);
    channelMsg->prosumerId = ret;
    auto error = mediaChannel->HandlePauseProducer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_115, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    auto channelMsg = std::make_shared<ChannelEventMsg>();
    event.eventMsg = channelMsg;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    std::string className = "TestProducer";
    auto ret = mediaChannel->CreateProducer(className);
    EXPECT_NE(ret, 0);
    channelMsg->prosumerId = ret;
    auto error = mediaChannel->HandleStartProducer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_116, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto error = mediaChannel->HandleStartProducer(event);
    EXPECT_NE(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_117, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    auto channelMsg = std::make_shared<ChannelEventMsg>();
    channelMsg->className = "TestProducer";
    event.eventMsg = channelMsg;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto error = mediaChannel->HandleCreateProducer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_118, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    auto channelMsg = std::make_shared<ChannelEventMsg>();
    channelMsg->className = "TestProducer1";
    event.eventMsg = channelMsg;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto error = mediaChannel->HandleCreateProducer(event);
    EXPECT_NE(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_119, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    auto channelMsg = std::make_shared<ChannelEventMsg>();
    event.eventMsg = channelMsg;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    std::string className = "TestProducer";
    auto ret = mediaChannel->CreateProducer(className);
    EXPECT_NE(ret, 0);
    channelMsg->prosumerId = ret;
    auto error = mediaChannel->HandleResumeProducer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_120, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto error = mediaChannel->HandleResumeProducer(event);
    EXPECT_NE(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_121, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    auto channelMsg = std::make_shared<ChannelEventMsg>();
    event.eventMsg = channelMsg;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    std::string className = "TestProducer";
    auto ret = mediaChannel->CreateProducer(className);
    EXPECT_NE(ret, 0);
    mediaChannel->producers_[ret]->isRunning_ = true;
    channelMsg->prosumerId = ret;
    auto error = mediaChannel->HandleDestroyProducer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_122, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    auto channelMsg = std::make_shared<ChannelEventMsg>();
    event.eventMsg = channelMsg;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    std::string className = "TestProducer";
    auto ret = mediaChannel->CreateProducer(className);
    EXPECT_NE(ret, 0);
    mediaChannel->producers_[ret]->isRunning_ = true;
    channelMsg->prosumerId = ret + 1;
    auto error = mediaChannel->HandleDestroyProducer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_123, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    auto channelMsg = std::make_shared<ChannelEventMsg>();
    event.eventMsg = channelMsg;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    std::string className = "TestProducer";
    auto ret = mediaChannel->CreateProducer(className);
    EXPECT_NE(ret, 0);
    channelMsg->prosumerId = ret;
    auto error = mediaChannel->HandleDestroyProducer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_124, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    auto channelMsg = std::make_shared<ChannelEventMsg>();
    event.eventMsg = channelMsg;
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    std::string className = "TestProducer";
    auto ret = mediaChannel->CreateProducer(className);
    EXPECT_NE(ret, 0);
    channelMsg->prosumerId = ret + 1;
    auto error = mediaChannel->HandleDestroyProducer(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_125, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    std::string className = "TestConsumer";
    auto ret = mediaChannel->CreateConsumer(className);
    EXPECT_EQ(ret, ERR_OK);
    auto ret1 = mediaChannel->InitPlayController();
    EXPECT_NE(ret1, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_126, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    auto mediaController = std::make_shared<MediaController>(1);
    EXPECT_NE(mediaController, nullptr);
    std::string className = "TestConsumer";
    auto ret = mediaChannel->CreateConsumer(className);
    EXPECT_EQ(ret, ERR_OK);
    mediaChannel->playController_ = mediaController;
    auto ret1 = mediaChannel->InitPlayController();
    EXPECT_EQ(ret1, SharingErrorCode::ERR_DECODE_FORMAT);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_127, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto mediaController = std::make_shared<MediaController>(1);
    EXPECT_NE(mediaController, nullptr);
    mediaChannel->playController_ = mediaController;
    auto error = mediaChannel->HandleStopPlay(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_128, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto error = mediaChannel->HandleStopPlay(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaChannel_129, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelSetVolumeEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto mediaController = std::make_shared<MediaController>(1);
    EXPECT_NE(mediaController, nullptr);
    mediaChannel->playController_ = mediaController;
    auto error = mediaChannel->HandleSetVolume(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaDispatcher_130, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelSetVolumeEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto error = mediaChannel->HandleSetVolume(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaDispatcher_131, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto mediaController = std::make_shared<MediaController>(1);
    EXPECT_NE(mediaController, nullptr);
    mediaChannel->playController_ = mediaController;
    auto error = mediaChannel->HandleStartPlay(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaDispatcher_132, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto error = mediaChannel->HandleStartPlay(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaDispatcher_133, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelSetSceneTypeEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto mediaController = std::make_shared<MediaController>(1);
    EXPECT_NE(mediaController, nullptr);
    mediaChannel->playController_ = mediaController;
    auto error = mediaChannel->HandleSetSceneType(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaDispatcher_134, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelSetSceneTypeEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto error = mediaChannel->HandleSetSceneType(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaDispatcher_135, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelAppendSurfaceEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto mediaController = std::make_shared<MediaController>(1);
    EXPECT_NE(mediaController, nullptr);
    mediaChannel->playController_ = mediaController;
    auto error = mediaChannel->HandleAppendSurface(event);
    EXPECT_NE(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaChannelUnitTest, MediaDispatcher_136, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelAppendSurfaceEventMsg>();
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto error = mediaChannel->HandleAppendSurface(event);
    EXPECT_NE(error, SharingErrorCode::ERR_OK);
}

} // namespace
} // namespace Sharing
} // namespace OHOS
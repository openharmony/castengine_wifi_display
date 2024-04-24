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

#include "media_dispatcher_unit_test.h"
#include <iostream>
#include "common/reflect_registration.h"
#include "mediachannel/channel_manager.h"
#include "mediachannel/media_channel.h"

using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

void MediaDispatcherUnitTest::SetUpTestCase() {}
void MediaDispatcherUnitTest::TearDownTestCase() {}
void MediaDispatcherUnitTest::SetUp() {}
void MediaDispatcherUnitTest::TearDown() {}

class BufferDispatcherListenerImpl : public BufferDispatcherListener {
public:
    ~BufferDispatcherListenerImpl() = default;

    void OnWriteTimeout() {}
};

class BufferReceiverListener : public IBufferReceiverListener {
public:
    BufferReceiverListener(){};
    ~BufferReceiverListener(){};

    void OnAccelerationDoneNotify() {}
    void OnKeyModeNotify(bool enable)
    {
        (void)enable;
    }
};

namespace {
HWTEST_F(MediaDispatcherUnitTest, MediaDispatcher_001, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelEventMsg>();
    EXPECT_NE(event.eventMsg, nullptr);
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto error = mediaChannel->HandleRemoveSurface(event);
    EXPECT_NE(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaDispatcherUnitTest, MediaDispatcher_002, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelRemoveSurfaceEventMsg>();
    EXPECT_NE(event.eventMsg, nullptr);
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto mediaController = std::make_shared<MediaController>(1);
    EXPECT_NE(mediaController, nullptr);
    mediaChannel->playController_ = mediaController;
    std::string className = "TestConsumer";
    auto ret = mediaChannel->CreateConsumer(className);
    EXPECT_EQ(ret, ERR_OK);
    auto error = mediaChannel->HandleRemoveSurface(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaDispatcherUnitTest, MediaDispatcher_003, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelSetKeyRedirectEventMsg>();
    EXPECT_NE(event.eventMsg, nullptr);
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto error = mediaChannel->HandleSetKeyRedirect(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaDispatcherUnitTest, MediaDispatcher_004, Function | SmallTest | Level2)
{
    auto mediaChannel = std::make_shared<MediaChannel>();
    EXPECT_NE(mediaChannel, nullptr);
    SharingEvent event;
    event.description = "test";
    event.emitterType = CLASS_TYPE_CONSUMER;
    event.eventMsg = std::make_shared<ChannelSetKeyRedirectEventMsg>();
    EXPECT_NE(event.eventMsg, nullptr);
    event.listenerType = CLASS_TYPE_SCHEDULER;
    event.eventMsg->type = EventType::EVENT_MEDIA_PLAY_STOP;
    auto mediaController = std::make_shared<MediaController>(1);
    EXPECT_NE(mediaController, nullptr);
    mediaChannel->playController_ = mediaController;
    auto error = mediaChannel->HandleSetKeyRedirect(event);
    EXPECT_EQ(error, SharingErrorCode::ERR_OK);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_005, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto ret = bufferReceiver->IsMixedReceiver();
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_006, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto ret = bufferReceiver->OnMediaDataNotify();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_007, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto ret = bufferReceiver->OnAudioDataNotify();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_008, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto ret = bufferReceiver->OnVideoDataNotify();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_009, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    MediaType type = MEDIA_TYPE_AV;
    auto f = [](const MediaData::Ptr &data) { (void)data; };
    auto ret = bufferReceiver->RequestRead(type, f);
    EXPECT_NE(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_010, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto bufferDispatcher = std::make_shared<BufferDispatcher>();
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferReceiver->SetSource(bufferDispatcher);
    MediaType type = MEDIA_TYPE_AV;
    auto f = [](const MediaData::Ptr &data) { (void)data; };
    std::thread test([&]() {
        auto ret = bufferReceiver->RequestRead(type, f);
        EXPECT_EQ(ret, -1);
    });
    bufferReceiver->OnMediaDataNotify();
    test.join();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_011, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto bufferDispatcher = std::make_shared<BufferDispatcher>();
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferReceiver->SetSource(bufferDispatcher);
    MediaType type = MEDIA_TYPE_AUDIO;
    auto f = [](const MediaData::Ptr &data) { (void)data; };
    std::thread test([&]() {
        auto ret = bufferReceiver->RequestRead(type, f);
        EXPECT_EQ(ret, -1);
    });
    bufferReceiver->OnAudioDataNotify();
    test.join();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_012, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto bufferDispatcher = std::make_shared<BufferDispatcher>();
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferReceiver->SetSource(bufferDispatcher);
    MediaType type = MEDIA_TYPE_VIDEO;
    auto f = [](const MediaData::Ptr &data) { (void)data; };
    std::thread test([&]() {
        auto ret = bufferReceiver->RequestRead(type, f);
        EXPECT_EQ(ret, -1);
    });
    bufferReceiver->OnVideoDataNotify();
    test.join();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_013, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto ret = bufferReceiver->GetReceiverId();
    EXPECT_NE(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_014, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto ret = bufferReceiver->GetDispatcherId();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_015, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto bufferDispatcher = std::make_shared<BufferDispatcher>();
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferReceiver->SetSource(bufferDispatcher);
    auto ret = bufferReceiver->GetDispatcherId();
    EXPECT_NE(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_016, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    bufferReceiver->NotifyReadStop();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_017, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    bufferReceiver->NotifyReadStart();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_018, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    bufferReceiver->keyOnly_ = true;
    bufferReceiver->EnableKeyMode(false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_019, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    bufferReceiver->EnableKeyMode(true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_020, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    bufferReceiver->EnableKeyMode(false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_021, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto bufferDispatcher = std::make_shared<BufferDispatcher>();
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferReceiver->SetSource(bufferDispatcher);
    bufferReceiver->EnableKeyMode(true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_022, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto bufferDispatcher = std::make_shared<BufferDispatcher>();
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferReceiver->SetSource(bufferDispatcher);
    auto receiverListener = std::make_shared<BufferReceiverListener>();
    EXPECT_NE(receiverListener, nullptr);
    bufferReceiver->SetBufferReceiverListener(receiverListener);
    bufferReceiver->EnableKeyMode(true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_023, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto ret = bufferReceiver->IsKeyMode();
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_024, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto ret = bufferReceiver->IsKeyRedirect();
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_025, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto ret = bufferReceiver->GetSPS();
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_026, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto bufferDispatcher = std::make_shared<BufferDispatcher>();
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferReceiver->SetSource(bufferDispatcher);
    auto ret = bufferReceiver->GetSPS();
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_027, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto ret = bufferReceiver->GetPPS();
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_028, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto bufferDispatcher = std::make_shared<BufferDispatcher>();
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferReceiver->SetSource(bufferDispatcher);
    auto ret = bufferReceiver->GetPPS();
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_029, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto ret = bufferReceiver->NeedAcceleration();
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_030, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    bufferReceiver->DisableAcceleration();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_031, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    bufferReceiver->SendAccelerationDone();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_032, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    bufferReceiver->EnableKeyRedirect(false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_033, Function | SmallTest | Level2)
{
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto bufferDispatcher = std::make_shared<BufferDispatcher>();
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferReceiver->SetSource(bufferDispatcher);
    bufferReceiver->EnableKeyRedirect(true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_034, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    uint32_t index = 1;
    dataNotifier->SetReadIndex(index);
    auto ret = dataNotifier->GetReadIndex();
    EXPECT_EQ(ret, index);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_035, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    dataNotifier->SetBlock();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_036, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    dataNotifier->SendAccelerationDone();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_037, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    dataNotifier->NotifyDataReceiver(MEDIA_TYPE_AV);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_038, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    dataNotifier->SetNotifyReceiver(bufferReceiver);
    dataNotifier->NotifyDataReceiver(MEDIA_TYPE_AV);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_039, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    dataNotifier->SetNotifyReceiver(bufferReceiver);
    dataNotifier->NotifyDataReceiver(MEDIA_TYPE_AUDIO);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_040, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    dataNotifier->SetNotifyReceiver(bufferReceiver);
    dataNotifier->NotifyDataReceiver(MEDIA_TYPE_VIDEO);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_041, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    dataNotifier->SetNotifyReceiver(bufferReceiver);
    dataNotifier->block_ = true;
    dataNotifier->NotifyDataReceiver(MEDIA_TYPE_AV);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_042, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    dataNotifier->SetNeedUpdate(true, MEDIA_TYPE_AV);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_043, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    dataNotifier->SetNeedUpdate(true, MEDIA_TYPE_AUDIO);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_044, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    dataNotifier->SetNotifyReceiver(bufferReceiver);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_045, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto bufferDispatcher = std::make_shared<BufferDispatcher>();
    EXPECT_NE(bufferDispatcher, nullptr);
    dataNotifier->SetListenDispatcher(bufferDispatcher);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_046, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto ret = dataNotifier->IsMixedReceiver();
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_047, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto ret = dataNotifier->NeedAcceleration();
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_048, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto ret = dataNotifier->IsKeyModeReceiver();
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_049, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto ret = dataNotifier->IsKeyRedirectReceiver();
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_050, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto ret = dataNotifier->DataAvailable(MEDIA_TYPE_AV);
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_051, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto bufferDispatcher = std::make_shared<BufferDispatcher>();
    EXPECT_NE(bufferDispatcher, nullptr);
    dataNotifier->SetListenDispatcher(bufferDispatcher);
    auto ret = dataNotifier->DataAvailable(MEDIA_TYPE_AV);
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_052, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto bufferDispatcher = std::make_shared<BufferDispatcher>();
    EXPECT_NE(bufferDispatcher, nullptr);
    dataNotifier->SetListenDispatcher(bufferDispatcher);
    auto ret = dataNotifier->DataAvailable(MEDIA_TYPE_AUDIO);
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_053, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto bufferDispatcher = std::make_shared<BufferDispatcher>();
    EXPECT_NE(bufferDispatcher, nullptr);
    dataNotifier->SetListenDispatcher(bufferDispatcher);
    auto ret = dataNotifier->DataAvailable(MEDIA_TYPE_VIDEO);
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_054, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto ret = dataNotifier->GetReceiverId();
    EXPECT_NE(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_055, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto ret = dataNotifier->GetReceiverReadIndex(MEDIA_TYPE_AV);
    EXPECT_NE(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_056, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto ret = dataNotifier->GetReceiverReadIndex(MEDIA_TYPE_AUDIO);
    EXPECT_NE(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_057, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto ret = dataNotifier->GetReceiverReadIndex(MEDIA_TYPE_VIDEO);
    EXPECT_NE(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_058, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto ret = dataNotifier->GetReceiverReadIndex((MediaType)100);
    EXPECT_EQ(ret, INVALID_INDEX);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_059, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto ret = dataNotifier->GetBufferReceiver();
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_060, Function | SmallTest | Level2)
{
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    dataNotifier->SetNotifyReceiver(bufferReceiver);
    auto ret = dataNotifier->GetBufferReceiver();
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_061, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto ret = bufferDispatcher->GetDispatcherId();
    EXPECT_NE(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_062, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferDispatcher->StopDispatch();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_063, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_064, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferDispatcher->CancelReserve();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_065, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferDispatcher->ReleaseAllReceiver();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_066, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto ret = bufferDispatcher->AttachReceiver(bufferReceiver);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_067, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto ret = bufferDispatcher->AttachReceiver(nullptr);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_068, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto ret = bufferDispatcher->AttachReceiver(bufferReceiver);
    EXPECT_EQ(ret, 0);
    auto ret1 = bufferDispatcher->DetachReceiver(bufferReceiver);
    EXPECT_EQ(ret1, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_069, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    auto ret1 = bufferDispatcher->DetachReceiver(bufferReceiver);
    EXPECT_EQ(ret1, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_070, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dispatcherListener = std::make_shared<BufferDispatcherListenerImpl>();
    EXPECT_NE(dispatcherListener, nullptr);
    bufferDispatcher->SetBufferDispatcherListener(dispatcherListener);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_071, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto mediaData = std::make_shared<MediaData>();
    EXPECT_NE(mediaData, nullptr);
    bufferDispatcher->SetSpsNalu(mediaData);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_072, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto mediaData = std::make_shared<MediaData>();
    EXPECT_NE(mediaData, nullptr);
    bufferDispatcher->SetPpsNalu(mediaData);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_073, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    size_t capacity = 100;
    bufferDispatcher->SetBufferCapacity(capacity);
    auto ret = bufferDispatcher->GetBufferSize();
    EXPECT_NE(ret, capacity);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_074, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    MediaDispacherMode dataMode = MEDIA_VIDEO_ONLY;
    bufferDispatcher->SetDataMode(dataMode);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_075, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto mediaData = std::make_shared<MediaData>();
    EXPECT_NE(mediaData, nullptr);
    auto ret = bufferDispatcher->InputData(mediaData);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_076, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferDispatcher->FlushBuffer();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_077, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferDispatcher->ReleaseIdleBuffer();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_078, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferDispatcher->EnableKeyMode(true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_079, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferDispatcher->EnableRapidMode(true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_080, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    uint32_t receiverId = 0;
    MediaType type = MEDIA_TYPE_AV;
    bufferDispatcher->ClearReadBit(receiverId, type);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_081, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    uint32_t receiverId = 0;
    MediaType type = MEDIA_TYPE_VIDEO;
    bufferDispatcher->ClearReadBit(receiverId, type);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_082, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    uint32_t receiverId = 0;
    MediaType type = MEDIA_TYPE_AV;
    bufferDispatcher->ClearDataBit(receiverId, type);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_083, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    uint32_t receiverId = 0;
    MediaType type = MEDIA_TYPE_VIDEO;
    bufferDispatcher->ClearDataBit(receiverId, type);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_084, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    uint32_t receiverId = 0;
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    bufferDispatcher->SetReceiverReadFlag(receiverId, dataSpec);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_085, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    uint32_t receiverId = 0;
    auto ret = bufferDispatcher->IsRecevierExist(receiverId);
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_086, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    bufferDispatcher->AttachReceiver(bufferReceiver);
    uint32_t receiverId = bufferReceiver->GetReceiverId();
    auto ret = bufferDispatcher->IsRecevierExist(receiverId);
    EXPECT_EQ(ret, true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_087, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    bufferDispatcher->AttachReceiver(bufferReceiver);
    uint32_t receiverId = bufferReceiver->GetReceiverId();
    auto ret = bufferDispatcher->IsRead(receiverId, 1);
    EXPECT_EQ(ret, true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_088, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    bufferDispatcher->AttachReceiver(bufferReceiver);
    uint32_t receiverId = bufferReceiver->GetReceiverId();
    auto ret = bufferDispatcher->IsRead(receiverId, 0);
    EXPECT_EQ(ret, true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_089, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto ret = bufferDispatcher->GetCurrentGop();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_090, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferDispatcher->NotifyReadReady(1, MEDIA_TYPE_AV);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_091, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    bufferDispatcher->AttachReceiver(bufferReceiver);
    uint32_t receiverId = bufferReceiver->GetReceiverId();
    bufferDispatcher->NotifyReadReady(receiverId, MEDIA_TYPE_AV);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_092, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    bufferDispatcher->AttachReceiver(bufferReceiver);
    uint32_t receiverId = bufferReceiver->GetReceiverId();
    bufferDispatcher->NotifyReadReady(receiverId, MEDIA_TYPE_AUDIO);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_093, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto f = [](const MediaData::Ptr &data) { (void)data; };
    bufferDispatcher->ReadBufferData(1, MEDIA_TYPE_AV, f);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_094, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto f = [](const MediaData::Ptr &data) { (void)data; };
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    bufferDispatcher->AttachReceiver(bufferReceiver);
    uint32_t receiverId = bufferReceiver->GetReceiverId();
    bufferDispatcher->ReadBufferData(receiverId, MEDIA_TYPE_AV, f);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_095, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto ret = bufferDispatcher->GetSPS();
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_096, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto ret = bufferDispatcher->GetPPS();
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_097, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    MediaType type = MEDIA_TYPE_VIDEO;
    uint32_t size = 0;
    auto ret = bufferDispatcher->RequestDataBuffer(type, size);
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_098, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    MediaType type = MEDIA_TYPE_VIDEO;
    uint32_t size = 100;
    auto ret = bufferDispatcher->RequestDataBuffer(type, size);
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_099, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    MediaType type = MEDIA_TYPE_AUDIO;
    uint32_t size = 100;
    auto ret = bufferDispatcher->RequestDataBuffer(type, size);
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_100, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    uint32_t receiverId = 0;
    auto ret = bufferDispatcher->GetNotifierByReceiverId(receiverId);
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_101, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    bufferDispatcher->AttachReceiver(bufferReceiver);
    uint32_t receiverId = bufferReceiver->GetReceiverId();
    auto ret = bufferDispatcher->GetNotifierByReceiverId(receiverId);
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_102, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    BufferReceiver::Ptr receiver = nullptr;
    auto ret = bufferDispatcher->GetNotifierByReceiverPtr(receiver);
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_103, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    bufferDispatcher->AttachReceiver(bufferReceiver);
    BufferReceiver::Ptr receiver = bufferReceiver;
    auto ret = bufferDispatcher->GetNotifierByReceiverPtr(receiver);
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_104, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferDispatcher->UpdateIndex();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_105, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferDispatcher->ResetAllIndex();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_106, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    dataSpec->mediaData = std::make_shared<MediaData>();
    EXPECT_NE(dataSpec->mediaData, nullptr);
    dataSpec->mediaData->mediaType = MEDIA_TYPE_VIDEO;
    auto ret = bufferDispatcher->IsVideoData(dataSpec);
    EXPECT_EQ(ret, true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_107, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    dataSpec->mediaData = std::make_shared<MediaData>();
    EXPECT_NE(dataSpec->mediaData, nullptr);
    dataSpec->mediaData->mediaType = MEDIA_TYPE_AUDIO;
    auto ret = bufferDispatcher->IsVideoData(dataSpec);
    EXPECT_NE(ret, true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_108, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = nullptr;
    auto ret = bufferDispatcher->IsVideoData(dataSpec);
    EXPECT_NE(ret, true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_109, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    dataSpec->mediaData = std::make_shared<MediaData>();
    EXPECT_NE(dataSpec->mediaData, nullptr);
    dataSpec->mediaData->mediaType = MEDIA_TYPE_AUDIO;
    auto ret = bufferDispatcher->IsAudioData(dataSpec);
    EXPECT_EQ(ret, true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_110, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    dataSpec->mediaData = std::make_shared<MediaData>();
    EXPECT_NE(dataSpec->mediaData, nullptr);
    dataSpec->mediaData->mediaType = MEDIA_TYPE_VIDEO;
    auto ret = bufferDispatcher->IsAudioData(dataSpec);
    EXPECT_NE(ret, true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_111, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = nullptr;
    auto ret = bufferDispatcher->IsAudioData(dataSpec);
    EXPECT_NE(ret, true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_112, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    dataSpec->mediaData = std::make_shared<MediaData>();
    EXPECT_NE(dataSpec->mediaData, nullptr);
    dataSpec->mediaData->mediaType = MEDIA_TYPE_AUDIO;
    auto ret = bufferDispatcher->IsKeyVideoFrame(dataSpec);
    EXPECT_NE(ret, true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_113, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    dataSpec->mediaData = std::make_shared<MediaData>();
    EXPECT_NE(dataSpec->mediaData, nullptr);
    dataSpec->mediaData->mediaType = MEDIA_TYPE_VIDEO;
    auto ret = bufferDispatcher->IsKeyVideoFrame(dataSpec);
    EXPECT_NE(ret, true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_114, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    auto bufferReceiver = std::make_shared<BufferReceiver>();
    EXPECT_NE(bufferReceiver, nullptr);
    bufferDispatcher->AttachReceiver(bufferReceiver);
    uint32_t receiverId = bufferReceiver->GetReceiverId();
    auto ret = bufferDispatcher->IsDataReaded(receiverId, dataSpec);
    EXPECT_NE(ret, true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_115, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    uint32_t receiverId = 0;
    auto ret = bufferDispatcher->IsDataReaded(receiverId, dataSpec);
    EXPECT_NE(ret, true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_116, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    dataSpec->mediaData = std::make_shared<MediaData>();
    EXPECT_NE(dataSpec->mediaData, nullptr);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    auto ret = bufferDispatcher->FindNextDeleteVideoIndex();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_117, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto ret = bufferDispatcher->FindNextDeleteVideoIndex();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_118, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto ret = bufferDispatcher->FindLastIndex(MEDIA_TYPE_AUDIO);
    EXPECT_EQ(ret, INVALID_INDEX);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_119, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    auto ret = bufferDispatcher->FindLastIndex(MEDIA_TYPE_AUDIO);
    EXPECT_EQ(ret, INVALID_INDEX);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_120, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    uint32_t index = 0;
    auto ret = bufferDispatcher->FindNextIndex(index, MEDIA_TYPE_VIDEO);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_121, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    uint32_t index = 0;
    auto ret = bufferDispatcher->FindNextIndex(index, MEDIA_TYPE_VIDEO);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_122, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    uint32_t index = 0;
    auto ret = bufferDispatcher->FindNextIndex(index, MEDIA_TYPE_AV);
    EXPECT_EQ(ret, 1);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_123, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    uint32_t index = 0;
    auto ret = bufferDispatcher->FindNextIndex(index, MEDIA_TYPE_AUDIO);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_124, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    uint32_t index = 0;
    auto ret = bufferDispatcher->FindNextIndex(index, MEDIA_TYPE_VIDEO);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_125, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    uint32_t index = 0;
    uint32_t receiverId = 0;
    auto ret = bufferDispatcher->FindNextIndex(index, MEDIA_TYPE_AV, receiverId);
    EXPECT_EQ(ret, 1);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_126, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    uint32_t index = 0;
    uint32_t receiverId = 0;
    auto ret = bufferDispatcher->FindNextIndex(index, MEDIA_TYPE_AUDIO, receiverId);
    EXPECT_NE(ret, 1);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_127, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    uint32_t index = 0;
    uint32_t receiverId = 0;
    auto ret = bufferDispatcher->FindNextIndex(index, MEDIA_TYPE_VIDEO, receiverId);
    EXPECT_EQ(ret, INVALID_INDEX);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_128, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    uint32_t index = 0;
    uint32_t receiverId = 0;
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    bufferDispatcher->notifiers_.emplace(receiverId, dataNotifier);
    auto ret = bufferDispatcher->FindNextIndex(index, MEDIA_TYPE_VIDEO, receiverId);
    EXPECT_EQ(ret, index);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_129, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    uint32_t index = 0;
    uint32_t receiverId = 0;
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    bufferDispatcher->notifiers_.emplace(receiverId, dataNotifier);
    auto ret = bufferDispatcher->FindNextIndex(index, MEDIA_TYPE_AUDIO, receiverId);
    EXPECT_EQ(ret, index);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_130, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferDispatcher->EraseOldGopDatas();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_131, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferDispatcher->SetDataMode(MEDIA_AUDIO_ONLY);
    bufferDispatcher->EraseOldGopDatas();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_132, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferDispatcher->ReCalculateCapacity(true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_133, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferDispatcher->ReCalculateCapacity(false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_134, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    BufferDispatcher::DataSpec::Ptr data = nullptr;
    bufferDispatcher->ReturnIdleBuffer(data);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_135, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    dataSpec->mediaData = std::make_shared<MediaData>();
    EXPECT_NE(dataSpec->mediaData, nullptr);
    dataSpec->mediaData->mediaType = MEDIA_TYPE_VIDEO;
    bufferDispatcher->ReturnIdleBuffer(dataSpec);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_136, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    dataSpec->mediaData = std::make_shared<MediaData>();
    EXPECT_NE(dataSpec->mediaData, nullptr);
    dataSpec->mediaData->mediaType = MEDIA_TYPE_AUDIO;
    bufferDispatcher->ReturnIdleBuffer(dataSpec);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_137, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    uint32_t size = 0;
    bool forceDelete = false;
    bufferDispatcher->DeleteHeadDatas(size, forceDelete);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_138, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    uint32_t size = 1;
    bool forceDelete = false;
    bufferDispatcher->DeleteHeadDatas(size, forceDelete);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_139, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    uint32_t size = 1;
    bool forceDelete = false;
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    dataSpec->mediaData = std::make_shared<MediaData>();
    EXPECT_NE(dataSpec->mediaData, nullptr);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    bufferDispatcher->DeleteHeadDatas(size, forceDelete);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_140, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    dataSpec->mediaData = std::make_shared<MediaData>();
    EXPECT_NE(dataSpec->mediaData, nullptr);
    dataSpec->mediaData->mediaType = MEDIA_TYPE_AUDIO;
    bufferDispatcher->PreProcessDataSpec(dataSpec);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_141, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    dataSpec->mediaData = std::make_shared<MediaData>();
    EXPECT_NE(dataSpec->mediaData, nullptr);
    dataSpec->mediaData->mediaType = MEDIA_TYPE_VIDEO;
    bufferDispatcher->PreProcessDataSpec(dataSpec);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_142, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    dataSpec->mediaData = std::make_shared<MediaData>();
    EXPECT_NE(dataSpec->mediaData, nullptr);
    dataSpec->mediaData->mediaType = MEDIA_TYPE_AV;
    bufferDispatcher->PreProcessDataSpec(dataSpec);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_143, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    dataSpec->mediaData = std::make_shared<MediaData>();
    EXPECT_NE(dataSpec->mediaData, nullptr);
    dataSpec->mediaData->mediaType = MEDIA_TYPE_AV;
    bufferDispatcher->waitingKey_ = false;
    bufferDispatcher->PreProcessDataSpec(dataSpec);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_144, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    dataSpec->mediaData = std::make_shared<MediaData>();
    EXPECT_NE(dataSpec->mediaData, nullptr);
    dataSpec->mediaData->mediaType = MEDIA_TYPE_AV;
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    auto ret = bufferDispatcher->HeadFrameNeedReserve();
    EXPECT_NE(ret, true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_145, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto ret = bufferDispatcher->HeadFrameNeedReserve();
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_146, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    dataSpec->mediaData = std::make_shared<MediaData>();
    EXPECT_NE(dataSpec->mediaData, nullptr);
    dataSpec->mediaData->mediaType = MEDIA_TYPE_AV;
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    auto ret = bufferDispatcher->NeedExtendToDBCapacity();
    EXPECT_NE(ret, true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_147, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto ret = bufferDispatcher->NeedExtendToDBCapacity();
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_148, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    dataSpec->mediaData = std::make_shared<MediaData>();
    EXPECT_NE(dataSpec->mediaData, nullptr);
    dataSpec->mediaData->mediaType = MEDIA_TYPE_AV;
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    auto ret = bufferDispatcher->NeedRestoreToNormalCapacity();
    EXPECT_NE(ret, true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_149, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto ret = bufferDispatcher->NeedRestoreToNormalCapacity();
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_150, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    dataSpec->mediaData = std::make_shared<MediaData>();
    EXPECT_NE(dataSpec->mediaData, nullptr);
    dataSpec->mediaData->mediaType = MEDIA_TYPE_VIDEO;
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    bufferDispatcher->EnableKeyMode(true);
    auto ret = bufferDispatcher->WriteDataIntoBuffer(dataSpec);
    EXPECT_NE(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_151, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    dataSpec->mediaData = std::make_shared<MediaData>();
    EXPECT_NE(dataSpec->mediaData, nullptr);
    dataSpec->mediaData->mediaType = MEDIA_TYPE_AUDIO;
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    bufferDispatcher->EnableKeyMode(true);
    auto ret = bufferDispatcher->WriteDataIntoBuffer(dataSpec);
    EXPECT_NE(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_152, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto ret = bufferDispatcher->NeedRestoreToNormalCapacity();
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_153, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    auto ret = bufferDispatcher->NeedRestoreToNormalCapacity();
    EXPECT_EQ(ret, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_154, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferDispatcher->OnKeyRedirect();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_155, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    bufferDispatcher->keyIndexList_.push_back(1);
    bufferDispatcher->OnKeyRedirect();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_156, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    uint32_t bitref = 1;
    bufferDispatcher->SetDataRef(bitref);
    auto ret = bufferDispatcher->GetDataRef();
    EXPECT_NE(ret, bitref);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_157, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    uint32_t bitref = 1;
    bufferDispatcher->SetReadRef(bitref);
    auto ret = bufferDispatcher->GetReadRef();
    EXPECT_NE(ret, bitref);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_158, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    MediaType type = MEDIA_TYPE_AV;
    bufferDispatcher->UnlockWaitingReceiverIndex(type);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_159, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    MediaType type = MEDIA_TYPE_AV;
    bufferDispatcher->notifiers_.emplace(1, dataNotifier);
    bufferDispatcher->ActiveDataRef(type, true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_160, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    MediaType type = MEDIA_TYPE_AV;
    bufferDispatcher->notifiers_.emplace(1, dataNotifier);
    bufferDispatcher->ActiveDataRef(type, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_161, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    MediaType type = MEDIA_TYPE_VIDEO;
    uint32_t index = 1;
    bufferDispatcher->notifiers_.emplace(index, dataNotifier);
    bufferDispatcher->ActivateReceiverIndex(index, type);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_162, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    MediaType type = MEDIA_TYPE_AUDIO;
    uint32_t index = 1;
    bufferDispatcher->notifiers_.emplace(index, dataNotifier);
    bufferDispatcher->ActivateReceiverIndex(index, type);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_163, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    MediaType type = MEDIA_TYPE_AUDIO;
    uint32_t receiverId = 1;
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    bufferDispatcher->notifiers_.emplace(receiverId, dataNotifier);
    bufferDispatcher->SetReceiverDataRef(receiverId, type, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_164, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    MediaType type = MEDIA_TYPE_AUDIO;
    uint32_t receiverId = 1;
    bufferDispatcher->notifiers_.emplace(receiverId, dataNotifier);
    bufferDispatcher->SetReceiverDataRef(receiverId, type, true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_165, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    MediaType type = MEDIA_TYPE_AUDIO;
    uint32_t receiverId = 1;
    bufferDispatcher->notifiers_.emplace(receiverId, dataNotifier);
    bufferDispatcher->SetReceiverDataRef(receiverId, type, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_166, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    MediaType type = MEDIA_TYPE_VIDEO;
    uint32_t receiverId = 1;
    bufferDispatcher->notifiers_.emplace(receiverId, dataNotifier);
    bufferDispatcher->SetReceiverDataRef(receiverId, type, true);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_167, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    MediaType type = MEDIA_TYPE_VIDEO;
    uint32_t receiverId = 1;
    bufferDispatcher->notifiers_.emplace(receiverId, dataNotifier);
    bufferDispatcher->SetReceiverDataRef(receiverId, type, false);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_168, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    uint32_t receiverId = 1;
    auto ret = bufferDispatcher->GetReceiverDataRef(receiverId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_169, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    MediaType type = MEDIA_TYPE_AUDIO;
    uint32_t receiverId = 1;
    bufferDispatcher->notifiers_.emplace(receiverId, dataNotifier);
    bufferDispatcher->SetReceiverReadRef(receiverId, type, false);
    auto ret = bufferDispatcher->GetReceiverReadRef(receiverId);
    EXPECT_NE(ret, receiverId);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_170, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    MediaType type = MEDIA_TYPE_VIDEO;
    uint32_t receiverId = 1;
    bufferDispatcher->notifiers_.emplace(receiverId, dataNotifier);
    bufferDispatcher->SetReceiverReadRef(receiverId, type, true);
    auto ret = bufferDispatcher->GetReceiverReadRef(receiverId);
    EXPECT_NE(ret, receiverId);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_171, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    MediaType type = MEDIA_TYPE_VIDEO;
    uint32_t receiverId = 1;
    bufferDispatcher->notifiers_.emplace(receiverId, dataNotifier);
    bufferDispatcher->SetReceiverReadRef(receiverId, type, false);
    auto ret = bufferDispatcher->GetReceiverReadRef(receiverId);
    EXPECT_NE(ret, receiverId);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_172, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    MediaType type = MEDIA_TYPE_AV;
    uint32_t receiverId = 1;
    bufferDispatcher->notifiers_.emplace(receiverId, dataNotifier);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    bufferDispatcher->UpdateReceiverReadIndex(receiverId, 0, type);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_173, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    MediaType type = MEDIA_TYPE_AUDIO;
    uint32_t receiverId = 1;
    bufferDispatcher->notifiers_.emplace(receiverId, dataNotifier);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    bufferDispatcher->UpdateReceiverReadIndex(receiverId, 0, type);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_174, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    MediaType type = MEDIA_TYPE_VIDEO;
    uint32_t receiverId = 1;
    bufferDispatcher->notifiers_.emplace(receiverId, dataNotifier);
    auto dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    EXPECT_NE(dataSpec, nullptr);
    bufferDispatcher->circularBuffer_.push_back(dataSpec);
    bufferDispatcher->UpdateReceiverReadIndex(receiverId, 0, type);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_175, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    uint32_t receiverId = 1;
    bufferDispatcher->notifiers_.emplace(receiverId, dataNotifier);
    auto ret = bufferDispatcher->FindReceiverIndex(receiverId);
    EXPECT_NE(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_176, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    uint32_t receiverId = 1;
    auto ret = bufferDispatcher->FindReceiverIndex(receiverId);
    EXPECT_EQ(ret, INVALID_INDEX);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_177, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    uint32_t receiverId = 1;
    bufferDispatcher->notifiers_.emplace(receiverId, dataNotifier);
    auto ret = bufferDispatcher->GetReceiverIndexRef(receiverId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_178, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    uint32_t receiverId = 1;
    bufferDispatcher->notifiers_.emplace(receiverId, dataNotifier);
    std::thread testThread([&]() {
        auto ret = bufferDispatcher->NotifyThreadWorker((void *)bufferDispatcher.get());
        EXPECT_EQ(ret, 0);
    });
    bufferDispatcher->dataCV_.notify_one();
    bufferDispatcher->running_ = false;
    testThread.join();
}

HWTEST_F(MediaDispatcherUnitTest, BufferDispatcher_179, Function | SmallTest | Level2)
{
    int32_t maxCapacity = MAX_BUFFER_CAPACITY;
    uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT;
    auto bufferDispatcher = std::make_shared<BufferDispatcher>(maxCapacity, capacityIncrement);
    EXPECT_NE(bufferDispatcher, nullptr);
    auto dataNotifier = std::make_shared<BufferDispatcher::DataNotifier>();
    EXPECT_NE(dataNotifier, nullptr);
    uint32_t receiverId = 1;
    bufferDispatcher->notifiers_.emplace(receiverId, dataNotifier);
    bufferDispatcher->running_ = false;
    auto ret = bufferDispatcher->NotifyThreadWorker((void *)bufferDispatcher.get());
    EXPECT_EQ(ret, 0);
}

} // namespace
} // namespace Sharing
} // namespace OHOS
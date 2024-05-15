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

#include "wfd_rtp_producer_test.h"
#include "mock_rtp_pack.h"
#include "wfd_media_def.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

void WfdRtpProducerTest::SetUpTestCase(void)
{
    producer_ = std::make_shared<MockWfdRtpProducer>();
    listener_ = std::make_shared<MockProducerListener>();
}

void WfdRtpProducerTest::TearDownTestCase(void)
{
    producer_ = nullptr;
    listener_ = nullptr;
}

void WfdRtpProducerTest::SetUp(void)
{
    producer_->SetProducerListener(listener_);
    producer_->ValidateUdpClient();
}

void WfdRtpProducerTest::TearDown(void)
{
}

HWTEST_F(WfdRtpProducerTest, SetUdpDataListener_001, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);

    producer_->udpClient = std::make_shared<WfdRtpProducer::UdpClient>(true);
    producer_->udpClient->SetUdpDataListener(producer_);
}

HWTEST_F(WfdRtpProducerTest, OnClientReadData_001, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);

    DataBuffer::Ptr buf = std::make_shared<DataBuffer>();
    char data[100];
    buf->Assign(data, 100);
    producer_->udpClient = std::make_shared<WfdRtpProducer::UdpClient>(true);
    producer_->udpClient->SetUdpDataListener(producer_);
    producer_->udpClient->OnClientReadData(0, buf);
}

HWTEST_F(WfdRtpProducerTest, OnClientConnect_001, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);

    producer_->udpClient = std::make_shared<WfdRtpProducer::UdpClient>(true);
    producer_->udpClient->OnClientConnect(true);
}

HWTEST_F(WfdRtpProducerTest, ClientSendDataBuffer_001, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);

    DataBuffer::Ptr buf = std::make_shared<DataBuffer>();
    char data[100];
    buf->Assign(data, 100);
    producer_->udpClient = std::make_shared<WfdRtpProducer::UdpClient>(true);
    producer_->udpClient->SendDataBuffer(buf);
}

HWTEST_F(WfdRtpProducerTest, OnClientWriteable_001, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);

    producer_->udpClient = std::make_shared<WfdRtpProducer::UdpClient>(true);
    producer_->udpClient->OnClientWriteable(0);
}

HWTEST_F(WfdRtpProducerTest, OnClientException_001, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);

    producer_->udpClient = std::make_shared<WfdRtpProducer::UdpClient>(true);
    producer_->udpClient->OnClientException(0);
}

HWTEST_F(WfdRtpProducerTest, OnClientClose_001, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);

    producer_->udpClient = std::make_shared<WfdRtpProducer::UdpClient>(true);
    producer_->udpClient->OnClientClose(0);
}

HWTEST_F(WfdRtpProducerTest, ClientStop_001, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);

    producer_->udpClient = std::make_shared<WfdRtpProducer::UdpClient>(true);
    producer_->udpClient->Stop();
}

HWTEST_F(WfdRtpProducerTest, ClientRelease_001, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);

    producer_->udpClient = std::make_shared<WfdRtpProducer::UdpClient>(true);
    producer_->udpClient->Release();
}

HWTEST_F(WfdRtpProducerTest, UpdateOperation_001, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnProducerNotify(_));

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = PROSUMER_INIT;
    producer_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdRtpProducerTest, UpdateOperation_002, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnProducerNotify(_));

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = PROSUMER_START;
    producer_->SetInitState(false);
    producer_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdRtpProducerTest, UpdateOperation_003, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnProducerNotify(_));

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = PROSUMER_PAUSE;
    producer_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdRtpProducerTest, UpdateOperation_004, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnProducerNotify(_));

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = PROSUMER_RESUME;
    producer_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdRtpProducerTest, UpdateOperation_005, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnProducerNotify(_));

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = PROSUMER_STOP;
    producer_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdRtpProducerTest, UpdateOperation_006, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnProducerNotify(_));

    auto statusMsg = std::make_shared<ProsumerStatusMsg>();
    statusMsg->status = PROSUMER_DESTROY;
    producer_->UpdateOperation(statusMsg);
}

HWTEST_F(WfdRtpProducerTest, PublishOneFrame_001, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);

    auto tsPacker = std::make_shared<MockRtpPack>();
    EXPECT_CALL(*tsPacker, InputFrame(_));

    auto mediaData = std::make_shared<MediaData>();
    DataBuffer::Ptr buf = std::make_shared<DataBuffer>();
    char data[100];
    buf->Assign(data, 100);
    mediaData->buff = buf;
    mediaData->mediaType = MEDIA_TYPE_AUDIO;
    producer_->tsPacker_ = tsPacker;
    producer_->SetRunningState(true);
    producer_->SetPauseState(false);
    producer_->PublishOneFrame(mediaData);
}

HWTEST_F(WfdRtpProducerTest, PublishOneFrame_002, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);

    auto tsPacker = std::make_shared<MockRtpPack>();
    EXPECT_CALL(*tsPacker, InputFrame(_));

    auto mediaData = std::make_shared<MediaData>();
    DataBuffer::Ptr buf = std::make_shared<DataBuffer>();
    char data[100];
    buf->Assign(data, 100);
    mediaData->buff = buf;
    mediaData->mediaType = MEDIA_TYPE_VIDEO;
    producer_->tsPacker_ = tsPacker;
    producer_->SetRunningState(true);
    producer_->SetPauseState(false);
    producer_->PublishOneFrame(mediaData);
}

HWTEST_F(WfdRtpProducerTest, HandleEvent_001, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnProducerNotify(_));

    SharingEvent event;
    auto msg = std::make_shared<WfdProducerEventMsg>();
    msg->type = EVENT_WFD_MEDIA_INIT;
    event.eventMsg = msg;
    producer_->HandleEvent(event);
}

HWTEST_F(WfdRtpProducerTest, SendDataBuffer_001, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);

    DataBuffer::Ptr buf = std::make_shared<DataBuffer>();
    char data[100];
    buf->Assign(data, 100);
    producer_->SendDataBuffer(buf, false);
}

HWTEST_F(WfdRtpProducerTest, Connect_001, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);

    producer_->tsUdpClient_ = nullptr;
    producer_->Connect();
}

HWTEST_F(WfdRtpProducerTest, OnRtcpReadData_001, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);

    DataBuffer::Ptr buf = std::make_shared<DataBuffer>();
    char data[100];
    buf->Assign(data, 100);
    producer_->OnRtcpReadData(0, buf);
}

HWTEST_F(WfdRtpProducerTest, OnRtcpTimeOut_001, TestSize.Level1)
{
    ASSERT_TRUE(producer_ != nullptr);
    ASSERT_TRUE(listener_ != nullptr);

    EXPECT_CALL(*listener_, OnProducerNotify(_));

    producer_->rtcpOvertimes_ = 3; // 3 : DEFAULT_RTCP_OVERTIMES
    producer_->OnRtcpTimeOut();
}

} // namespace Sharing
} // namespace OHOS

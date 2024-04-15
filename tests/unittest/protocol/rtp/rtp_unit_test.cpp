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

#include "rtp_unit_test.h"
#include <gtest/gtest.h>
#include <iostream>
#include "common/sharing_log.h"
#include "protocol/rtp/include/adts.h"
#include "protocol/rtp/include/rtp_codec_aac.h"
#include "protocol/rtp/include/rtp_codec_g711.h"
#include "protocol/rtp/include/rtp_codec_h264.h"
#include "protocol/rtp/include/rtp_codec_ts.h"
#include "protocol/rtp/include/rtp_factory.h"
#include "protocol/rtp/include/rtp_maker.h"
#include "protocol/rtp/include/rtp_pack.h"
#include "protocol/rtp/include/rtp_pack_impl.h"
#include "protocol/rtp/include/rtp_packet.h"
#include "protocol/rtp/include/rtp_queue.h"
#include "protocol/rtp/include/rtp_unpack_impl.h"

using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

void RtpUnitTest::SetUpTestCase() {}
void RtpUnitTest::TearDownTestCase() {}
void RtpUnitTest::SetUp() {}
void RtpUnitTest::TearDown() {}

namespace {

HWTEST_F(RtpUnitTest, RtpUnitTest_001, Function | SmallTest | Level2)
{
    const uint8_t *data = nullptr;
    size_t bytes = 0;
    auto ret = AdtsHeader::GetAacFrameLength(data, bytes);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_002, Function | SmallTest | Level2)
{
    const uint8_t data[]{1, 2, 3, 4};
    size_t bytes = sizeof(data);
    auto ret = AdtsHeader::GetAacFrameLength(data, bytes);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_003, Function | SmallTest | Level2)
{
    const uint8_t data[]{1, 2, 3, 4, 5, 6, 7, 8};
    size_t bytes = sizeof(data);
    auto ret = AdtsHeader::GetAacFrameLength(data, bytes);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_004, Function | SmallTest | Level2)
{
    const uint8_t data[]{0xFF, 0xFF, 3, 4, 5, 6, 7, 8};
    size_t bytes = sizeof(data);
    auto ret = AdtsHeader::GetAacFrameLength(data, bytes);
    EXPECT_NE(ret, 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_005, Function | SmallTest | Level2)
{
    const std::string config{"12"};
    AdtsHeader adts;
    AdtsHeader::ParseAacConfig(config, adts);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_006, Function | SmallTest | Level2)
{
    const AdtsHeader hed;
    uint8_t out[8]{0};
    AdtsHeader::DumpAdtsHeader(hed, out);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_007, Function | SmallTest | Level2)
{
    const std::string config{"12"};
    size_t length = config.length();
    uint8_t out[8]{0};
    size_t outSize = sizeof(out);
    auto ret = AdtsHeader::DumpAacConfig(config, length, out, outSize);
    EXPECT_NE(ret, 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_008, Function | SmallTest | Level2)
{
    const RtpPlaylodParam rpp;
    auto aac = std::make_shared<RtpDecoderAAC>(rpp);
    EXPECT_NE(aac, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_009, Function | SmallTest | Level2)
{
    const RtpPlaylodParam rpp;
    auto aac = std::make_shared<RtpDecoderAAC>(rpp);
    EXPECT_NE(aac, nullptr);
    auto f = [](const Frame::Ptr &frame) { (void)frame; };
    aac->SetOnFrame(f);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_010, Function | SmallTest | Level2)
{
    const RtpPlaylodParam rpp;
    auto aac = std::make_shared<RtpDecoderAAC>(rpp);
    EXPECT_NE(aac, nullptr);
    const auto rtp = std::make_shared<RtpPacket>();
    EXPECT_NE(rtp, nullptr);
    rtp->SetCapacity(sizeof(RtpHeader));
    aac->InputRtp(rtp);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_011, Function | SmallTest | Level2)
{
    const RtpPlaylodParam rpp;
    auto aac = std::make_shared<RtpDecoderAAC>(rpp);
    EXPECT_NE(aac, nullptr);
    aac->frame_->SetCapacity(sizeof(AdtsHeader));
    aac->frame_->SetSize(sizeof(AdtsHeader));
    auto ptr = aac->frame_->Data();
    ptr[0] = 0xFF;
    ptr[1] = 0xFF;
    aac->FlushData();
}

HWTEST_F(RtpUnitTest, RtpUnitTest_012, Function | SmallTest | Level2)
{
    const RtpPlaylodParam rpp;
    auto aac = std::make_shared<RtpDecoderAAC>(rpp);
    EXPECT_NE(aac, nullptr);
    auto ret = aac->ObtainFrame();
    EXPECT_NE(ret.get(), nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_013, Function | SmallTest | Level2)
{
    const RtpPlaylodParam rpp;
    auto aac = std::make_shared<RtpDecoderAAC>(rpp);
    EXPECT_NE(aac, nullptr);
    auto frame = std::make_shared<FrameImpl>(DataBuffer{sizeof(AdtsHeader)});
    EXPECT_NE(frame, nullptr);
    aac->InputFrame(frame);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_014, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint16_t seq = 0;
    auto aac = std::make_shared<RtpEncoderAAC>(ssrc, mtuSize, sampleRate, payloadType, seq);
    EXPECT_NE(aac, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_015, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint16_t seq = 0;
    auto aac = std::make_shared<RtpEncoderAAC>(ssrc, mtuSize, sampleRate, payloadType, seq);
    EXPECT_NE(aac, nullptr);
    auto f = [](const RtpPacket::Ptr &rtp) { (void)rtp; };
    aac->SetOnRtpPack(f);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_016, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint16_t seq = 0;
    auto aac = std::make_shared<RtpEncoderAAC>(ssrc, mtuSize, sampleRate, payloadType, seq);
    EXPECT_NE(aac, nullptr);
    const auto frame = std::make_shared<FrameImpl>(DataBuffer{});
    EXPECT_NE(frame, nullptr);
    aac->InputFrame(frame);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_017, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint16_t seq = 0;
    auto aac = std::make_shared<RtpEncoderAAC>(ssrc, mtuSize, sampleRate, payloadType, seq);
    EXPECT_NE(aac, nullptr);
    const void *data = nullptr;
    size_t len = 0;
    bool mark = false;
    uint32_t stamp = 0;
    aac->MakeAACRtp(data, len, mark, stamp);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_018, Function | SmallTest | Level2)
{
    auto g711 = std::make_shared<RtpDecoderG711>();
    EXPECT_NE(g711, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_019, Function | SmallTest | Level2)
{
    auto g711 = std::make_shared<RtpDecoderG711>();
    EXPECT_NE(g711, nullptr);
    const auto rtp = std::make_shared<RtpPacket>();
    EXPECT_NE(rtp, nullptr);
    rtp->SetCapacity(sizeof(RtpHeader));
    rtp->SetSize(sizeof(RtpHeader));
    g711->InputRtp(rtp);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_020, Function | SmallTest | Level2)
{
    auto g711 = std::make_shared<RtpDecoderG711>();
    EXPECT_NE(g711, nullptr);
    auto f = [](const Frame::Ptr &frame) { (void)frame; };
    g711->SetOnFrame(f);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_021, Function | SmallTest | Level2)
{
    auto g711 = std::make_shared<RtpDecoderG711>();
    EXPECT_NE(g711, nullptr);
    auto ret = g711->ObtainFrame();
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_022, Function | SmallTest | Level2)
{
    uint32_t ssrc = 1;
    uint32_t mtuSize = 1;
    uint32_t sampleRate = 1;
    uint8_t payloadType = 0;
    uint32_t channels = 1;
    uint16_t seq = 0;
    auto g711 = std::make_shared<RtpEncoderG711>(ssrc, mtuSize, sampleRate, payloadType, channels, seq);
    EXPECT_NE(g711, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_023, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint32_t channels = 1;
    uint16_t seq = 0;
    auto g711 = std::make_shared<RtpEncoderG711>(ssrc, mtuSize, sampleRate, payloadType, channels, seq);
    EXPECT_NE(g711, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_024, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint32_t channels = 1;
    uint16_t seq = 0;
    auto g711 = std::make_shared<RtpEncoderG711>(ssrc, mtuSize, sampleRate, payloadType, channels, seq);
    EXPECT_NE(g711, nullptr);
    auto f = [](const RtpPacket::Ptr &rtp) { (void)rtp; };
    g711->SetOnRtpPack(f);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_025, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint32_t channels = 1;
    uint16_t seq = 0;
    auto g711 = std::make_shared<RtpEncoderG711>(ssrc, mtuSize, sampleRate, payloadType, channels, seq);
    EXPECT_NE(g711, nullptr);
    const auto frame = std::make_shared<FrameImpl>(DataBuffer{});
    EXPECT_NE(frame, nullptr);
    g711->InputFrame(frame);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_026, Function | SmallTest | Level2)
{
    auto h264 = std::make_shared<RtpDecoderH264>();
    EXPECT_NE(h264, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_027, Function | SmallTest | Level2)
{
    auto h264 = std::make_shared<RtpDecoderH264>();
    EXPECT_NE(h264, nullptr);
    const auto rtp = std::make_shared<RtpPacket>();
    EXPECT_NE(rtp, nullptr);
    rtp->SetCapacity(sizeof(RtpHeader));
    rtp->SetSize(sizeof(RtpHeader));
    RtpHeader *head = rtp->GetHeader();
    EXPECT_NE(head, nullptr);
    head->version_ = 1;
    head->padding_ = 0;
    head->ext_ = 1;
    head->csrc_ = 2;
    head->mark_ = 1;
    head->pt_ = 1;
    head->seq_ = 12;
    head->stamp_ = 1212;
    head->ssrc_ = 33;
    head->payload_ = 16; // default
    h264->onFrame_ = [](const Frame::Ptr &frame) { (void)frame; };
    h264->InputRtp(rtp);
    head->payload_ = 24; // 24
    h264->InputRtp(rtp);
    head->payload_ = 28; // 28
    h264->InputRtp(rtp);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_028, Function | SmallTest | Level2)
{
    auto h264 = std::make_shared<RtpDecoderH264>();
    EXPECT_NE(h264, nullptr);
    auto f = [](const Frame::Ptr &frame) { (void)frame; };
    h264->SetOnFrame(f);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_029, Function | SmallTest | Level2)
{
    auto h264 = std::make_shared<RtpDecoderH264>();
    EXPECT_NE(h264, nullptr);
    auto ret = h264->ObtainFrame();
    EXPECT_NE(ret.get(), nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_030, Function | SmallTest | Level2)
{
    auto h264 = std::make_shared<RtpDecoderH264>();
    EXPECT_NE(h264, nullptr);
    const auto rtp = std::make_shared<RtpPacket>();
    EXPECT_NE(rtp, nullptr);
    rtp->SetCapacity(sizeof(RtpHeader));
    rtp->SetSize(sizeof(RtpHeader));
    auto ret = h264->UnpackStapA(rtp, nullptr, 0, 0);
    EXPECT_EQ(ret, false);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_031, Function | SmallTest | Level2)
{
    auto h264 = std::make_shared<RtpDecoderH264>();
    EXPECT_NE(h264, nullptr);
    const auto rtp = std::make_shared<RtpPacket>();
    EXPECT_NE(rtp, nullptr);
    rtp->SetCapacity(sizeof(RtpHeader));
    rtp->SetSize(sizeof(RtpHeader));
    RtpHeader *head = rtp->GetHeader();
    EXPECT_NE(head, nullptr);
    head->version_ = 1;
    head->padding_ = 0;
    head->ext_ = 1;
    head->csrc_ = 2;
    head->mark_ = 1;
    head->pt_ = 1;
    head->seq_ = 12;
    head->stamp_ = 1212;
    head->ssrc_ = 33;
    head->payload_ = 16; // default
    h264->onFrame_ = [=](const Frame::Ptr &frame) { (void)frame; };
    auto frame = rtp->GetPayload();
    int32_t length = rtp->GetPayloadSize();
    auto stamp = rtp->GetStampMS();
    auto ret = h264->UnpackSingle(rtp, frame, length, stamp);
    EXPECT_EQ(ret, false);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_032, Function | SmallTest | Level2)
{
    auto h264 = std::make_shared<RtpDecoderH264>();
    EXPECT_NE(h264, nullptr);
    const auto rtp = std::make_shared<RtpPacket>();
    EXPECT_NE(rtp, nullptr);
    rtp->SetCapacity(sizeof(RtpHeader));
    rtp->SetSize(sizeof(RtpHeader));
    RtpHeader *head = rtp->GetHeader();
    EXPECT_NE(head, nullptr);
    head->version_ = 1;
    head->padding_ = 0;
    head->ext_ = 1;
    head->csrc_ = 2;
    head->mark_ = 1;
    head->pt_ = 1;
    head->seq_ = 12;
    head->stamp_ = 1212;
    head->ssrc_ = 33;
    head->payload_ = 24;
    h264->onFrame_ = [=](const Frame::Ptr &frame) { (void)frame; };
    auto frame = rtp->GetPayload();
    int32_t length = rtp->GetPayloadSize();
    auto stamp = rtp->GetStampMS();
    auto seq = rtp->GetSeq();
    auto ret = h264->UnpackFuA(rtp, frame, length, stamp, seq);
    EXPECT_EQ(ret, false);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_033, Function | SmallTest | Level2)
{
    auto h264 = std::make_shared<RtpDecoderH264>();
    EXPECT_NE(h264, nullptr);
    const auto rtp = std::make_shared<RtpPacket>();
    EXPECT_NE(rtp, nullptr);
    rtp->SetCapacity(sizeof(RtpHeader));
    rtp->SetSize(sizeof(RtpHeader));
    const auto frame = std::make_shared<H264Frame>(DataBuffer{100});
    EXPECT_NE(frame, nullptr);
    h264->onFrame_ = [=](const Frame::Ptr &frame) { (void)frame; };
    h264->OutputFrame(rtp, frame);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_034, Function | SmallTest | Level2)
{
    uint32_t ssrc = 1;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint16_t seq = 1;
    auto h264 = std::make_shared<RtpEncoderH264>(ssrc, mtuSize, sampleRate, payloadType, seq);
    EXPECT_NE(h264, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_035, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint16_t seq = 0;
    auto h264 = std::make_shared<RtpEncoderH264>(ssrc, mtuSize, sampleRate, payloadType, seq);
    EXPECT_NE(h264, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_036, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint16_t seq = 0;
    auto h264 = std::make_shared<RtpEncoderH264>(ssrc, mtuSize, sampleRate, payloadType, seq);
    EXPECT_NE(h264, nullptr);
    auto f = [](const RtpPacket::Ptr &rtp) { (void)rtp; };
    h264->SetOnRtpPack(f);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_037, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint16_t seq = 0;
    auto h264 = std::make_shared<RtpEncoderH264>(ssrc, mtuSize, sampleRate, payloadType, seq);
    EXPECT_NE(h264, nullptr);
    auto frame = std::make_shared<FrameImpl>(DataBuffer{100});
    EXPECT_NE(frame, nullptr);
    h264->InputFrame(frame);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_038, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint16_t seq = 0;
    auto h264 = std::make_shared<RtpEncoderH264>(ssrc, mtuSize, sampleRate, payloadType, seq);
    EXPECT_NE(h264, nullptr);
    uint32_t pts = 0;
    h264->InsertConfigFrame(pts);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_039, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint16_t seq = 0;
    auto h264 = std::make_shared<RtpEncoderH264>(ssrc, mtuSize, sampleRate, payloadType, seq);
    EXPECT_NE(h264, nullptr);
    auto frame = std::make_shared<FrameImpl>(DataBuffer{100});
    EXPECT_NE(frame, nullptr);
    bool isMark = false;
    h264->onRtpPack_ = [=](const RtpPacket::Ptr &rtp) { (void)rtp; };
    auto ret = h264->InputFrame(frame, isMark);
    EXPECT_NE(ret, false);
    isMark = true;
    auto ret1 = h264->InputFrame(frame, isMark);
    EXPECT_NE(ret1, false);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_040, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint16_t seq = 0;
    auto h264 = std::make_shared<RtpEncoderH264>(ssrc, mtuSize, sampleRate, payloadType, seq);
    EXPECT_NE(h264, nullptr);
    const uint8_t *data = nullptr;
    size_t len = 0;
    uint32_t pts = 0;
    bool isMark = false;
    bool gopPos = false;
    h264->onRtpPack_ = [=](const RtpPacket::Ptr &rtp) { (void)rtp; };
    h264->PackRtp(data, len, pts, isMark, gopPos);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_041, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint16_t seq = 0;
    auto h264 = std::make_shared<RtpEncoderH264>(ssrc, mtuSize, sampleRate, payloadType, seq);
    EXPECT_NE(h264, nullptr);
    const uint8_t *data = nullptr;
    size_t len = 0;
    uint32_t pts = 0;
    bool isMark = false;
    bool gopPos = false;
    h264->onRtpPack_ = [=](const RtpPacket::Ptr &rtp) { (void)rtp; };
    h264->PackRtpFu(data, len, pts, isMark, gopPos);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_042, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint16_t seq = 0;
    auto h264 = std::make_shared<RtpEncoderH264>(ssrc, mtuSize, sampleRate, payloadType, seq);
    EXPECT_NE(h264, nullptr);
    const uint8_t *data = nullptr;
    size_t len = 0;
    uint32_t pts = 0;
    bool isMark = false;
    bool gopPos = false;
    h264->onRtpPack_ = [=](const RtpPacket::Ptr &rtp) { (void)rtp; };
    h264->PackSingle(data, len, pts, isMark, gopPos);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_043, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint16_t seq = 0;
    auto h264 = std::make_shared<RtpEncoderH264>(ssrc, mtuSize, sampleRate, payloadType, seq);
    EXPECT_NE(h264, nullptr);
    const uint8_t *data = nullptr;
    size_t len = 0;
    uint32_t pts = 0;
    bool isMark = false;
    bool gopPos = false;
    h264->onRtpPack_ = [=](const RtpPacket::Ptr &rtp) { (void)rtp; };
    h264->PackRtpStapA(data, len, pts, isMark, gopPos);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_044, Function | SmallTest | Level2)
{
    auto decoder = std::make_shared<RtpDecoderTs>();
    EXPECT_NE(decoder, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_045, Function | SmallTest | Level2)
{
    auto decoder = std::make_shared<RtpDecoderTs>();
    EXPECT_NE(decoder, nullptr);
    decoder->onFrame_ = [=](const Frame::Ptr &frame) { (void)frame; };
    const auto rtp = std::make_shared<RtpPacket>();
    EXPECT_NE(rtp, nullptr);
    rtp->Resize(100);
    decoder->InputRtp(rtp);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_046, Function | SmallTest | Level2)
{
    auto decoder = std::make_shared<RtpDecoderTs>();
    EXPECT_NE(decoder, nullptr);
    auto f = [](const Frame::Ptr &frame) { (void)frame; };
    decoder->SetOnFrame(f);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_047, Function | SmallTest | Level2)
{
    auto decoder = std::make_shared<RtpDecoderTs>();
    EXPECT_NE(decoder, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_048, Function | SmallTest | Level2)
{
    auto decoder = std::make_shared<RtpDecoderTs>();
    EXPECT_NE(decoder, nullptr);
    decoder->Release();
}

HWTEST_F(RtpUnitTest, RtpUnitTest_049, Function | SmallTest | Level2)
{
    auto decoder = std::make_shared<RtpDecoderTs>();
    EXPECT_NE(decoder, nullptr);
    const auto rtp = std::make_shared<RtpPacket>();
    EXPECT_NE(rtp, nullptr);
    rtp->Resize(sizeof(RtpHeader));
    RtpHeader *head = rtp->GetHeader();
    EXPECT_NE(head, nullptr);
    head->version_ = 1;
    head->padding_ = 0;
    head->ext_ = 1;
    head->csrc_ = 2;
    head->mark_ = 1;
    head->pt_ = 1;
    head->seq_ = 12;
    head->stamp_ = 1212;
    head->ssrc_ = 33;
    head->payload_ = 24;
    decoder->dataQueue_.emplace(rtp);
    int bufSize = sizeof(RtpHeader);
    auto buf = std::make_shared<uint8_t>(bufSize);
    auto ret = decoder->ReadPacket(buf.get(), bufSize);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_050, Function | SmallTest | Level2)
{
    void *opaque = nullptr;
    uint8_t *buf = nullptr;
    int bufSize = 0;
    auto ret = RtpDecoderTs::StaticReadPacket(opaque, buf, bufSize);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_051, Function | SmallTest | Level2)
{
    uint32_t ssrc = 1;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint16_t seq = 0;
    auto encoder = std::make_shared<RtpEncoderTs>(ssrc, mtuSize, sampleRate, payloadType, seq);
    EXPECT_NE(encoder, nullptr);
    const auto &f = [](const RtpPacket::Ptr &rtp) { (void)rtp; };
    encoder->SetOnRtpPack(f);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_052, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint16_t seq = 0;
    auto encoder = std::make_shared<RtpEncoderTs>(ssrc, mtuSize, sampleRate, payloadType, seq);
    EXPECT_NE(encoder, nullptr);
    auto frame = std::make_shared<FrameImpl>(DataBuffer{100});
    EXPECT_NE(frame, nullptr);
    encoder->InputFrame(frame);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_053, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    uint32_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t payloadType = 0;
    uint16_t seq = 0;
    auto encoder = std::make_shared<RtpEncoderTs>(ssrc, mtuSize, sampleRate, payloadType, seq);
    EXPECT_NE(encoder, nullptr);
    auto f = [](const RtpPacket::Ptr &rtp) { (void)rtp; };
    encoder->SetOnRtpPack(f);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_054, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    size_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t pt = 0;
    RtpPayloadStream ps = RtpPayloadStream::H264;
    uint32_t audioChannels = 0;
    auto ret = RtpFactory::CreateRtpPack(ssrc, mtuSize, sampleRate, pt, ps, audioChannels);
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_055, Function | SmallTest | Level2)
{
    const RtpPlaylodParam rpp;
    auto ret = RtpFactory::CreateRtpUnpack(rpp);
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_056, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    size_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t pt = 0;
    RtpPayloadStream ps = RtpPayloadStream::H264;
    uint32_t audioChannels = 1;
    auto ret = RtpFactory::CreateRtpPack(ssrc, mtuSize, sampleRate, pt, ps, audioChannels);
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_057, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    size_t mtuSize = 0;
    uint8_t payloadType = 0;
    uint32_t sampleRate = 0;
    uint16_t seq = 0;
    auto maker = std::make_shared<RtpMaker>(ssrc, mtuSize, payloadType, sampleRate, seq);
    EXPECT_NE(maker, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_058, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    size_t mtuSize = 0;
    uint8_t payloadType = 0;
    uint32_t sampleRate = 0;
    uint16_t seq = 0;
    auto maker = std::make_shared<RtpMaker>(ssrc, mtuSize, payloadType, sampleRate, seq);
    EXPECT_NE(maker, nullptr);
    auto ret = maker->GetSsrc();
    EXPECT_NE(ret, 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_059, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    size_t mtuSize = 0;
    uint8_t payloadType = 0;
    uint32_t sampleRate = 0;
    uint16_t seq = 0;
    auto maker = std::make_shared<RtpMaker>(ssrc, mtuSize, payloadType, sampleRate, seq);
    EXPECT_NE(maker, nullptr);
    auto ret = maker->GetMaxSize();
    EXPECT_NE(ret, 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_060, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    size_t mtuSize = 0;
    uint8_t payloadType = 0;
    uint32_t sampleRate = 0;
    uint16_t seq = 0;
    auto maker = std::make_shared<RtpMaker>(ssrc, mtuSize, payloadType, sampleRate, seq);
    EXPECT_NE(maker, nullptr);
    const void *data = nullptr;
    size_t len = 0;
    bool mark = false;
    uint32_t stamp = 0;
    auto ret = maker->MakeRtp(data, len, mark, stamp);
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_061, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    size_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t pt = 0;
    RtpPayloadStream ps;
    uint32_t channels = 1;
    auto rtpPack = std::make_shared<RtpPackImpl>(ssrc, mtuSize, sampleRate, pt, ps, channels);
    EXPECT_NE(rtpPack, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_062, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    size_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t pt = 0;
    RtpPayloadStream ps;
    uint32_t channels = 1;
    auto rtpPack = std::make_shared<RtpPackImpl>(ssrc, mtuSize, sampleRate, pt, ps, channels);
    EXPECT_NE(rtpPack, nullptr);
    auto f = [](const RtpPacket::Ptr &rtp) { (void)rtp; };
    rtpPack->SetOnRtpPack(f);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_063, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    size_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t pt = 0;
    RtpPayloadStream ps;
    uint32_t channels = 1;
    auto rtpPack = std::make_shared<RtpPackImpl>(ssrc, mtuSize, sampleRate, pt, ps, channels);
    EXPECT_NE(rtpPack, nullptr);
    auto frame = std::make_shared<FrameImpl>(DataBuffer{100});
    EXPECT_NE(frame, nullptr);
    rtpPack->InputFrame(frame);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_064, Function | SmallTest | Level2)
{
    uint32_t ssrc = 0;
    size_t mtuSize = 0;
    uint32_t sampleRate = 0;
    uint8_t pt = 0;
    RtpPayloadStream ps;
    uint32_t channels = 1;
    auto rtpPack = std::make_shared<RtpPackImpl>(ssrc, mtuSize, sampleRate, pt, ps, channels);
    EXPECT_NE(rtpPack, nullptr);
    rtpPack->InitEncoder();
}

HWTEST_F(RtpUnitTest, RtpUnitTest_065, Function | SmallTest | Level2)
{
    auto rtpHeader = std::make_shared<RtpHeader>();
    EXPECT_NE(rtpHeader, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_066, Function | SmallTest | Level2)
{
    auto rtpHeader = std::make_shared<RtpHeader>();
    EXPECT_NE(rtpHeader, nullptr);
    auto ret = rtpHeader->GetExtData();
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_067, Function | SmallTest | Level2)
{
    auto rtpHeader = std::make_shared<RtpHeader>();
    EXPECT_NE(rtpHeader, nullptr);
    auto ret = rtpHeader->GetCsrcData();
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_068, Function | SmallTest | Level2)
{
    auto rtpHeader = std::make_shared<RtpHeader>();
    EXPECT_NE(rtpHeader, nullptr);
    auto ret = rtpHeader->GetPayloadData();
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_069, Function | SmallTest | Level2)
{
    auto rtpHeader = std::make_shared<RtpHeader>();
    EXPECT_NE(rtpHeader, nullptr);
    auto ret = rtpHeader->GetExtReserved();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_070, Function | SmallTest | Level2)
{
    auto rtpHeader = std::make_shared<RtpHeader>();
    EXPECT_NE(rtpHeader, nullptr);
    auto ret = rtpHeader->GetExtSize();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_071, Function | SmallTest | Level2)
{
    auto rtpHeader = std::make_shared<RtpHeader>();
    EXPECT_NE(rtpHeader, nullptr);
    auto ret = rtpHeader->GetCsrcSize();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_072, Function | SmallTest | Level2)
{
    auto rtpHeader = std::make_shared<RtpHeader>();
    EXPECT_NE(rtpHeader, nullptr);
    auto ret = rtpHeader->GetPayloadSize(0);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_073, Function | SmallTest | Level2)
{
    auto rtpHeader = std::make_shared<RtpHeader>();
    EXPECT_NE(rtpHeader, nullptr);
    auto ret = rtpHeader->GetPayloadOffset();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_074, Function | SmallTest | Level2)
{
    auto rtpHeader = std::make_shared<RtpHeader>();
    EXPECT_NE(rtpHeader, nullptr);
    auto ret = rtpHeader->GetPaddingSize(0);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_075, Function | SmallTest | Level2)
{
    auto rtpPacket = std::make_shared<RtpPacket>();
    EXPECT_NE(rtpPacket, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_076, Function | SmallTest | Level2)
{
    auto rtpPacket = std::make_shared<RtpPacket>();
    EXPECT_NE(rtpPacket, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_077, Function | SmallTest | Level2)
{
    auto rtpPacket = std::make_shared<RtpPacket>();
    EXPECT_NE(rtpPacket, nullptr);
    rtpPacket->Resize(sizeof(RtpHeader));
    auto ret = rtpPacket->GetSeq();
    EXPECT_NE(ret, 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_078, Function | SmallTest | Level2)
{
    auto rtpPacket = std::make_shared<RtpPacket>();
    EXPECT_NE(rtpPacket, nullptr);
    rtpPacket->Resize(sizeof(RtpHeader));
    auto ret = rtpPacket->GetSSRC();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_079, Function | SmallTest | Level2)
{
    auto rtpPacket = std::make_shared<RtpPacket>();
    EXPECT_NE(rtpPacket, nullptr);
    rtpPacket->Resize(sizeof(RtpHeader));
    auto ret = rtpPacket->GetStamp();
    EXPECT_NE(ret, 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_080, Function | SmallTest | Level2)
{
    auto rtpPacket = std::make_shared<RtpPacket>();
    EXPECT_NE(rtpPacket, nullptr);
    rtpPacket->Resize(sizeof(RtpHeader));
    auto ret = rtpPacket->GetPayload();
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_081, Function | SmallTest | Level2)
{
    auto rtpPacket = std::make_shared<RtpPacket>();
    EXPECT_NE(rtpPacket, nullptr);
    rtpPacket->Resize(sizeof(RtpHeader));
    auto ret = rtpPacket->GetStampMS();
    EXPECT_NE(ret, 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_082, Function | SmallTest | Level2)
{
    auto rtpPacket = std::make_shared<RtpPacket>();
    EXPECT_NE(rtpPacket, nullptr);
    rtpPacket->Resize(sizeof(RtpHeader));
    auto ret = rtpPacket->GetHeader();
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_083, Function | SmallTest | Level2)
{
    auto rtpPacket = std::make_shared<RtpPacket>();
    EXPECT_NE(rtpPacket, nullptr);
    rtpPacket->Resize(sizeof(RtpHeader));
    auto ret = rtpPacket->GetPayloadSize();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_084, Function | SmallTest | Level2)
{
    int32_t sampleRate = 0;
    size_t kMax = 1024;
    size_t kMin = 16;
    auto rtpSortor = std::make_shared<RtpPacketSortor>(sampleRate, kMax, kMin);
    EXPECT_NE(rtpSortor, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_085, Function | SmallTest | Level2)
{
    int32_t sampleRate = 0;
    size_t kMax = 1024;
    size_t kMin = 16;
    auto rtpSortor = std::make_shared<RtpPacketSortor>(sampleRate, kMax, kMin);
    EXPECT_NE(rtpSortor, nullptr);
    rtpSortor->Clear();
}

HWTEST_F(RtpUnitTest, RtpUnitTest_086, Function | SmallTest | Level2)
{
    int32_t sampleRate = 0;
    size_t kMax = 1024;
    size_t kMin = 16;
    auto rtpSortor = std::make_shared<RtpPacketSortor>(sampleRate, kMax, kMin);
    EXPECT_NE(rtpSortor, nullptr);
    rtpSortor->Flush();
}

HWTEST_F(RtpUnitTest, RtpUnitTest_087, Function | SmallTest | Level2)
{
    int32_t sampleRate = 0;
    size_t kMax = 1024;
    size_t kMin = 16;
    auto rtpSortor = std::make_shared<RtpPacketSortor>(sampleRate, kMax, kMin);
    EXPECT_NE(rtpSortor, nullptr);
    auto f = [](uint16_t seq, const RtpPacket::Ptr &packet) {
        (void)seq;
        (void)packet;
    };
    rtpSortor->SetOnSort(f);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_088, Function | SmallTest | Level2)
{
    int32_t sampleRate = 0;
    size_t kMax = 1024;
    size_t kMin = 16;
    auto rtpSortor = std::make_shared<RtpPacketSortor>(sampleRate, kMax, kMin);
    EXPECT_NE(rtpSortor, nullptr);
    uint16_t seq = 0;
    auto rtpPacket = std::make_shared<RtpPacket>();
    EXPECT_NE(rtpPacket, nullptr);
    rtpPacket->Resize(sizeof(RtpHeader));
    auto f = [](uint16_t seq, const RtpPacket::Ptr &packet) {
        (void)seq;
        (void)packet;
    };
    rtpSortor->SetOnSort(f);
    rtpSortor->SortPacket(seq, rtpPacket);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_089, Function | SmallTest | Level2)
{
    int32_t sampleRate = 0;
    size_t kMax = 1024;
    size_t kMin = 16;
    auto rtpSortor = std::make_shared<RtpPacketSortor>(sampleRate, kMax, kMin);
    EXPECT_NE(rtpSortor, nullptr);
    TrackType type = TRACK_VIDEO;
    uint8_t *ptr = nullptr;
    size_t len = 0;
    auto f = [](uint16_t seq, const RtpPacket::Ptr &packet) {
        (void)seq;
        (void)packet;
    };
    rtpSortor->SetOnSort(f);
    rtpSortor->InputRtp(type, ptr, len);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_090, Function | SmallTest | Level2)
{
    int32_t sampleRate = 0;
    size_t kMax = 1024;
    size_t kMin = 16;
    auto rtpSortor = std::make_shared<RtpPacketSortor>(sampleRate, kMax, kMin);
    EXPECT_NE(rtpSortor, nullptr);
    auto f = [](uint16_t seq, const RtpPacket::Ptr &packet) {
        (void)seq;
        (void)packet;
    };
    rtpSortor->SetOnSort(f);
    auto ret = rtpSortor->GetSSRC();
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_091, Function | SmallTest | Level2)
{
    int32_t sampleRate = 0;
    size_t kMax = 1024;
    size_t kMin = 16;
    auto rtpSortor = std::make_shared<RtpPacketSortor>(sampleRate, kMax, kMin);
    EXPECT_NE(rtpSortor, nullptr);
    auto f = [](uint16_t seq, const RtpPacket::Ptr &packet) {
        (void)seq;
        (void)packet;
    };
    rtpSortor->SetOnSort(f);
    auto ret = rtpSortor->GetJitterSize();
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_092, Function | SmallTest | Level2)
{
    int32_t sampleRate = 0;
    size_t kMax = 1024;
    size_t kMin = 16;
    auto rtpSortor = std::make_shared<RtpPacketSortor>(sampleRate, kMax, kMin);
    EXPECT_NE(rtpSortor, nullptr);
    auto f = [](uint16_t seq, const RtpPacket::Ptr &packet) {
        (void)seq;
        (void)packet;
    };
    rtpSortor->SetOnSort(f);
    auto ret = rtpSortor->GetCycleCount();
    ASSERT_TRUE(ret == 0);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_093, Function | SmallTest | Level2)
{
    int32_t sampleRate = 0;
    size_t kMax = 1024;
    size_t kMin = 16;
    auto rtpSortor = std::make_shared<RtpPacketSortor>(sampleRate, kMax, kMin);
    EXPECT_NE(rtpSortor, nullptr);
    auto f = [](uint16_t seq, const RtpPacket::Ptr &packet) {
        (void)seq;
        (void)packet;
    };
    rtpSortor->SetOnSort(f);
    uint16_t seq = 0;
    auto rtpPacket = std::make_shared<RtpPacket>();
    EXPECT_NE(rtpPacket, nullptr);
    rtpPacket->Resize(sizeof(RtpHeader));
    rtpSortor->pktSortCacheMap_.emplace(seq, std::move(rtpPacket));
    rtpSortor->PopPacket();
}

HWTEST_F(RtpUnitTest, RtpUnitTest_094, Function | SmallTest | Level2)
{
    int32_t sampleRate = 0;
    size_t kMax = 1024;
    size_t kMin = 16;
    auto rtpSortor = std::make_shared<RtpPacketSortor>(sampleRate, kMax, kMin);
    EXPECT_NE(rtpSortor, nullptr);
    auto f = [](uint16_t seq, const RtpPacket::Ptr &packet) {
        (void)seq;
        (void)packet;
    };
    rtpSortor->SetOnSort(f);
    rtpSortor->SetSortSize();
}

HWTEST_F(RtpUnitTest, RtpUnitTest_095, Function | SmallTest | Level2)
{
    int32_t sampleRate = 0;
    size_t kMax = 1024;
    size_t kMin = 16;
    auto rtpSortor = std::make_shared<RtpPacketSortor>(sampleRate, kMax, kMin);
    EXPECT_NE(rtpSortor, nullptr);
    auto f = [](uint16_t seq, const RtpPacket::Ptr &packet) {
        (void)seq;
        (void)packet;
    };
    rtpSortor->SetOnSort(f);
    rtpSortor->TryPopPacket();
}

HWTEST_F(RtpUnitTest, RtpUnitTest_096, Function | SmallTest | Level2)
{
    int32_t sampleRate = 0;
    size_t kMax = 1024;
    size_t kMin = 16;
    auto rtpSortor = std::make_shared<RtpPacketSortor>(sampleRate, kMax, kMin);
    EXPECT_NE(rtpSortor, nullptr);
    auto f = [](uint16_t seq, const RtpPacket::Ptr &packet) {
        (void)seq;
        (void)packet;
    };
    rtpSortor->SetOnSort(f);
    std::map<uint16_t, RtpPacket::Ptr> rtpMap;
    auto rtp = std::make_shared<RtpPacket>();
    EXPECT_NE(rtp, nullptr);
    rtpMap[0] = rtp;
    rtpSortor->PopIterator(rtpMap.begin());
}

HWTEST_F(RtpUnitTest, RtpUnitTest_097, Function | SmallTest | Level2)
{
    auto rtpUnpack = std::make_shared<RtpUnpackImpl>();
    EXPECT_NE(rtpUnpack, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_098, Function | SmallTest | Level2)
{
    const RtpPlaylodParam rpp;
    auto rtpUnpack = std::make_shared<RtpUnpackImpl>(rpp);
    EXPECT_NE(rtpUnpack, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_099, Function | SmallTest | Level2)
{
    RtpPlaylodParam rpp;
    rpp.ps_ = RtpPayloadStream::MPEG4_GENERIC;
    auto rtpUnpack = std::make_shared<RtpUnpackImpl>(rpp);
    EXPECT_NE(rtpUnpack, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_099_001, Function | SmallTest | Level2)
{
    RtpPlaylodParam rpp;
    rpp.ps_ = RtpPayloadStream::PCMA;
    auto rtpUnpack = std::make_shared<RtpUnpackImpl>(rpp);
    EXPECT_NE(rtpUnpack, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_099_002, Function | SmallTest | Level2)
{
    RtpPlaylodParam rpp;
    rpp.ps_ = RtpPayloadStream::MPEG2_TS;
    auto rtpUnpack = std::make_shared<RtpUnpackImpl>(rpp);
    EXPECT_NE(rtpUnpack, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_099_003, Function | SmallTest | Level2)
{
    RtpPlaylodParam rpp;
    rpp.ps_ = RtpPayloadStream::MPEG2_PS;
    auto rtpUnpack = std::make_shared<RtpUnpackImpl>(rpp);
    EXPECT_NE(rtpUnpack, nullptr);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_100, Function | SmallTest | Level2)
{
    const RtpPlaylodParam rpp;
    auto rtpUnpack = std::make_shared<RtpUnpackImpl>(rpp);
    EXPECT_NE(rtpUnpack, nullptr);
    auto f = [](uint32_t, const Frame::Ptr &frame) { (void)frame; };
    rtpUnpack->SetOnRtpUnpack(f);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_101, Function | SmallTest | Level2)
{
    const RtpPlaylodParam rpp;
    auto rtpUnpack = std::make_shared<RtpUnpackImpl>(rpp);
    EXPECT_NE(rtpUnpack, nullptr);
    auto f = [](uint32_t) {};
    rtpUnpack->SetOnRtpNotify(f);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_102, Function | SmallTest | Level2)
{
    const RtpPlaylodParam rpp;
    auto rtpUnpack = std::make_shared<RtpUnpackImpl>(rpp);
    EXPECT_NE(rtpUnpack, nullptr);
    auto rtpPacket = std::make_shared<RtpPacket>();
    EXPECT_NE(rtpPacket, nullptr);
    rtpPacket->Resize(sizeof(RtpHeader));
    const char *data = reinterpret_cast<const char *>(rtpPacket->GetHeader());
    size_t len = sizeof(RtpHeader);
    rtpUnpack->ParseRtp(data, len);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_103, Function | SmallTest | Level2)
{
    const RtpPlaylodParam rpp;
    auto rtpUnpack = std::make_shared<RtpUnpackImpl>(rpp);
    EXPECT_NE(rtpUnpack, nullptr);
    int32_t pt = 0;
    auto frame = std::make_shared<FrameImpl>(DataBuffer{});
    EXPECT_NE(frame, nullptr);
    rtpUnpack->OnRtpDecode(pt, frame);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_104, Function | SmallTest | Level2)
{
    const RtpPlaylodParam rpp;
    auto rtpUnpack = std::make_shared<RtpUnpackImpl>(rpp);
    EXPECT_NE(rtpUnpack, nullptr);
    uint16_t seq = 0;
    auto rtpPacket = std::make_shared<RtpPacket>();
    EXPECT_NE(rtpPacket, nullptr);
    rtpPacket->Resize(sizeof(RtpHeader));
    rtpUnpack->OnRtpSorted(seq, rtpPacket);
}

HWTEST_F(RtpUnitTest, RtpUnitTest_105, Function | SmallTest | Level2)
{
    const RtpPlaylodParam rpp;
    auto rtpUnpack = std::make_shared<RtpUnpackImpl>(rpp);
    EXPECT_NE(rtpUnpack, nullptr);
    rtpUnpack->CreateRtpDecoder(rpp);
}

} // namespace
} // namespace Sharing
} // namespace OHOS
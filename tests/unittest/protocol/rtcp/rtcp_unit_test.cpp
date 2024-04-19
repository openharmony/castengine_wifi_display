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

#include "rtcp_unit_test.h"
#include <iostream>
#include "common/sharing_log.h"
#include "protocol/rtcp/include/rtcp_context.h"

using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

void RtcpUnitTest::SetUpTestCase() {}
void RtcpUnitTest::TearDownTestCase() {}
void RtcpUnitTest::SetUp() {}
void RtcpUnitTest::TearDown() {}

namespace {
HWTEST_F(RtcpUnitTest, RtcpSenderContext_001, Function | SmallTest | Level2)
{
    auto context = std::make_shared<RtcpSenderContext>();
    EXPECT_NE(context, nullptr);
}

HWTEST_F(RtcpUnitTest, RtcpSenderContext_002, Function | SmallTest | Level2)
{
    auto context = std::make_shared<RtcpSenderContext>();
    EXPECT_NE(context, nullptr);
    uint32_t ssrc = 0;
    uint32_t rtt = 1;
    context->rtt_[ssrc] = rtt;
    auto ret = context->GetRtt(ssrc);
    EXPECT_EQ(ret, rtt);
}

HWTEST_F(RtcpUnitTest, RtcpSenderContext_003, Function | SmallTest | Level2)
{
    auto context = std::make_shared<RtcpSenderContext>();
    EXPECT_NE(context, nullptr);
    uint32_t ssrc = 0;
    uint32_t rtt = 1;
    context->rtt_[ssrc] = rtt;
    auto ret = context->GetRtt(rtt);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RtcpUnitTest, RtcpSenderContext_004, Function | SmallTest | Level2)
{
    auto context = std::make_shared<RtcpSenderContext>();
    EXPECT_NE(context, nullptr);
    RtcpHeader head;
    head.pt_ = static_cast<uint8_t>(RtcpType::RTCP_RR);
    head.version_ = 2;
    head.padding_ = 0;
    head.reportCount_ = 0;
    context->OnRtcp(&head);
}

HWTEST_F(RtcpUnitTest, RtcpSenderContext_005, Function | SmallTest | Level2)
{
    auto context = std::make_shared<RtcpSenderContext>();
    EXPECT_NE(context, nullptr);
    RtcpHeader head;
    head.pt_ = static_cast<uint8_t>(RtcpType::RTCP_XR);
    head.version_ = 2;
    head.padding_ = 0;
    head.reportCount_ = 0;
    context->OnRtcp(&head);
}

HWTEST_F(RtcpUnitTest, RtcpSenderContext_006, Function | SmallTest | Level2)
{
    auto context = std::make_shared<RtcpSenderContext>();
    EXPECT_NE(context, nullptr);
    RtcpHeader head;
    head.pt_ = static_cast<uint8_t>(RtcpType::RTCP_XR);
    head.version_ = 2;
    head.padding_ = 0;
    head.reportCount_ = 0;
    context->OnRtcp(&head);
}

HWTEST_F(RtcpUnitTest, RtcpSenderContext_007, Function | SmallTest | Level2)
{
    auto context = std::make_shared<RtcpSenderContext>();
    EXPECT_NE(context, nullptr);
    uint32_t rtcp_ssrc = 0;
    uint32_t rtp_ssrc = 0;
    auto ret = context->CreateRtcpXRDLRR(rtcp_ssrc, rtp_ssrc);
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(RtcpUnitTest, RtcpReceiverContext_008, Function | SmallTest | Level2)
{
    auto context = std::make_shared<RtcpReceiverContext>();
    EXPECT_NE(context, nullptr);
    RtcpHeader head;
    head.pt_ = static_cast<uint8_t>(RtcpType::RTCP_SR);
    head.version_ = 2;
    head.padding_ = 0;
    head.reportCount_ = 0;
    context->OnRtcp(&head);
}

HWTEST_F(RtcpUnitTest, RtcpReceiverContext_009, Function | SmallTest | Level2)
{
    auto context = std::make_shared<RtcpReceiverContext>();
    EXPECT_NE(context, nullptr);
    RtcpHeader head;
    head.pt_ = static_cast<uint8_t>(RtcpType::RTCP_RR);
    head.version_ = 2;
    head.padding_ = 0;
    head.reportCount_ = 0;
    context->OnRtcp(&head);
}

HWTEST_F(RtcpUnitTest, RtcpReceiverContext_010, Function | SmallTest | Level2)
{
    auto context = std::make_shared<RtcpReceiverContext>();
    EXPECT_NE(context, nullptr);
    context->OnRtp(0, 0, 0, 0, 0);
}

HWTEST_F(RtcpUnitTest, RtcpReceiverContext_011, Function | SmallTest | Level2)
{
    auto context = std::make_shared<RtcpReceiverContext>();
    EXPECT_NE(context, nullptr);
    uint32_t rtcp_ssrc = 0;
    uint32_t rtp_ssrc = 0;
    auto ret = context->CreateRtcpRR(rtcp_ssrc, rtp_ssrc);
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(RtcpUnitTest, RtcpReceiverContext_012, Function | SmallTest | Level2)
{
    auto context = std::make_shared<RtcpReceiverContext>();
    EXPECT_NE(context, nullptr);
    auto ret = context->GetLost();
    EXPECT_NE(ret, 0);
}

HWTEST_F(RtcpUnitTest, RtcpReceiverContext_013, Function | SmallTest | Level2)
{
    auto context = std::make_shared<RtcpReceiverContext>();
    EXPECT_NE(context, nullptr);
    auto ret = context->GetLostInterval();
    EXPECT_EQ(ret, 1);
}

HWTEST_F(RtcpUnitTest, RtcpReceiverContext_014, Function | SmallTest | Level2)
{
    auto context = std::make_shared<RtcpReceiverContext>();
    EXPECT_NE(context, nullptr);
    auto ret = context->GetExpectedPackets();
    EXPECT_EQ(ret, 1);
}

HWTEST_F(RtcpUnitTest, RtcpReceiverContext_015, Function | SmallTest | Level2)
{
    auto context = std::make_shared<RtcpReceiverContext>();
    EXPECT_NE(context, nullptr);
    auto ret = context->GetExpectedPacketsInterval();
    EXPECT_EQ(ret, 1);
}

HWTEST_F(RtcpUnitTest, RtcpSR_016, Function | SmallTest | Level2)
{
    auto sr = std::make_shared<RtcpSR>();
    EXPECT_NE(sr, nullptr);
}

HWTEST_F(RtcpUnitTest, RtcpSR_017, Function | SmallTest | Level2)
{
    auto sr = RtcpSR::Create(1);
    EXPECT_NE(sr, nullptr);
}

HWTEST_F(RtcpUnitTest, RtcpSR_018, Function | SmallTest | Level2)
{
    auto sr = std::make_shared<RtcpSR>();
    EXPECT_NE(sr, nullptr);
    auto &ret = sr->SetSsrc(0);
    EXPECT_EQ(&ret, sr.get());
}

HWTEST_F(RtcpUnitTest, RtcpSR_019, Function | SmallTest | Level2)
{
    auto sr = std::make_shared<RtcpSR>();
    EXPECT_NE(sr, nullptr);
    auto &ret = sr->SetNtpStamp(timeval{});
    EXPECT_EQ(&ret, sr.get());
}

HWTEST_F(RtcpUnitTest, RtcpSR_020, Function | SmallTest | Level2)
{
    auto sr = std::make_shared<RtcpSR>();
    EXPECT_NE(sr, nullptr);
    auto &ret = sr->SetPacketCount(0);
    EXPECT_EQ(&ret, sr.get());
}

HWTEST_F(RtcpUnitTest, RtcpSR_021, Function | SmallTest | Level2)
{
    auto sr = std::make_shared<RtcpSR>();
    EXPECT_NE(sr, nullptr);
    auto &ret = sr->SetNtpStamp(0);
    EXPECT_EQ(&ret, sr.get());
}

HWTEST_F(RtcpUnitTest, RtcpSR_022, Function | SmallTest | Level2)
{
    auto sr = std::make_shared<RtcpSR>();
    EXPECT_NE(sr, nullptr);
    auto &ret = sr->SetSsrc(0);
    EXPECT_EQ(&ret, sr.get());
    auto ret1 = sr->GetSsrc();
    EXPECT_EQ(ret1, 0);
}

HWTEST_F(RtcpUnitTest, RtcpSR_023, Function | SmallTest | Level2)
{
    auto sr = std::make_shared<RtcpSR>();
    EXPECT_NE(sr, nullptr);
    auto &ret = sr->SetPacketCount(0);
    EXPECT_EQ(&ret, sr.get());
    auto ret1 = sr->GetPacketCount();
    EXPECT_EQ(ret1, 0);
}

HWTEST_F(RtcpUnitTest, RtcpSR_024, Function | SmallTest | Level2)
{
    auto sr = std::make_shared<RtcpSR>();
    EXPECT_NE(sr, nullptr);
    auto &ret = sr->SetNtpStamp(0);
    EXPECT_EQ(&ret, sr.get());
    auto ret1 = sr->GetNtpUnixStampMS();
    EXPECT_EQ(ret1, 0);
}

HWTEST_F(RtcpUnitTest, RtcpSR_025, Function | SmallTest | Level2)
{
    auto sr = std::make_shared<RtcpSR>();
    EXPECT_NE(sr, nullptr);
    auto &ret = sr->SetNtpStamp(timeval{});
    EXPECT_EQ(&ret, sr.get());
    auto ret1 = sr->GetNtpStamp();
    EXPECT_NE(ret1, "");
}

HWTEST_F(RtcpUnitTest, RtcpSR_026, Function | SmallTest | Level2)
{
    auto sr = std::make_shared<RtcpSR>();
    EXPECT_NE(sr, nullptr);
    auto ret = sr->GetItemList();
    EXPECT_EQ(ret.size(), 0);
}

HWTEST_F(RtcpUnitTest, RtcpRR_027, Function | SmallTest | Level2)
{
    auto rr = std::make_shared<RtcpRR>();
    EXPECT_NE(rr, nullptr);
    auto ret = rr->GetItemList();
    EXPECT_EQ(ret.size(), 0);
}

HWTEST_F(RtcpUnitTest, RtcpRR_028, Function | SmallTest | Level2)
{
    auto rr = RtcpRR::Create(0);
    EXPECT_NE(rr, nullptr);
}

HWTEST_F(RtcpUnitTest, SdesChunk_029, Function | SmallTest | Level2)
{
    auto chunk = std::make_shared<SdesChunk>();
    EXPECT_NE(chunk, nullptr);
}

HWTEST_F(RtcpUnitTest, SdesChunk_030, Function | SmallTest | Level2)
{
    auto ret = SdesChunk::MinSize();
    EXPECT_NE(ret, 0);
}

HWTEST_F(RtcpUnitTest, SdesChunk_031, Function | SmallTest | Level2)
{
    auto chunk = std::make_shared<SdesChunk>();
    EXPECT_NE(chunk, nullptr);
    auto ret = chunk->TotalBytes();
    EXPECT_NE(ret, 0);
}

HWTEST_F(RtcpUnitTest, RtcpSdes_032, Function | SmallTest | Level2)
{
    auto sdes = std::make_shared<RtcpSdes>();
    EXPECT_NE(sdes, nullptr);
}

HWTEST_F(RtcpUnitTest, RtcpSdes_033, Function | SmallTest | Level2)
{
    auto sdes = std::make_shared<RtcpSdes>();
    EXPECT_NE(sdes, nullptr);
    auto ret = sdes->GetChunkList();
    EXPECT_EQ(ret.size(), 0);
}

HWTEST_F(RtcpUnitTest, RtcpSdes_034, Function | SmallTest | Level2)
{
    const std::vector<std::string> itemText{"123"};
    auto ret = RtcpSdes::Create(itemText);
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(RtcpUnitTest, RtcpFB_035, Function | SmallTest | Level2)
{
    auto fb = std::make_shared<RtcpFB>();
    EXPECT_NE(fb, nullptr);
}

HWTEST_F(RtcpUnitTest, RtcpFB_036, Function | SmallTest | Level2)
{
    auto fb = std::make_shared<RtcpFB>();
    EXPECT_NE(fb, nullptr);
    auto ret = fb->GetFciSize();
    EXPECT_EQ(ret, -8);
}

HWTEST_F(RtcpUnitTest, RtcpFB_037, Function | SmallTest | Level2)
{
    auto fb = std::make_shared<RtcpFB>();
    EXPECT_NE(fb, nullptr);
    auto ret = fb->GetFciPtr();
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(RtcpUnitTest, RtcpFB_038, Function | SmallTest | Level2)
{
    auto ret = RtcpFB::Create(PsfbType::RTCP_PSFB_PLI);
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(RtcpUnitTest, RtcpFB_039, Function | SmallTest | Level2)
{
    auto ret = RtcpFB::Create(RtpfbType::RTCP_RTPFB_NACK);
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(RtcpUnitTest, RtcpFB_040, Function | SmallTest | Level2)
{
    auto ret = RtcpFB::CreateInner(RtcpType::RTCP_FIR, 0, nullptr, 0);
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(RtcpUnitTest, RtcpFB_041, Function | SmallTest | Level2)
{
    auto bye = std::make_shared<RtcpBye>();
    EXPECT_NE(bye, nullptr);
}

HWTEST_F(RtcpUnitTest, RtcpBye_042, Function | SmallTest | Level2)
{
    auto bye = std::make_shared<RtcpBye>();
    EXPECT_NE(bye, nullptr);
}

HWTEST_F(RtcpUnitTest, RtcpBye_043, Function | SmallTest | Level2)
{
    auto bye = std::make_shared<RtcpBye>();
    EXPECT_NE(bye, nullptr);
    bye->ssrc_[0] = 1;
    auto ret = bye->GetReason();
    EXPECT_EQ(ret, "");
}

HWTEST_F(RtcpUnitTest, RtcpBye_044, Function | SmallTest | Level2)
{
    auto bye = std::make_shared<RtcpBye>();
    EXPECT_NE(bye, nullptr);
    auto ret = bye->GetReason();
    EXPECT_EQ(ret, "");
}

HWTEST_F(RtcpUnitTest, RtcpBye_045, Function | SmallTest | Level2)
{
    auto bye = std::make_shared<RtcpBye>();
    EXPECT_NE(bye, nullptr);
    auto ret = bye->GetSSRC();
    EXPECT_EQ(ret.size(), 0);
}

HWTEST_F(RtcpUnitTest, RtcpBye_046, Function | SmallTest | Level2)
{
    const std::vector<uint32_t> ssrc{1, 2};
    const std::string reason{"1111"};
    auto ret = RtcpBye::Create(ssrc, reason);
    EXPECT_NE(ret, nullptr);
}

HWTEST_F(RtcpUnitTest, RtcpXRDLRR_047, Function | SmallTest | Level2)
{
    auto lrr = std::make_shared<RtcpXRDLRR>();
    EXPECT_NE(lrr, nullptr);
    auto ret = lrr->GetItemList();
    EXPECT_EQ(ret.size(), 0);
}

HWTEST_F(RtcpUnitTest, RtcpXRDLRR_048, Function | SmallTest | Level2)
{
    auto ret = RtcpXRDLRR::Create(0);
    EXPECT_NE(ret, nullptr);
}
} // namespace
} // namespace Sharing
} // namespace OHOS
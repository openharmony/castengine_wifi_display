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

#include "frame_unit_test.h"
#include <iostream>
#include "common/sharing_log.h"
#include "protocol/frame/aac_frame.h"
#include "protocol/frame/frame_merger.h"
#include "protocol/frame/h264_frame.h"

using namespace testing::ext;
using namespace OHOS::Sharing;

namespace OHOS {
namespace Sharing {

void FrameUnitTest::SetUpTestCase() {}
void FrameUnitTest::TearDownTestCase() {}
void FrameUnitTest::SetUp() {}
void FrameUnitTest::TearDown() {}

uint8_t aacFrame[] = {
    0xff, 0xf1, 0x50, 0x40, 0x1b, 0x5f, 0xfc, 0xde, 0x02, 0x20, 0x4c, 0x61, 0x76, 0x63, 0x35, 0x38, 0x2e, 0x34, 0x37,
    0x2e, 0x31, 0x30, 0x30, 0x20, 0x01, 0xf0, 0x6b, 0x50, 0xd3, 0x6c, 0x77, 0x16, 0x91, 0xd2, 0x25, 0x71, 0xcf, 0x1c,
    0x9c, 0x73, 0x35, 0xb4, 0xa8, 0x52, 0x85, 0x05, 0x21, 0xe1, 0x70, 0x37, 0x83, 0x6d, 0x0d, 0x90, 0x6e, 0x3d, 0x26,
    0x06, 0x44, 0x0f, 0x59, 0xf4, 0xf2, 0xb1, 0xcc, 0x69, 0xeb, 0xf5, 0xb0, 0x20, 0x01, 0x2a, 0x38, 0xfa, 0x70, 0x02,
    0x04, 0x6e, 0xcd, 0x9b, 0x20, 0x19, 0x61, 0xf0, 0xf7, 0x47, 0x52, 0xd0, 0xf3, 0xee, 0x8e, 0x3a, 0x6d, 0xf0, 0xeb,
    0x7b, 0xb3, 0x24, 0x20, 0xae, 0xc5, 0xa1, 0x9c, 0x50, 0xda, 0x3c, 0x58, 0x47, 0x75, 0x6a, 0xc9, 0xdc, 0xb0, 0x19,
    0x85, 0x20, 0x86, 0x50, 0x60, 0x42, 0x34, 0x8c, 0x0f, 0xba, 0xf1, 0xae, 0xa5, 0xa1, 0xd2, 0xf7, 0xe3, 0x26, 0xc1,
    0xca, 0x32, 0x35, 0x1b, 0x8c, 0x4c, 0x72, 0x67, 0xf2, 0x99, 0x33, 0xc0, 0x01, 0x04, 0x59, 0x54, 0xf6, 0x12, 0x05,
    0xd4, 0x67, 0xe3, 0x86, 0xc9, 0x6e, 0xb2, 0xe6, 0xb9, 0xab, 0xca, 0x36, 0xf4, 0xb2, 0x54, 0xab, 0xca, 0xc4, 0xd9,
    0xe7, 0x5e, 0xce, 0xc1, 0xbf, 0xa1, 0x64, 0xd2, 0x94, 0x41, 0x97, 0x7d, 0x12, 0xa7, 0xf5, 0x53, 0x5a, 0xc2, 0x97,
    0x6c, 0x5b, 0x9b, 0x39, 0x40, 0x15, 0xe7, 0x55, 0xf1, 0x96, 0xc9, 0x3c, 0xb3, 0x83, 0x08, 0xd7, 0xe5, 0xca, 0x33,
    0xc3, 0xe1, 0xf9, 0xf4, 0x26, 0x26, 0x24, 0x20, 0xe0, 0xff, 0xf1, 0x50, 0x40, 0x18, 0x5f, 0xfc, 0x01, 0x08, 0x33,
    0x9b, 0x28, 0xc3, 0x13, 0xef, 0x27, 0x53, 0xc4, 0x07, 0x1b, 0xb9, 0xbb, 0x84, 0xac, 0x55, 0x2b, 0x99, 0x8a, 0x99,
    0xad, 0x0d, 0x42, 0x40, 0x02, 0x04, 0x4e, 0x84, 0x96, 0x8d, 0x17, 0x43, 0x6e, 0x70, 0x07, 0x60, 0x13, 0x8d, 0x34,
    0x08, 0x85, 0x07, 0xb6, 0xb3, 0xa4, 0x1d, 0x95, 0x45, 0x24, 0xeb, 0x69, 0x6a, 0x87, 0x2b, 0xab, 0x20, 0x13, 0x49,
    0x46, 0x5c, 0x24, 0x59, 0x22, 0x6b, 0xf1, 0x80, 0xa2, 0xc1, 0xba, 0xfe, 0xbb, 0xaa, 0x98, 0x7c, 0x0f, 0x04, 0x25,
    0x99, 0xe9, 0x03, 0x9e, 0x92, 0x1c, 0x50, 0xc2, 0xfb, 0xda, 0xdf, 0x93, 0xea, 0xde, 0x61, 0xb1, 0x6f, 0x8b, 0x47,
    0xe4, 0xe9, 0x27, 0x8f, 0x9f, 0x86, 0xe4, 0x23, 0x99, 0x5e, 0xf2, 0x33, 0xad, 0x65, 0x16, 0x3d, 0xd0, 0xea, 0x19,
    0x20, 0x51, 0x23, 0x34, 0x36, 0x2d, 0xa8, 0xdc, 0x8c, 0xf7, 0x63, 0x29, 0x77, 0x1d, 0xcf, 0x81, 0xc8, 0xaf, 0x6b,
    0x8d, 0xa1, 0x63, 0xbf, 0xa5, 0x1a, 0xe7, 0x2e, 0x40, 0x7a, 0x1d, 0x95, 0x6b, 0xb7, 0xbc, 0xf7, 0x7c, 0x05, 0x56,
    0xed, 0xa6, 0x32, 0xd0, 0xa4, 0xa2, 0xeb, 0x25, 0xd8, 0x79, 0xc5, 0xc5, 0x7a, 0xec, 0xdf, 0x22, 0x23, 0x94, 0x54,
    0x0b};

namespace {
HWTEST_F(FrameUnitTest, AACFrame_001, Function | SmallTest | Level2)
{
    auto frame = std::make_shared<AACFrame>();
    EXPECT_NE(frame, nullptr);
}

HWTEST_F(FrameUnitTest, AACFrame_002, Function | SmallTest | Level2)
{
    auto frame = std::make_shared<AACFrame>(aacFrame, sizeof(aacFrame), 0, 0, 0);
    EXPECT_NE(frame, nullptr);
}

HWTEST_F(FrameUnitTest, FrameMerger_001, Function | SmallTest | Level2)
{
    auto merger = std::make_shared<FrameMerger>();
    EXPECT_NE(merger, nullptr);
}

HWTEST_F(FrameUnitTest, FrameMerger_002, Function | SmallTest | Level2)
{
    auto merger = std::make_shared<FrameMerger>();
    EXPECT_NE(merger, nullptr);
    merger->Clear();
}

HWTEST_F(FrameUnitTest, FrameMerger_003, Function | SmallTest | Level2)
{
    auto merger = std::make_shared<FrameMerger>();
    EXPECT_NE(merger, nullptr);
    merger->SetType(0);
}

HWTEST_F(FrameUnitTest, FrameMerger_004, Function | SmallTest | Level2)
{
    auto merger = std::make_shared<FrameMerger>();
    EXPECT_NE(merger, nullptr);
    merger->SetType(0);
}

HWTEST_F(FrameUnitTest, FrameMerger_005, Function | SmallTest | Level2)
{
    auto merger = std::make_shared<FrameMerger>();
    EXPECT_NE(merger, nullptr);
    auto frame = std::make_shared<FrameImpl>();
    EXPECT_NE(frame, nullptr);
    auto buffer = std::make_shared<DataBuffer>();
    EXPECT_NE(buffer, nullptr);
    auto output = [](uint32_t dts, uint32_t pts, const DataBuffer::Ptr &buffer, bool haveKeyFrame) {
        (void)dts;
        (void)pts;
        (void)buffer;
        (void)haveKeyFrame;
    };
    auto ret = merger->InputFrame(frame, buffer, output);
    EXPECT_EQ(ret, true);
}

HWTEST_F(FrameUnitTest, FrameMerger_006, Function | SmallTest | Level2)
{
    auto merger = std::make_shared<FrameMerger>();
    EXPECT_NE(merger, nullptr);
    auto frame = std::make_shared<FrameImpl>();
    EXPECT_NE(frame, nullptr);
    auto ret = merger->WillFlush(frame);
    EXPECT_EQ(ret, false);
}

HWTEST_F(FrameUnitTest, FrameMerger_007, Function | SmallTest | Level2)
{
    auto merger = std::make_shared<FrameMerger>();
    EXPECT_NE(merger, nullptr);
    auto frame = std::make_shared<FrameImpl>();
    EXPECT_NE(frame, nullptr);
    auto buffer = std::make_shared<DataBuffer>();
    EXPECT_NE(buffer, nullptr);
    merger->DoMerge(buffer, frame);
}

HWTEST_F(FrameUnitTest, FrameImpl_001, Function | SmallTest | Level2)
{
    auto frame = FrameImpl::Create();
    EXPECT_NE(frame, nullptr);
}

HWTEST_F(FrameUnitTest, FrameImpl_002, Function | SmallTest | Level2)
{
    auto frame = std::make_shared<FrameImpl>(DataBuffer{});
    EXPECT_NE(frame, nullptr);
}

HWTEST_F(FrameUnitTest, FrameImpl_003, Function | SmallTest | Level2)
{
    auto frame = FrameImpl::CreateFrom(DataBuffer{});
    EXPECT_NE(frame, nullptr);
}

HWTEST_F(FrameUnitTest, FrameImpl_004, Function | SmallTest | Level2)
{
    auto frame = FrameImpl::CreateFrom(DataBuffer{});
    EXPECT_NE(frame, nullptr);
    auto dts = frame->Dts();
    EXPECT_EQ(dts, 0);
}

HWTEST_F(FrameUnitTest, FrameImpl_005, Function | SmallTest | Level2)
{
    auto frame = FrameImpl::CreateFrom(DataBuffer{});
    EXPECT_NE(frame, nullptr);
    auto size = frame->PrefixSize();
    EXPECT_EQ(size, 0);
}

HWTEST_F(FrameUnitTest, FrameImpl_006, Function | SmallTest | Level2)
{
    auto frame = FrameImpl::CreateFrom(DataBuffer{});
    EXPECT_NE(frame, nullptr);
    auto codecId = frame->GetCodecId();
    EXPECT_EQ(codecId, CODEC_NONE);
}

HWTEST_F(FrameUnitTest, FrameImpl_007, Function | SmallTest | Level2)
{
    auto frame = FrameImpl::CreateFrom(DataBuffer{});
    EXPECT_NE(frame, nullptr);
    auto ret = frame->KeyFrame();
    EXPECT_EQ(ret, false);
}

HWTEST_F(FrameUnitTest, FrameImpl_008, Function | SmallTest | Level2)
{
    auto frame = FrameImpl::CreateFrom(DataBuffer{});
    EXPECT_NE(frame, nullptr);
    auto ret = frame->ConfigFrame();
    EXPECT_EQ(ret, false);
}

HWTEST_F(FrameUnitTest, FrameImpl_009, Function | SmallTest | Level2)
{
    auto frame = FrameImpl::CreateFrom(DataBuffer{});
    EXPECT_NE(frame, nullptr);
    auto ret = frame->Pts();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(FrameUnitTest, FrameImpl_010, Function | SmallTest | Level2)
{
    auto frame = FrameImpl::CreateFrom(DataBuffer{});
    EXPECT_NE(frame, nullptr);
    auto ret = frame->CacheAble();
    EXPECT_EQ(ret, true);
}

HWTEST_F(FrameUnitTest, FrameImpl_011, Function | SmallTest | Level2)
{
    auto frame = FrameImpl::CreateFrom(DataBuffer{});
    EXPECT_NE(frame, nullptr);
    auto ret = frame->DecodeAble();
    EXPECT_EQ(ret, true);
}

HWTEST_F(FrameUnitTest, FrameImpl_012, Function | SmallTest | Level2)
{
    auto frame = FrameImpl::CreateFrom(DataBuffer{});
    EXPECT_NE(frame, nullptr);
    frame->codecId_ = CODEC_DEFAULT;
    auto ret = frame->DecodeAble();
    EXPECT_EQ(ret, true);
}

HWTEST_F(FrameUnitTest, H264Frame_001, Function | SmallTest | Level2)
{
    auto frame = std::make_shared<H264Frame>(DataBuffer{});
    EXPECT_NE(frame, nullptr);
}

HWTEST_F(FrameUnitTest, H264Frame_002, Function | SmallTest | Level2)
{
    auto frame = std::make_shared<H264Frame>();
    EXPECT_NE(frame, nullptr);
}

HWTEST_F(FrameUnitTest, H264Frame_003, Function | SmallTest | Level2)
{
    auto frame = std::make_shared<H264Frame>(aacFrame, sizeof(aacFrame) / sizeof(uint8_t), 0);
    EXPECT_NE(frame, nullptr);
}

HWTEST_F(FrameUnitTest, H264Frame_004, Function | SmallTest | Level2)
{
    auto frame = std::make_shared<H264Frame>(aacFrame, sizeof(aacFrame) / sizeof(uint8_t), 0);
    EXPECT_NE(frame, nullptr);
    auto ret = frame->KeyFrame();
    EXPECT_NE(ret, true);
}

HWTEST_F(FrameUnitTest, H264Frame_005, Function | SmallTest | Level2)
{
    auto frame = std::make_shared<H264Frame>(aacFrame, sizeof(aacFrame) / sizeof(uint8_t), 0);
    EXPECT_NE(frame, nullptr);
    auto ret = frame->ConfigFrame();
    EXPECT_NE(ret, true);
}

HWTEST_F(FrameUnitTest, H264Frame_006, Function | SmallTest | Level2)
{
    auto frame = std::make_shared<H264Frame>(aacFrame, sizeof(aacFrame) / sizeof(uint8_t), 0);
    EXPECT_NE(frame, nullptr);
    auto ret = frame->DropAble();
    EXPECT_NE(ret, true);
}

HWTEST_F(FrameUnitTest, H264Frame_007, Function | SmallTest | Level2)
{
    auto frame = std::make_shared<H264Frame>(aacFrame, sizeof(aacFrame) / sizeof(uint8_t), 0);
    EXPECT_NE(frame, nullptr);
    auto ret = frame->DecodeAble();
    EXPECT_NE(ret, true);
}

} // namespace
} // namespace Sharing
} // namespace OHOS
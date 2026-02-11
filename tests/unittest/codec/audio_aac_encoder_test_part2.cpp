/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#include "audio_aac_encoder_test.h"
#include "common/const_def.h"

namespace OHOS {
namespace Sharing {

using ::testing::_;
using ::testing::Return;
using ::testing::DoAll;
using ::testing::SetArrayArgument;

// TC_ENC_011: Init - av_malloc失败测试
TEST_F(AudioAACEncoderTest, Init_AvMallocFailedTest)
{
    // 设置成功的调用链直到av_malloc
    EXPECT_CALL(*mockCodec_, avcodec_find_encoder(AV_CODEC_ID_AAC))
        .WillOnce(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillOnce(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillOnce(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_frame_alloc())
        .WillOnce(Return(reinterpret_cast<AVFrame*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, av_frame_get_buffer(testing::_, testing::_))
        .WillOnce(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_packet_alloc())
        .WillOnce(Return(reinterpret_cast<AVPacket*>(0x1)));
    
    EXPECT_CALL(*mockUtil_, av_audio_fifo_alloc(testing::_, testing::_, testing::_))
        .WillOnce(reinterpret_cast<AVAudioFifo*>(0x1));
    
    EXPECT_CALL(*mockUtil_, av_samples_alloc(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(Return(0));
    
    // 设置av_malloc返回nullptr
    EXPECT_CALL(*mockCodec_, av_malloc(testing::_))
        .WillOnce(Return(nullptr));
    
    EXPECT_CALL(*mockUtil_, av_samples_free(testing::_, testing::_))
        .Times(1);
    
    EXPECT_CALL(*mockCodec_, av_frame_free(testing::_))
        .Times(1);
    
    EXPECT_CALL(*mockCodec_, avcodec_free_context(testing::_))
        .Times(1);
    
    EXPECT_CALL(*mockCodec_, av_packet_free(testing::_))
        .Times(1);
    
    EXPECT_CALL(*mockUtil_, av_audio_fifo_free(testing::_))
        .Times(1);
    
    int32_t result = encoder_->Init(2, 16, 44100);
    EXPECT_EQ(result, 1);
}

// TC_ENC_012: InitSwr - 立体声测试
TEST_F(AudioAACEncoderTest, InitSwr_StereoTest)
{
    SetupAudioConfig(2, 16, 44100); // 2 双通道 16 采样 44100立体声的构造参数
    
    auto frame = CreateTestFrame(nullptr, 0);
    EXPECT_NO_THROW({
        encoder_->OnFrame(frame);
    });
}

// TC_ENC_013: InitSwr - 单声道测试
TEST_F(AudioAACEncoderTest, InitSwr_MonoTest)
{
    // 重新设置mock以模拟单声道
    EXPECT_CALL(*mockCodec_, avcodec_find_encoder(AV_CODEC_ID_AAC))
        .WillRepeatedly(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillRepeatedly(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_frame_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVFrame*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, av_frame_get_buffer(testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_packet_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVPacket*>(0x1)));
    
    EXPECT_CALL(*mockUtil_, av_audio_fifo_alloc(testing::_, testing::_, testing::_))
        .WillRepeatedly(reinterpret_cast<AVAudioFifo*>(0x1));
    
    EXPECT_CALL(*mockUtil_, av_samples_alloc(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_malloc(testing::_))
        .WillRepeatedly(reinterpret_cast<uint8_t*>(0x1));
    
    // 测试单声道
    int32_t result = encoder_->Init(1, 16, 44100);
    EXPECT_EQ(result, 0);
    
    auto frame = CreateTestFrame(nullptr, 0);
    EXPECT_NO_THROW({
        encoder_->OnFrame(frame);
    });
}

// TC_ENC_014: InitSwr - U8格式测试
TEST_F(AudioAACEncoderTest, InitSwr_U8FormatTest)
{
    EXPECT_CALL(*mockCodec_, avcodec_find_encoder(AV_CODEC_ID_AAC))
        .WillRepeatedly(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillRepeatedly(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_frame_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVFrame*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, av_frame_get_buffer(testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_packet_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVPacket*>(0x1)));
    
    EXPECT_CALL(*mockUtil_, av_audio_fifo_alloc(testing::_, testing::_, testing::_))
        .WillRepeatedly(reinterpret_cast<AVAudioFifo*>(0x1));
    
    EXPECT_CALL(*mockUtil_, av_samples_alloc(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_malloc(testing::_))
        .WillRepeatedly(reinterpret_cast<uint8_t*>(0x1));
    
    // 测试U8格式
    int32_t result = encoder_->Init(2, 8, 44100);
    EXPECT_EQ(result, 0);
    
    auto frame = CreateTestFrame(nullptr, 0);
    EXPECT_NO_THROW({
        encoder_->OnFrame(frame);
    });
}

// TC_ENC_015: InitSwr - swr_alloc_set_opts2失败测试
TEST_F(AudioAACEncoderTest, InitSwr_AllocFailedTest)
{
    // 首先确保初始化成功
    EXPECT_CALL(*mockCodec_, avcodec_find_encoder(AV_CODEC_ID_AAC))
        .WillRepeatedly(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillRepeatedly(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_frame_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVFrame*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, av_frame_get_buffer(testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_packet_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVPacket*>(0x1)));
    
    EXPECT_CALL(*mockUtil_, av_audio_fifo_alloc(testing::_, testing::_, testing::_))
        .WillRepeatedly(reinterpret_cast<AVAudioFifo*>(0x1));
    
    EXPECT_CALL(*mockUtil_, av_samples_alloc(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_malloc(testing::_))
        .WillRepeatedly(reinterpret_cast<uint8_t*>(0x1));
    
    // 设置swr_alloc_set_opts2返回nullptr
    EXPECT_CALL(*mockSwr_, swr_alloc_set_opts2(testing::_, testing::_, testing::_, testing::_,
        testing::_, testing::_, testing::_, testing::_))
        .WillOnce(Return(nullptr));
    
    auto frame = CreateTestFrame(nullptr, 0);
    EXPECT_NO_THROW({
        encoder_->OnFrame(frame);
    });
}

// TC_ENC_016: InitSwr - swr_init失败测试
TEST_F(AudioAACEncoderTest, InitSwr_InitFailedTest)
{
    // 首先确保初始化成功
    EXPECT_CALL(*mockCodec_, avcodec_find_encoder(AV_CODEC_ID_AAC))
        .WillRepeatedly(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillRepeatedly(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_frame_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVFrame*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, av_frame_get_buffer(testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_packet_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVPacket*>(0x1)));
    
    EXPECT_CALL(*mockUtil_, av_audio_fifo_alloc(testing::_, testing::_, testing::_))
        .WillRepeatedly(reinterpret_cast<AVAudioFifo*>(0x1));
    
    EXPECT_CALL(*mockUtil_, av_samples_alloc(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_malloc(testing::_))
        .WillRepeatedly(reinterpret_cast<uint8_t*>(0x1));
    
    // 设置swr_alloc_set_opts2成功
    EXPECT_CALL(*mockSwr_, swr_alloc_set_opts2(testing::_, testing::_, testing::_, testing::_,
        testing::_, testing::_, testing::_, testing::_))
        .WillOnce(Return(reinterpret_cast<SwrContext*>(0x1)));
    
    // 设置swr_init失败，返回-1
    EXPECT_CALL(*mockSwr_, swr_init(testing::_))
        .WillOnce(Return(-1));
    
    auto frame = CreateTestFrame(nullptr, 0);
    EXPECT_NO_THROW({
        encoder_->OnFrame(frame);
    });
}

// TC_ENC_017: InitSwr - swrData分配失败测试
TEST_F(AudioAACEncoderTest, InitSwr_SwrDataAllocFailedTest)
{
    // 首先确保初始化成功
    EXPECT_CALL(*mockCodec_, avcodec_find_encoder(AV_CODEC_ID_AAC))
        .WillRepeatedly(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillRepeatedly(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_frame_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVFrame*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, av_frame_get_buffer(testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_packet_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVPacket*>(0x1)));
    
    EXPECT_CALL(*mockUtil_, av_audio_fifo_alloc(testing::_, testing::_, testing::_))
        .WillRepeatedly(reinterpret_cast<AVAudioFifo*>(0x1));
    
    EXPECT_CALL(*mockUtil_, av_samples_alloc(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_malloc(testing::_))
        .WillRepeatedly(reinterpret_cast<uint8_t*>(0x1));
    
    // 设置swr_alloc_set_opts2成功
    EXPECT_CALL(*mockSwr_, swr_alloc_set_opts2(testing::_, testing::_, testing::_, testing::_,
        testing::_, testing::_, testing::_, testing::_))
        .WillOnce(Return(reinterpret_cast<SwrContext*>(0x1)));
    
    EXPECT_CALL(*mockSwr_, swr_init(testing::_))
        .WillOnce(Return(0));
    
    // 模拟calloc失败
    EXPECT_CALL(*mockCodec_, av_freep(testing::_))
        .Times(1);
    
    auto frame = CreateTestFrame(nullptr, 0);
    EXPECT_NO_THROW({
        encoder_->OnFrame(frame);
    });
}

// TC_ENC_018: InitSwr - av_samples_alloc失败测试
TEST_F(AudioAACEncoderTest, InitSwr_SamplesAllocFailedTest)
{
    // 首先确保初始化成功
    EXPECT_CALL(*mockCodec_, avcodec_find_encoder(AV_CODEC_ID_AAC))
        .WillRepeatedly(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillRepeatedly(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_frame_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVFrame*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, av_frame_get_buffer(testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_packet_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVPacket*>(0x1)));
    
    EXPECT_CALL(*mockUtil_, av_audio_fifo_alloc(testing::_, testing::_, testing::_))
        .WillRepeatedly(reinterpret_cast<AVAudioFifo*>(0x1));
    
    EXPECT_CALL(*mockUtil_, av_samples_alloc(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillRepeatedly(Return(-1));
    
    EXPECT_CALL(*mockCodec_, av_malloc(testing::_))
        .WillRepeatedly(reinterpret_cast<uint8_t*>(0x1));
    
    // 设置swr_alloc_set_opts2成功
    EXPECT_CALL(*mockSwr_, swr_alloc_set_opts2(testing::_, testing::_, testing::_, testing::_,
        testing::_, testing::_, testing::_, testing::_))
        .WillOnce(Return(reinterpret_cast<SwrContext*>(0x1)));
    
    EXPECT_CALL(*mockSwr_, swr_init(testing::_))
        .WillOnce(Return(0));
    
    auto frame = CreateTestFrame(nullptr, 0);
    EXPECT_NO_THROW({
        encoder_->OnFrame(frame);
    });
}

// TC_ENC_019: OnFrame - frame为nullptr测试
TEST_F(AudioAACEncoderTest, OnFrameNullFrameTest)
{
    // 不应该有任何FFmpeg调用
    EXPECT_CALL(*mockCodec_, _).Times(0);
    EXPECT_CALL(*mockSwr_, _).Times(0);
    EXPECT_CALL(*mockUtil_, _).Times(0);
    
    encoder_->OnFrame(nullptr);
}

// TC_ENC_020: OnFrame - swr初始化失败测试
TEST_F(AudioAACEncoderTest, OnFrame_SwrInitFailedTest)
{
    // 首先确保初始化成功
    EXPECT_CALL(*mockCodec_, avcodec_find_encoder(AV_CODEC_ID_AAC))
        .WillRepeatedly(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillRepeatedly(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_frame_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVFrame*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, av_frame_get_buffer(testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_packet_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVPacket*>(0x1)));
    
    EXPECT_CALL(*mockUtil_, av_audio_fifo_alloc(testing::_, testing::_, testing::_))
        .WillRepeatedly(reinterpret_cast<AVAudioFifo*>(0x1));
    
    EXPECT_CALL(*mockUtil_, av_samples_alloc(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_malloc(testing::_))
        .WillRepeatedly(reinterpret_cast<uint8_t*>(0x1));
    
    // 设置swr_alloc_set_opts2成功
    EXPECT_CALL(*mockSwr_, swr_alloc_set_opts2(testing::_, testing::_, testing::_, testing::_,
        testing::_, testing::_, testing::_, testing::_))
        .WillOnce(Return(reinterpret_cast<SwrContext*>(0x1)));
    
    // 设置swr_init失败
    EXPECT_CALL(*mockSwr_, swr_init(testing::_))
        .WillOnce(Return(-1));
    
    auto frame = CreateTestFrame(nullptr, 0);
    EXPECT_NO_THROW({
        encoder_->OnFrame(frame);
    });
}

} // namespace Sharing
} // namespace OHOS
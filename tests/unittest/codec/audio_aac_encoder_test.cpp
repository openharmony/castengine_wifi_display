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

// TC_ENC_001: 构造函数测试
TEST_F(AudioAACEncoderTest, ConstructorTest)
{
    // 测试构造函数不会抛出异常
    EXPECT_NO_THROW({
        AudioAACEncoder encoder;
    });
}

// TC_ENC_002: 析构函数测试
TEST_F(AudioAACEncoderTest, DestructorTest)
{
    EXPECT_NO_THROW({
        AudioAACEncoder* encoder = new AudioAACEncoder();
        delete encoder;
    });
}

// TC_ENC_003: Init正常流程测试（mono/stereo, U8/S16）
TEST_F(AudioAACEncoderTest, InitSuccessTest)
{
    // 测试立体声，16位，44100Hz
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
    
    EXPECT_CALL(*mockCodec_, av_malloc(testing::_))
        .WillOnce(reinterpret_cast<uint8_t*>(0x1));
    
    int32_t result = encoder_->Init(2, 16, 44100);
    EXPECT_EQ(result, 0);
}

// TC_ENC_004: Init - avcodec_find_encoder失败测试
TEST_F(AudioAACEncoderTest, Init_FindEncoderFailedTest)
{
    // 设置avcodec_find_encoder返回nullptr
    EXPECT_CALL(*mockCodec_, avcodec_find_encoder(AV_CODEC_ID_AAC))
        .WillOnce(Return(nullptr));
    
    int32_t result = encoder_->Init(2, 16, 44100);
    EXPECT_EQ(result, 1);
}

// TC_ENC_005: Init - avcodec_alloc_context3失败测试
TEST_F(AudioAACEncoderTest, Init_AllocContextFailedTest)
{
    // 设置avcodec_find_encoder成功
    EXPECT_CALL(*mockCodec_, avcodec_find_encoder(AV_CODEC_ID_AAC))
        .WillOnce(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    // 设置avcodec_alloc_context3返回nullptr
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillOnce(Return(nullptr));
    
    int32_t result = encoder_->Init(2, 16, 44100);
    EXPECT_EQ(result, 1);
}

// TC_ENC_006: Init - avcodec_open2失败测试
TEST_F(AudioAACEncoderTest, Init_OpenCodecFailedTest)
{
    // 设置成功的调用链直到avcodec_open2
    EXPECT_CALL(*mockCodec_, avcodec_find_encoder(AV_CODEC_ID_AAC))
        .WillOnce(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillOnce(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    // 设置avcodec_open2失败，返回-1
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillOnce(Return(-1));
    
    EXPECT_CALL(*mockCodec_, avcodec_free_context(testing::_))
        .Times(1);
    
    int32_t result = encoder_->Init(2, 16, 44100);
    EXPECT_EQ(result, 1);
}

// TC_ENC_007: Init - av_frame_alloc失败测试
TEST_F(AudioAACEncoderTest, Init_FrameAllocFailedTest)
{
    // 设置成功的调用链直到av_frame_alloc
    EXPECT_CALL(*mockCodec_, avcodec_find_encoder(AV_CODEC_ID_AAC))
        .WillOnce(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillOnce(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillOnce(Return(0));
    
    // 设置av_frame_alloc返回nullptr
    EXPECT_CALL(*mockCodec_, av_frame_alloc())
        .WillOnce(Return(nullptr));
    
    EXPECT_CALL(*mockCodec_, avcodec_free_context(testing::_))
        .Times(1);
    
    int32_t result = encoder_->Init(2, 16, 44100);
    EXPECT_EQ(result, 1);
}

// TC_ENC_008: Init - av_frame_get_buffer失败测试
TEST_F(AudioAACEncoderTest, Init_FrameGetBufferFailedTest)
{
    // 设置成功的调用链直到av_frame_get_buffer
    EXPECT_CALL(*mockCodec_, avcodec_find_encoder(AV_CODEC_ID_AAC))
        .WillOnce(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillOnce(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillOnce(Return(0));
    
    EXPECT_CALL(*mockCodec_, av_frame_alloc())
        .WillOnce(Return(reinterpret_cast<AVFrame*>(0x1)));
    
    // 设置av_frame_get_buffer失败，返回-1
    EXPECT_CALL(*mockCodec_, av_frame_get_buffer(testing::_, testing::_))
        .WillOnce(Return(-1));
    
    EXPECT_CALL(*mockCodec_, av_frame_free(testing::_))
        .Times(1);
    
    EXPECT_CALL(*mockCodec_, avcodec_free_context(testing::_))
        .Times(1);
    
    EXPECT_CALL(*mockCodec_, av_packet_free(testing::_))
        .Times(1);
    
    int32_t result = encoder_->Init(2, 16, 44100);
    EXPECT_EQ(result, 1);
}

// TC_ENC_009: Init - av_packet_alloc失败测试
TEST_F(AudioAACEncoderTest, Init_PacketAllocFailedTest)
{
    // 设置成功的调用链直到av_packet_alloc
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
    
    // 设置av_packet_alloc返回nullptr
    EXPECT_CALL(*mockCodec_, av_packet_alloc())
        .WillOnce(Return(nullptr));
    
    EXPECT_CALL(*mockCodec_, av_frame_free(testing::_))
        .Times(1);
    
    EXPECT_CALL(*mockCodec_, avcodec_free_context(testing::_))
        .Times(1);
    
    int32_t result = encoder_->Init(2, 16, 44100);
    EXPECT_EQ(result, 1);
}

// TC_ENC_010: Init - av_audio_fifo_alloc失败测试
TEST_F(AudioAACEncoderTest, Init_FifoAllocFailedTest)
{
    // 设置成功的调用链直到av_audio_fifo_alloc
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
    
    // 设置av_audio_fifo_alloc返回nullptr
    EXPECT_CALL(*mockUtil_, av_audio_fifo_alloc(testing::_, testing::_, testing::_))
        .WillOnce(Return(nullptr));
    
    EXPECT_CALL(*mockCodec_, av_frame_free(testing::_))
        .Times(1);
    
    EXPECT_CALL(*mockCodec_, avcodec_free_context(testing::_))
        .Times(1);
    
    EXPECT_CALL(*mockCodec_, av_packet_free(testing::_))
        .Times(1);
    
    int32_t result = encoder_->Init(2, 16, 44100);
    EXPECT_EQ(result, 1);
}

} // namespace Sharing
} // namespace OHOS
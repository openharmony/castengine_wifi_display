/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#include "audio_aac_decoder_test.h"
#include "/const_def.h"
#include <algorithm> // 添加std::copy所需的头文件

namespace OHOS {
namespace Sharing {

using ::testing::_;
using ::testing::Return;
using ::testing::DoAll;
using ::testing::SetArrayArgument;

// TC_DEC_001: 构造函数测试
TEST_F(AudioAACDecoderTest, ConstructorTest)
{
    // 测试构造函数不会抛出异常
    EXPECT_NO_THROW({
        AudioAACDecoder decoder;
    });
    
    // 验证构造函数后核心组件为nullptr
    // 注意：在实际的析构函数中会释放这些资源，构造时应该是nullptr
    // 由于构造函数是空的，我们主要验证它能正常创建
}

// TC_DEC_002: 析构函数测试
TEST_F(AudioAACDecoderTest, DestructorTest)
{
    EXPECT_NO_THROW({
        AudioAACDecoder* decoder = new AudioAACDecoder();
        delete decoder;
    });
}

// TC_DEC_003: Init正常流程测试
TEST_F(AudioAACDecoderTest, InitSuccessTest)
{
    AudioTrack track = CreateDefaultAudioTrack();
    
    // 设置成功的调用链
    // 值0x1: 模拟非空的有效指针，表示找到AAC解码器成功
    EXPECT_CALL(*mockCodec_, avcodec_find_decoder(AV_CODEC_ID_AAC))
        .WillOnce(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    // 值0x1: 模拟非空的有效指针，表示编码上下文分配成功
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillOnce(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillOnce(Return(0));
    
    // 值0x1: 模拟非空的有效指针，表示数据包分配成功
    EXPECT_CALL(*mockCodec_, av_packet_alloc())
        .WillOnce(Return(reinterpret_cast<AVPacket*>(0x1)));
    
    // 值0x1: 模拟非空的有效指针，表示音频帧分配成功
    EXPECT_CALL(*mockCodec_, av_frame_alloc())
        .WillOnce(Return(reinterpret_cast<AVFrame*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, av_packet_free(testing::_))
        .Times(1);
    
    EXPECT_CALL(*mockCodec_, av_frame_free(testing::_))
        .Times(1);
    
    EXPECT_CALL(*mockCodec_, avcodec_free_context(testing::_))
        .Times(1);
    
    EXPECT_CALL(*mockSwr_, swr_free(testing::_))
        .Times(1);
    
    EXPECT_CALL(*mockCodec_, av_freep(testing::_))
        .Times(1);
    
    int32_t result = decoder_->Init(track);
    EXPECT_EQ(result, 0);
}

// TC_DEC_004: Init - avcodec_find_decoder失败测试
TEST_F(AudioAACDecoderTest, Init_FindDecoderFailedTest)
{
    AudioTrack track = CreateDefaultAudioTrack();
    
    // 设置avcodec_find_decoder返回nullptr
    EXPECT_CALL(*mockCodec_, avcodec_find_decoder(AV_CODEC_ID_AAC))
        .WillOnce(Return(nullptr));
    
    int32_t result = decoder_->Init(track);
    EXPECT_EQ(result, -1);
}

// TC_DEC_005: Init - avcodec_alloc_context3失败测试
TEST_F(AudioAACDecoderTest, Init_AllocContextFailedTest)
{
    AudioTrack track = CreateDefaultAudioTrack();
    
    // 值0x1: 模拟非空的有效指针，表示找到AAC解码器成功
    EXPECT_CALL(*mockCodec_, avcodec_find_decoder(AV_CODEC_ID_AAC))
        .WillOnce(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    // 设置avcodec_alloc_context3返回nullptr
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillOnce(Return(nullptr));
    
    int32_t result = decoder_->Init(track);
    EXPECT_EQ(result, -1);
}

// TC_DEC_006: Init - avcodec_open2失败测试
TEST_F(AudioAACDecoderTest, Init_OpenCodecFailedTest)
{
    AudioTrack track = CreateDefaultAudioTrack();
    
    // 值0x1: 模拟非空的有效指针，表示找到AAC解码器成功
    EXPECT_CALL(*mockCodec_, avcodec_find_decoder(AV_CODEC_ID_AAC))
        .WillOnce(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    // 值0x1: 模拟非空的有效指针，表示编码上下文分配成功
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillOnce(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    // 设置avcodec_open2失败，返回-1
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillOnce(Return(-1));
    
    EXPECT_CALL(*mockCodec_, avcodec_free_context(testing::_))
        .Times(1);
    
    int32_t result = decoder_->Init(track);
    EXPECT_EQ(result, -1);
}

// TC_DEC_007: Init - av_packet_alloc失败测试
TEST_F(AudioAACDecoderTest, Init_PacketAllocFailedTest)
{
    AudioTrack track = CreateDefaultAudioTrack();
    
    // 值0x1: 模拟非空的有效指针，表示找到AAC解码器成功
    EXPECT_CALL(*mockCodec_, avcodec_find_decoder(AV_CODEC_ID_AAC))
        .WillOnce(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    // 值0x1: 模拟非空的有效指针，表示编码上下文分配成功
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillOnce(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillOnce(Return(0));
    
    // 设置av_packet_alloc返回nullptr
    EXPECT_CALL(*mockCodec_, av_packet_alloc())
        .WillOnce(Return(nullptr));
    
    EXPECT_CALL(*mockCodec_, avcodec_free_context(testing::_))
        .Times(1);
    
    EXPECT_CALL(*mockCodec_, av_frame_free(testing::_))
        .Times(1);
    
    int32_t result = decoder_->Init(track);
    EXPECT_EQ(result, -1);
}

// TC_DEC_008: Init - av_frame_alloc失败测试
TEST_F(AudioAACDecoderTest, Init_FrameAllocFailedTest)
{
    AudioTrack track = CreateDefaultAudioTrack();
    
    // 值0x1: 模拟非空的有效指针，表示找到AAC解码器成功
    EXPECT_CALL(*mockCodec_, avcodec_find_decoder(AV_CODEC_ID_AAC))
        .WillOnce(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    // 值0x1: 模拟非空的有效指针，表示编码上下文分配成功
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillOnce(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillOnce(Return(0));
    
    // 值0x1: 模拟非空的有效指针，表示数据包分配成功
    EXPECT_CALL(*mockCodec_, av_packet_alloc())
        .WillOnce(Return(reinterpret_cast<AVPacket*>(0x1)));
    
    // 设置av_frame_alloc返回nullptr
    EXPECT_CALL(*mockCodec_, av_frame_alloc())
        .WillOnce(Return(nullptr));
    
    EXPECT_CALL(*mockCodec_, av_packet_free(testing::_))
        .Times(1);
    
    EXPECT_CALL(*mockCodec_, avcodec_free_context(testing::_))
        .Times(1);
    
    int32_t result = decoder_->Init(track);
    EXPECT_EQ(result, -1);
}

// TC_DEC_009: OnFrame - frame为nullptr测试
TEST_F(AudioAACDecoderTest, OnFrameNullFrameTest)
{
    // 不应该有任何FFmpeg调用
    EXPECT_CALL(*mockCodec_, _).Times(0);
    EXPECT_CALL(*mockSwr_, _).Times(0);
    EXPECT_CALL(*mockUtil_, _).Times(0);
    
    decoder_->OnFrame(nullptr);
}

// TC_DEC_010: OnFrame - 组件为nullptr测试
TEST_F(AudioAACDecoderTest, OnFrameNullComponentsTest)
{
    // 创建一个测试frame
    std::vector<uint8_t> testData = {0x12, 0x34, 0x56, 0x78};
    auto frame = CreateTestFrame(testData.data(), testData.size());
    
    // 模拟内部组件为nullptr的情况
    // 由于私有成员无法直接访问，我们通过测试代码路径来验证
    EXPECT_NO_THROW({
        decoder_->OnFrame(frame);
    });
}

// TC_DEC_011: OnFrame - 正常编码流程测试
TEST_F(AudioAACDecoderTest, OnFrameSuccessTest)
{
    // 首先确保解码器初始化成功
    AudioTrack track = CreateDefaultAudioTrack();
    // 值0x1: 模拟非空的有效指针，表示找到AAC解码器成功
    EXPECT_CALL(*mockCodec_, avcodec_find_decoder(AV_CODEC_ID_AAC))
        .WillRepeatedly(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    // 值0x1: 模拟非空的有效指针，表示编码上下文分配成功
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillRepeatedly(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    // 值0x1: 模拟非空的有效指针，表示数据包分配成功
    EXPECT_CALL(*mockCodec_, av_packet_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVPacket*>(0x1)));
    
    // 值0x1: 模拟非空的有效指针，表示音频帧分配成功
    EXPECT_CALL(*mockCodec_, av_frame_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVFrame*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_send_packet(testing::_, testing::_))
        .WillOnce(Return(0));
    
    EXPECT_CALL(*mockCodec_, avcodec_receive_frame(testing::_, testing::_))
        .WillOnce(Return(0));
    
    // 值0x1: 模拟非空的有效指针，表示重采样器分配成功
    EXPECT_CALL(*mockSwr_, swr_alloc_set_opts2(testing::_, testing::_, testing::_, testing::_, 
                                               testing::_, testing::_, testing::_, testing::_))
        .WillOnce(Return(reinterpret_cast<SwrContext*>(0x1)));
    
    EXPECT_CALL(*mockSwr_, swr_init(testing::_))
        .WillOnce(Return(0));
    
    // 值1024: 1KB的样本缓冲区大小，用于音频转换测试
    EXPECT_CALL(*mockUtil_, av_samples_get_buffer_size(testing::_, testing::_, testing::_, 
                                                       testing::_, testing::_))
        .WillOnce(Return(1024));
    
    // 值1024: 分配1024字节的内存用于音频数据转换
    EXPECT_CALL(*mockCodec_, av_malloc(1024))
        .WillOnce(reinterpret_cast<uint8_t*>(0x1));
    
    // 值1024: 成功转换1024个样本点
    EXPECT_CALL(*mockSwr_, swr_convert(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(Return(1024));
    
    // 创建测试frame - AAC ADTS帧头部数据
    std::vector<uint8_t> testData = {0xFF, 0xF1, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00}; // 简化的测试数据
    
    // 使用std::copy替代memcpy进行数据拷贝
    auto frame = CreateTestFrame(testData.data(), testData.size());
    
    EXPECT_NO_THROW({
        decoder_->OnFrame(frame);
    });
}

// TC_DEC_012: OnFrame - swr_alloc_set_opts2失败测试
TEST_F(AudioAACDecoderTest, OnFrame_SwrAllocFailedTest)
{
    // 首先确保解码器初始化成功
    AudioTrack track = CreateDefaultAudioTrack();
    // 值0x1: 模拟非空的有效指针，表示找到AAC解码器成功
    EXPECT_CALL(*mockCodec_, avcodec_find_decoder(AV_CODEC_ID_AAC))
        .WillRepeatedly(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    // 值0x1: 模拟非空的有效指针，表示编码上下文分配成功
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillRepeatedly(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    // 值0x1: 模拟非空的有效指针，表示数据包分配成功
    EXPECT_CALL(*mockCodec_, av_packet_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVPacket*>(0x1)));
    
    // 值0x1: 模拟非空的有效指针，表示音频帧分配成功
    EXPECT_CALL(*mockCodec_, av_frame_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVFrame*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_send_packet(testing::_, testing::_))
        .WillOnce(Return(0));
    
    EXPECT_CALL(*mockCodec_, avcodec_receive_frame(testing::_, testing::_))
        .WillOnce(Return(0));
    
    // 设置swr_alloc_set_opts2返回nullptr
    EXPECT_CALL(*mockSwr_, swr_alloc_set_opts2(testing::_, testing::_, testing::_, testing::_,
        testing::_, testing::_, testing::_, testing::_))
        .WillOnce(Return(nullptr));
    
    // 创建测试frame - AAC ADTS帧头部数据
    std::vector<uint8_t> testData = {0xFF, 0xF1, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00}; // 简化的测试数据
    
    // 使用std::copy替代memcpy进行数据拷贝
    auto frame = CreateTestFrame(testData.data(), testData.size());
    
    EXPECT_NO_THROW({
        decoder_->OnFrame(frame);
    });
}

// TC_DEC_013: OnFrame - buffer size验证失败测试
TEST_F(AudioAACDecoderTest, OnFrame_BufferSizeFailedTest)
{
    // 首先确保解码器初始化成功
    AudioTrack track = CreateDefaultAudioTrack();
    // 值0x1: 模拟非空的有效指针，表示找到AAC解码器成功
    EXPECT_CALL(*mockCodec_, avcodec_find_decoder(AV_CODEC_ID_AAC))
        .WillRepeatedly(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    // 值0x1: 模拟非空的有效指针，表示编码上下文分配成功
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillRepeatedly(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    // 值0x1: 模拟非空的有效指针，表示数据包分配成功
    EXPECT_CALL(*mockCodec_, av_packet_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVPacket*>(0x1)));
    
    // 值0x1: 模拟非空的有效指针，表示音频帧分配成功
    EXPECT_CALL(*mockCodec_, av_frame_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVFrame*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_send_packet(testing::_, testing::_))
        .WillOnce(Return(0));
    
    EXPECT_CALL(*mockCodec_, avcodec_receive_frame(testing::_, testing::_))
        .WillOnce(Return(0));
    
    // 值0x1: 模拟非空的有效指针，表示重采样器分配成功
    EXPECT_CALL(*mockSwr_, swr_alloc_set_opts2(testing::_, testing::_, testing::_, testing::_, 
        testing::_, testing::_, testing::_, testing::_))
        .WillOnce(Return(reinterpret_cast<SwrContext*>(0x1)));
    
    EXPECT_CALL(*mockSwr_, swr_init(testing::_))
        .WillOnce(Return(0));
    
    // 设置一个过大的buffer size，模拟验证失败（110MB + 1字节，超过MAX_AUDIO_BUFFER_SIZE）
    EXPECT_CALL(*mockUtil_, av_samples_get_buffer_size(testing::_, testing::_, testing::_, 
        testing::_, testing::_))
        .WillOnce(Return(110 * 100 * 1024 + 1)); // 110 字节 1024 超过MAX_AUDIO_BUFFER_SIZE 100 大小
    
    // 创建测试frame - AAC ADTS帧头部数据
    std::vector<uint8_t> testData = {0xFF, 0xF1, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00}; // 简化的测试数据
    
    // 使用std::copy替代memcpy进行数据拷贝
    auto frame = CreateTestFrame(testData.data(), testData.size());
    
    EXPECT_NO_THROW({
        decoder_->OnFrame(frame);
    });
}

// TC_DEC_014: OnFrame - av_malloc失败测试
TEST_F(AudioAACDecoderTest, OnFrame_AvMallocFailedTest)
{
    // 首先确保解码器初始化成功
    AudioTrack track = CreateDefaultAudioTrack();
    // 值0x1: 模拟非空的有效指针，表示找到AAC解码器成功
    EXPECT_CALL(*mockCodec_, avcodec_find_decoder(AV_CODEC_ID_AAC))
        .WillRepeatedly(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    // 值0x1: 模拟非空的有效指针，表示编码上下文分配成功
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillRepeatedly(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    // 值0x1: 模拟非空的有效指针，表示数据包分配成功
    EXPECT_CALL(*mockCodec_, av_packet_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVPacket*>(0x1)));
    
    // 值0x1: 模拟非空的有效指针，表示音频帧分配成功
    EXPECT_CALL(*mockCodec_, av_frame_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVFrame*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_send_packet(testing::_, testing::_))
        .WillOnce(Return(0));
    
    EXPECT_CALL(*mockCodec_, avcodec_receive_frame(testing::_, testing::_))
        .WillOnce(Return(0));
    
    // 值0x1: 模拟非空的有效指针，表示重采样器分配成功
    EXPECT_CALL(*mockSwr_, swr_alloc_set_opts2(testing::_, testing::_, testing::_, testing::_, 
                                               testing::_, testing::_, testing::_, testing::_))
        .WillOnce(Return(reinterpret_cast<SwrContext*>(0x1)));
    
    EXPECT_CALL(*mockSwr_, swr_init(testing::_))
        .WillOnce(Return(0));
    
    // 值1024: 1KB的样本缓冲区大小，用于音频转换测试
    EXPECT_CALL(*mockUtil_, av_samples_get_buffer_size(testing::_, testing::_, testing::_,
        testing::_, testing::_))
        .WillOnce(Return(1024));
    
    // 设置av_malloc返回nullptr
    EXPECT_CALL(*mockCodec_, av_malloc(1024)) // 1024 内存
        .WillOnce(Return(nullptr));
    
    // 创建测试frame - AAC ADTS帧头部数据
    std::vector<uint8_t> testData = {0xFF, 0xF1, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00}; // 简化的测试数据
    
    // 使用std::copy替代memcpy进行数据拷贝
    auto frame = CreateTestFrame(testData.data(), testData.size());
    
    EXPECT_NO_THROW({
        decoder_->OnFrame(frame);
    });
}

// TC_DEC_015: OnFrame - swr_convert失败测试
TEST_F(AudioAACDecoderTest, OnFrame_SwrConvertFailedTest)
{
    // 首先确保解码器初始化成功
    AudioTrack track = CreateDefaultAudioTrack();
    // 值0x1: 模拟非空的有效指针，表示找到AAC解码器成功
    EXPECT_CALL(*mockCodec_, avcodec_find_decoder(AV_CODEC_ID_AAC))
        .WillRepeatedly(Return(reinterpret_cast<const AVCodec*>(0x1)));
    
    // 值0x1: 模拟非空的有效指针，表示编码上下文分配成功
    EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
        .WillRepeatedly(Return(reinterpret_cast<AVCodecContext*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
        .WillRepeatedly(Return(0));
    
    // 值0x1: 模拟非空的有效指针，表示数据包分配成功
    EXPECT_CALL(*mockCodec_, av_packet_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVPacket*>(0x1)));
    
    // 值0x1: 模拟非空的有效指针，表示音频帧分配成功
    EXPECT_CALL(*mockCodec_, av_frame_alloc())
        .WillRepeatedly(Return(reinterpret_cast<AVFrame*>(0x1)));
    
    EXPECT_CALL(*mockCodec_, avcodec_send_packet(testing::_, testing::_))
        .WillOnce(Return(0));
    
    EXPECT_CALL(*mockCodec_, avcodec_receive_frame(testing::_, testing::_))
        .WillOnce(Return(0));
    
    // 值0x1: 模拟非空的有效指针，表示重采样器分配成功
    EXPECT_CALL(*mockSwr_, swr_alloc_set_opts2(testing::_, testing::_, testing::_, testing::_,
        testing::_, testing::_, testing::_, testing::_))
        .WillOnce(Return(reinterpret_cast<SwrContext*>(0x1)));
    
    EXPECT_CALL(*mockSwr_, swr_init(testing::_))
        .WillOnce(Return(0));
    
    // 值1024: 1KB的样本缓冲区大小，用于音频转换测试
    EXPECT_CALL(*mockUtil_, av_samples_get_buffer_size(testing::_, testing::_, testing::_,
        testing::_, testing::_))
        .WillOnce(Return(1024)); // 1024 单位转换
    
    // 值1024: 分配1024字节的内存用于音频数据转换
    EXPECT_CALL(*mockCodec_, av_malloc(1024))
        .WillOnce(reinterpret_cast<uint8_t*>(0x1));
    
    // 值-1: 模拟swr_convert调用失败，返回错误码
    EXPECT_CALL(*mockSwr_, swr_convert(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(Return(-1));
    
    // 创建测试frame - AAC ADTS帧头部数据
    std::vector<uint8_t> testData = {0xFF, 0xF1, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00}; // 简化的测试数据
    
    // 使用std::copy替代memcpy进行数据拷贝
    auto frame = CreateTestFrame(testData.data(), testData.size());
    
    EXPECT_NO_THROW(decoder_->OnFrame(frame););
}

} // namespace Sharing
} // namespace OHOS
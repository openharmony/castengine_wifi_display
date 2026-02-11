/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef TESTS_UNITTEST_CODEC_AUDIO_AAC_ENCODER_TEST_H
#define TESTS_UNITTEST_CODEC_AUDIO_AAC_ENCODER_TEST_H

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "audio_aac_codec.h"
#include "mock/mock_global.h"
#include "mock/mock_libavcodec.h"
#include "mock/mock_libswresample.h"
#include "mock/mock_libavutil.h"
#include "protocol/frame/frame.h"
#include <algorithm> // 添加std::copy所需的头文件

namespace OHOS {
namespace Sharing {

class AudioAACEncoderTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        // 初始化mock对象
        mockCodec_ = std::make_unique<MockLibavCodec>();
        mockSwr_ = std::make_unique<MockLibSwresample>();
        mockUtil_ = std::make_unique<MockLibavUtil>();
        
        // 设置全局mock
        MockGlobalManager::GetInstance().SetMocks(mockCodec_.get(), mockSwr_.get(), mockUtil_.get());
        
        // 设置默认行为
        SetupDefaultBehavior();
        
        // 创建AudioAACEncoder实例
        encoder_ = std::make_unique<AudioAACEncoder>();
    }

    void TearDown() override
    {
        mockCodec_.reset();
        mockSwr_.reset();
        mockUtil_.reset();
        encoder_.reset();
    }

    void SetupDefaultBehavior()
    {
        // 设置avcodec_find_encoder的默认行为
        EXPECT_CALL(*mockCodec_, avcodec_find_encoder(AV_CODEC_ID_AAC))
            .WillRepeatedly(Return(reinterpret_cast<const AVCodec*>(0x1)));
        
        // 设置avcodec_alloc_context3的默认行为
        EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
            .WillRepeatedly(Return(reinterpret_cast<AVCodecContext*>(0x1)));
        
        // 设置avcodec_open2的默认行为
        EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
            .WillRepeatedly(Return(0));
        
        // 设置av_frame_alloc的默认行为
        EXPECT_CALL(*mockCodec_, av_frame_alloc())
            .WillRepeatedly(Return(reinterpret_cast<AVFrame*>(0x1)));
        
        // 设置av_frame_get_buffer的默认行为
        EXPECT_CALL(*mockCodec_, av_frame_get_buffer(testing::_, testing::_))
            .WillRepeatedly(Return(0));
        
        // 设置av_packet_alloc的默认行为
        EXPECT_CALL(*mockCodec_, av_packet_alloc())
            .WillRepeatedly(Return(reinterpret_cast<AVPacket*>(0x1)));
        
        // 设置av_audio_fifo_alloc的默认行为
        EXPECT_CALL(*mockUtil_, av_audio_fifo_alloc(testing::_, testing::_, testing::_))
            .WillRepeatedly(reinterpret_cast<AVAudioFifo*>(0x1));
        
        // 设置av_samples_alloc的默认行为
        EXPECT_CALL(*mockUtil_, av_samples_alloc(testing::_, testing::_, testing::_, testing::_, testing::_))
            .WillRepeatedly(Return(0));
        
        // 设置av_malloc的默认行为
        EXPECT_CALL(*mockCodec_, av_malloc(testing::_))
            .WillRepeatedly(reinterpret_cast<uint8_t*>(0x1));
        
        // 设置av_channel_layout_from_mask的默认行为
        EXPECT_CALL(*mockUtil_, av_channel_layout_from_mask(testing::_, testing::_))
            .WillRepeatedly(Return(0));
        
        // 设置swr_alloc_set_opts2的默认行为
        EXPECT_CALL(*mockSwr_, swr_alloc_set_opts2(testing::_, testing::_, testing::_, testing::_,
            testing::_, testing::_, testing::_, testing::_))
            .WillRepeatedly(Return(reinterpret_cast<SwrContext*>(0x1)));
        
        // 设置swr_init的默认行为
        EXPECT_CALL(*mockSwr_, swr_init(testing::_))
            .WillRepeatedly(Return(0));
        
        // 设置av_audio_fifo_realloc的默认行为
        EXPECT_CALL(*mockUtil_, av_audio_fifo_realloc(testing::_, testing::_))
            .WillRepeatedly(Return(0));
        
        // 设置av_audio_fifo_write的默认行为
        EXPECT_CALL(*mockUtil_, av_audio_fifo_write(testing::_, testing::_, testing::_))
            .WillRepeatedly(Return(testing::_));
        
        // 设置av_audio_fifo_read的默认行为
        EXPECT_CALL(*mockUtil_, av_audio_fifo_read(testing::_, testing::_, testing::_))
            .WillRepeatedly(Return(testing::_));
        
        // 设置av_frame_make_writable的默认行为
        EXPECT_CALL(*mockCodec_, av_frame_make_writable(testing::_))
            .WillRepeatedly(Return(0));
        
        // 设置avcodec_send_frame的默认行为
        EXPECT_CALL(*mockCodec_, avcodec_send_frame(testing::_, testing::_))
            .WillRepeatedly(Return(0));
        
        // 设置avcodec_receive_packet的默认行为
        EXPECT_CALL(*mockCodec_, avcodec_receive_packet(testing::_, testing::_))
            .WillOnce(Return(0))
            .WillRepeatedly(Return(AVERROR(EAGAIN)));
    }

    // 创建测试Frame
    FrameImpl::Ptr CreateTestFrame(const uint8_t* data, size_t size, CodecId codecId = CODEC_PCM)
    {
        // 内存大小合法性校验
        if (size == 0 || size > 100 * 1024 * 1024) { // 1024 设置合理的上限，比如100MB
            SHARING_LOGE("Invalid frame size: %{public}zu", size);
            return nullptr;
        }
        
        // 创建DataBuffer
        DataBuffer dataBuffer(size);
        if (data) {
            // 使用std::copy替代memcpy进行数据拷贝，更安全且符合C++标准
            std::copy(data, data + size, dataBuffer.Data());
            dataBuffer.SetSize(size);
        }
        
        // 创建Frame对象，通过构造函数传递DataBuffer
        auto frame = FrameImpl::CreateFrom(std::move(dataBuffer));
        frame->codecId_ = codecId;
        frame->dts_ = 1000; // 1000 构造帧数据
        frame->pts_ = 1000; // 1000 构造帧数据
        return frame;
    }

    // 设置不同的音频配置
    void SetupAudioConfig(uint32_t channels, uint32_t sampleBit, uint32_t sampleRate)
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
        
        int32_t result = encoder_->Init(channels, sampleBit, sampleRate);
        EXPECT_EQ(result, 0);
    }

protected:
    std::unique_ptr<MockLibavCodec> mockCodec_;
    std::unique_ptr<MockLibSwresample> mockSwr_;
    std::unique_ptr<MockLibavUtil> mockUtil_;
    std::unique_ptr<AudioAACEncoder> encoder_;
};

} // namespace Sharing
} // namespace OHOS

#endif // TESTS_UNITTEST_CODEC_AUDIO_AAC_ENCODER_TEST_H
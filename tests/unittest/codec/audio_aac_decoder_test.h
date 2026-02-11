/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef TESTS_UNITTEST_CODEC_AUDIO_AAC_DECODER_TEST_H
#define TESTS_UNITTEST_CODEC_AUDIO_AAC_DECODER_TEST_H

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

class AudioAACDecoderTest : public ::testing::Test {
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
        
        // 创建AudioAACDecoder实例
        decoder_ = std::make_unique<AudioAACDecoder>();
    }

    void TearDown() override
    {
        mockCodec_.reset();
        mockSwr_.reset();
        mockUtil_.reset();
        decoder_.reset();
    }

    void SetupDefaultBehavior()
    {
        // 设置avcodec_find_decoder的默认行为
        EXPECT_CALL(*mockCodec_, avcodec_find_decoder(AV_CODEC_ID_AAC))
            .WillRepeatedly(Return(reinterpret_cast<const AVCodec*>(0x1)));
        
        // 设置avcodec_alloc_context3的默认行为
        EXPECT_CALL(*mockCodec_, avcodec_alloc_context3(testing::_))
            .WillRepeatedly(Return(reinterpret_cast<AVCodecContext*>(0x1)));
        
        // 设置avcodec_open2的默认行为
        EXPECT_CALL(*mockCodec_, avcodec_open2(testing::_, testing::_, testing::_))
            .WillRepeatedly(Return(0));
        
        // 设置av_packet_alloc的默认行为
        EXPECT_CALL(*mockCodec_, av_packet_alloc())
            .WillRepeatedly(Return(reinterpret_cast<AVPacket*>(0x1)));
        
        // 设置av_frame_alloc的默认行为
        EXPECT_CALL(*mockCodec_, av_frame_alloc())
            .WillRepeatedly(Return(reinterpret_cast<AVFrame*>(0x1)));
        
        // 设置swr_alloc_set_opts2的默认行为
        EXPECT_CALL(*mockSwr_, swr_alloc_set_opts2(testing::_, testing::_, testing::_, testing::_,
                                                   testing::_, testing::_, testing::_, testing::_))
            .WillRepeatedly(Return(reinterpret_cast<SwrContext*>(0x1)));
        
        // 设置swr_init的默认行为
        EXPECT_CALL(*mockSwr_, swr_init(testing::_))
            .WillRepeatedly(Return(0));
        
        // 设置av_samples_get_buffer_size的默认行为
        EXPECT_CALL(*mockUtil_, av_samples_get_buffer_size(testing::_, testing::_, testing::_,
            testing::_, testing::_))
            .WillRepeatedly(Return(1024)); // 1024 单位转换
        
        // 设置av_malloc的默认行为
        EXPECT_CALL(*mockCodec_, av_malloc(testing::_))
            .WillRepeatedly(reinterpret_cast<uint8_t*>(0x1));
    }

    // 创建测试Frame
    FrameImpl::Ptr CreateTestFrame(const uint8_t* data, size_t size, CodecId codecId = CODEC_AAC)
    {
        // 内存大小合法性校验
        if (size == 0 || size > 100 * 1024 * 1024) { // 100 大小 1024 单位转换 设置合理的上限，比如100MB
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
        return frame;
    }

    // 创建AudioTrack
    AudioTrack CreateDefaultAudioTrack()
    {
        AudioTrack track;
        track.channels = 2; // 2 通道数
        track.sampleBit = 16; // 16 采样
        track.sampleRate = 44100; // 44100 采样
        track.sampleFormat = 1; // 1 音频构造
        track.codecId = CODEC_AAC;
        return track;
    }

protected:
    std::unique_ptr<MockLibavCodec> mockCodec_;
    std::unique_ptr<MockLibSwresample> mockSwr_;
    std::unique_ptr<MockLibavUtil> mockUtil_;
    std::unique_ptr<AudioAACDecoder> decoder_;
};

} // namespace Sharing
} // namespace OHOS

#endif // TESTS_UNITTEST_CODEC_AUDIO_AAC_DECODER_TEST_H
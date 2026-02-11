/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef_UNIT TESTSTEST_CODEC_AUDIO_AAC_DECODER_TEST_H
#define TESTS_UNITTEST_CODEC_AUDIO_AAC_DECODER_TEST_H

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "audio_aac_codec.h"
#include "mock/mock_global.h"
#include "mock/mock_libavcodec.h"
#include "mock/mock_libswresample.h"
#include "mock/mock_libavutil.h"
#include "protocol/frame/frame.h"

namespace OHOS {
namespace Sharing {

class AudioAACDecoderTest : public ::testing::Test {
protected:
    void SetUp() override {
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

    void TearDown() override {
        mockCodec_.reset();
        mockSwr_.reset();
        mockUtil_.reset();
        decoder_.reset();
    }

    void SetupDefaultBehavior() {
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
            .WillRepeatedly(Return(1024));
        
        // 设置av_malloc的默认行为
        EXPECT_CALL(*mockCodec_, av_malloc(testing::_))
            .WillRepeatedly(reinterpret_cast<uint8_t*>(0x1));
    }

    // 创建测试Frame
    FrameImpl::Ptr CreateTestFrame(const uint8_t* data, size_t size, CodecId codecId = CODEC_AAC) {
        auto frame = FrameImpl::Create();
        frame->data_ = new uint8_t[size];
        frame->size_ = size;
        frame->capacity_ = size;
        if (data) {
            memcpy(frame->data_, data, size);
        }
        frame->codecId_ = codecId;
        return frame;
    }

    // 创建AudioTrack
    AudioTrack CreateDefaultAudioTrack() {
        AudioTrack track;
        track.channels = 2;
        track.sampleBit = 16;
        track.sampleRate = 44100;
        track.sampleFormat = 1;
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
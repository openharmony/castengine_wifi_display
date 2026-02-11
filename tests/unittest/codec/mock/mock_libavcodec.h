/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef TESTS_UNITTEST_CODEC_MOCK_MOCK_LIBAVCODEC_H
#define TESTS_UNITTEST_CODEC_MOCK_MOCK_LIBAVCODEC_H

#include <gmock/gmock.h>
#include <libavcodec/avcodec.h>

namespace OHOS {
namespace Sharing {

class MockLibavCodec {
public:
    MockLibavCodec() = default;
    virtual ~MockLibavCodec() = default;

    MOCK_METHOD(const AVCodec*, avcodec_find_decoder, (enum AVCodecID id));
    MOCK_METHOD(const AVCodec*, avcodec_find_encoder, (enum AVCodecID id));
    MOCK_METHOD(AVCodecContext*, avcodec_alloc_context3, (const AVCodec *codec));
    MOCK_METHOD(int, avcodec_open2, (AVCodecContext *avctx, const AVCodec *codec, const AVDictionary **options));
    MOCK_METHOD(void, avcodec_free_context, (AVCodecContext **avctx));
    MOCK_METHOD(AVPacket*, av_packet_alloc, ());
    MOCK_METHOD(void, av_packet_free, (AVPacket **pkt));
    MOCK_METHOD(void, av_packet_unref, (AVPacket *pkt));
    MOCK_METHOD(AVFrame*, av_frame_alloc, ());
    MOCK_METHOD(void, av_frame_free, (AVFrame **frame));
    MOCK_METHOD(void, av_frame_unref, (AVFrame *frame));
    MOCK_METHOD(int, av_frame_get_buffer, (AVFrame *frame, int align));
    MOCK_METHOD(int, av_frame_make_writable, (AVFrame *frame));
    MOCK_METHOD(int, avcodec_send_packet, (AVCodecContext *avctx, const AVPacket *avpkt));
    MOCK_METHOD(int, avcodec_receive_frame, (AVCodecContext *avctx, AVFrame *frame));
    MOCK_METHOD(int, avcodec_receive_packet, (AVCodecContext *avctx, AVPacket *avpkt));
    MOCK_METHOD(void, av_init_packet, (AVPacket *pkt));
    MOCK_METHOD(int, av_samples_get_buffer_size,
        (int *linesize, int nb_channels, int nb_samples, enum AVSampleFormat sample_fmt, int align));
    MOCK_METHOD(uint8_t*, av_malloc, (size_t size));
    MOCK_METHOD(void, av_freep, (void *ptr));
};

extern MockLibavCodec *g_mockLibavCodec;

} // namespace Sharing
} // namespace OHOS

#endif // TESTS_UNITTEST_CODEC_MOCK_MOCK_LIBAVCODEC_H
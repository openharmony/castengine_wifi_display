/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef TESTS_UNITTEST_CODEC_MOCK_MOCK_LIBAVUTIL_H
#define TESTS_UNITTEST_CODEC_MOCK_MOCK_LIBAVUTIL_H

#include <gmock/gmock.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/channel_layout.h>
#include <libavutil/error.h>

namespace OHOS {
namespace Sharing {

class MockLibavUtil {
public:
    MockLibavUtil() = default;
    virtual ~MockLibavUtil() = default;

    MOCK_METHOD(int, av_channel_layout_from_mask, (AVChannelLayout *channel_layout, uint64_t mask));
    MOCK_METHOD(void, av_channel_layout_copy, (AVChannelLayout *dst, const AVChannelLayout *src));
    MOCK_METHOD(void, av_channel_layout_default, (AVChannelLayout *ch_layout, int nb_channels));
    MOCK_METHOD(int, av_audio_fifo_alloc, (AVSampleFormat sample_fmt, int nb_channels, int nb_samples));
    MOCK_METHOD(void, av_audio_fifo_free, (AVAudioFifo *af));
    MOCK_METHOD(int, av_audio_fifo_realloc, (AVAudioFifo *af, int nb_samples));
    MOCK_METHOD(int, av_audio_fifo_write, (AVAudioFifo *af, void **data, int nb_samples));
    MOCK_METHOD(int, av_audio_fifo_read, (AVAudioFifo *af, void **data, int nb_samples));
    MOCK_METHOD(int, av_audio_fifo_size, (AVAudioFifo *af));
    MOCK_METHOD(int, av_samples_alloc, (uint8_t **audio_data, int *linesize, int nb_channels, 
        int nb_samples, enum AVSampleFormat sample_fmt, int align));
    MOCK_METHOD(int, av_samples_copy, (uint8_t **dst, uint8_t *const *src, int dst_offset, int src_offset,
        int nb_samples, int nb_channels, enum AVSampleFormat sample_fmt));
    MOCK_METHOD(int, av_samples_set_silence, (uint8_t **audio_data, int offset, int nb_samples,
        int nb_channels, enum AVSampleFormat sample_fmt));
    MOCK_METHOD(char*, av_make_error_string, (char *errbuf, size_t errbuf_size, int errnum));
    MOCK_METHOD(int64_t, av_rescale, (int64_t a, int64_t b, int64_t c));
};

extern MockLibavUtil *g_mockLibavUtil;

} // namespace Sharing
} // namespace OHOS

#endif // TESTS_UNITTEST_CODEC_MOCK_MOCK_LIBAVUTIL_H
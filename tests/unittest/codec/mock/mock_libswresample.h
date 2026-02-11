/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef TESTS_UNITTEST_CODEC_MOCK_MOCK_LIBSWRESAMPLE_H
#define TESTS_UNITTEST_CODEC_MOCK_MOCK_LIBSWRESAMPLE_H

#include <gmock/gmock.h>
extern "C" {
#include <libswresample/swresample.h>
}

namespace OHOS {
namespace Sharing {

class MockLibSwresample {
public:
    MockLibSwresample() = default;
    virtual ~MockLibSwresample() = default;

    MOCK_METHOD(SwrContext*, swr_alloc_set_opts2, 
                (SwrContext **ps, const AVChannelLayout *out_ch_layout, AVSampleFormat out_sample_fmt, int out_sample_rate,
                const AVChannelLayout *in_ch_layout, AVSampleFormat in_sample_fmt, int in_sample_rate, int64_t channel_map, void *log_ctx));
    MOCK_METHOD(SwrContext*, swr_alloc, ());
    MOCK_METHOD(void, swr_free, (SwrContext **swr));
    MOCK_METHOD(int, swr_init, (SwrContext *s));
    MOCK_METHOD(int, swr_convert, (SwrContext *s, uint8_t **out, int out_count, const uint8_t **in, int in_count));
    MOCK_METHOD(int, swr_get_out_samples, (SwrContext *s, int in_samples));
    MOCK_METHOD(int, swr_close, (SwrContext *s));
};

extern MockLibSwresample *g_mockLibSwresample;

} // namespace Sharing
} // namespace OHOS

#endif // TESTS_UNITTEST_CODEC_MOCK_MOCK_LIBSWRESAMPLE_H
/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef TESTS_UNITTEST_CODEC_MOCK_FFMPEG_FUNCTION_REPLACER_H
#define TESTS_UNITTEST_CODEC_MOCK_FFMPEG_FUNCTION_REPLACER_H

#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/channel_layout.h>
#include <libavutil/error.h>
#include "mock_global.h"

namespace OHOS {
namespace Sharing {

// 全局函数指针类型定义
typedef const AVCodec* (*AvCodecFindDecoderFunc)(enum AVCodecID id);
typedef const AVCodec* (*AvCodecFindEncoderFunc)(enum AVCodecID id);
typedef AVCodecContext* (*AvCodecAllocContext3Func)(const AVCodec *codec);
typedef int (*AvCodecOpen2Func)(AVCodecContext *avctx, const AVCodec *codec, const AVDictionary **options);
typedef void (*AvCodecFreeContextFunc)(AVCodecContext **avctx);
typedef AVPacket* (*AvPacketAllocFunc)(void);
typedef void (*AvPacketFreeFunc)(AVPacket **pkt);
typedef void (*AvPacketUnrefFunc)(AVPacket *pkt);
typedef AVFrame* (*AvFrameAllocFunc)(void);
typedef void (*AvFrameFreeFunc)(AVFrame **frame);
typedef void (*AvFrameUnrefFunc)(AVFrame *frame);
typedef int (*AvFrameGetBufferFunc)(AVFrame *frame, int align);
typedef int (*AvFrameMakeWritableFunc)(AVFrame *frame);
typedef int (*AvCodecSendPacketFunc)(AVCodecContext *avctx, const AVPacket *avpkt);
typedef int (*AvCodecReceiveFrameFunc)(AVCodecContext *avctx, AVFrame *frame);
typedef int (*AvCodecReceivePacketFunc)(AVCodecContext *avctx, AVPacket *avpkt);
typedef void (*AvInitPacketFunc)(AVPacket *pkt);
typedef int (*AvSamplesGetBufferSizeFunc)(int *linesize, int nb_channels, int nb_samples, 
    enum AVSampleFormat sample_fmt, int align);
typedef uint8_t* (*AvMallocFunc)(size_t size);
typedef void (*AvFreepFunc)(void *ptr);
typedef SwrContext* (*SwrAllocSetOpts2Func)(SwrContext **ps, const AVChannelLayout *out_ch_layout, 
    AVSampleFormat out_sample_fmt, int out_sample_rate, const AVChannelLayout *in_ch_layout,
    AVSampleFormat in_sample_fmt, int in_sample_rate, int64_t channel_map, void *log_ctx);
typedef SwrContext* (*SwrAllocFunc)(void);
typedef void (*SwrFreeFunc)(SwrContext **swr);
typedef int (*SwrInitFunc)(SwrContext *s);
typedef int (*SwrConvertFunc)(SwrContext *s, uint8_t **out, int out_count, const uint8_t **in, int in_count);
typedef int (*SwrGetOutSamplesFunc)(SwrContext *s, int in_samples);
typedef int (*SwrCloseFunc)(SwrContext *s);
typedef int (*AvChannelLayoutFromMaskFunc)(AVChannelLayout *channel_layout, uint64_t mask);
typedef void (*AvChannelLayoutCopyFunc)(AVChannelLayout *dst, const AVChannelLayout *src);
typedef void (*AvChannelLayoutDefaultFunc)(AVChannelLayout *ch_layout, int nb_channels);
typedef int (*AvAudioFifoAllocFunc)(AVSampleFormat sample_fmt, int nb_channels, int nb_samples);
typedef void (*AvAudioFifoFreeFunc)(AVAudioFifo *af);
typedef int (*AvAudioFifoReallocFunc)(AVAudioFifo *af, int nb_samples);
typedef int (*AvAudioFifoWriteFunc)(AVAudioFifo *af, void **data, int nb_samples);
typedef int (*AvAudioFifoReadFunc)(AVAudioFifo *af, void **data, int nb_samples);
typedef int (*AvAudioFifoSizeFunc)(AVAudioFifo *af);
typedef int (*AvSamplesAllocFunc)(uint8_t **audio_data, int *linesize, int nb_channels, 
    int nb_samples, enum AVSampleFormat sample_fmt, int align);
typedef int (*AvSamplesCopyFunc)(uint8_t **dst, uint8_t *const *src, int dst_offset, int src_offset,
    int nb_samples, int nb_channels, enum AVSampleFormat sample_fmt);
typedef int (*AvSamplesSetSilenceFunc)(uint8_t **audio_data, int offset, int nb_samples,
    int nb_channels, enum AVSampleFormat sample_fmt);
typedef char* (*AvMakeErrorStringFunc)(char *errbuf, size_t errbuf_size, intnum);
errtypedef int64_t (*AvRescaleFunc)(int64_t a, int64_t b, int64_t c);

// 全局函数指针
extern AvCodecFindDecoderFunc g_avcodec_find_decoder;
extern AvCodecFindEncoderFunc g_avcodec_find_encoder;
extern AvCodecAllocContext3Func g_avcodec_alloc_context3;
extern AvCodecOpen2Func g_avcodec_open2;
extern AvCodecFreeContextFunc g_avcodec_free_context;
extern AvPacketAllocFunc g_av_packet_alloc;
extern AvPacketFreeFunc g_av_packet_free;
extern AvPacketUnrefFunc g_av_packet_unref;
extern AvFrameAllocFunc g_av_frame_alloc;
extern AvFrameFreeFunc g_av_frame_free;
extern AvFrameUnrefFunc g_av_frame_unref;
extern AvFrameGetBufferFunc g_av_frame_get_buffer;
extern AvFrameMakeWritableFunc g_av_frame_make_writable;
extern AvCodecSendPacketFunc g_avcodec_send_packet;
extern AvCodecReceiveFrameFunc g_avcodec_receive_frame;
extern AvCodecReceivePacketFunc g_avcodec_receive_packet;
extern AvInitPacketFunc g_av_init_packet;
extern AvSamplesGetBufferSizeFunc g_av_samples_get_buffer_size;
extern AvMallocFunc g_av_malloc;
extern AvFreepFunc g_av_freep;
extern SwrAllocSetOpts2Func g_swr_alloc_set_opts2;
extern SwrAllocFunc g_swr_alloc;
extern SwrFreeFunc g_swr_free;
extern SwrInitFunc g_swr_init;
extern SwrConvertFunc g_swr_convert;
extern SwrGetOutSamplesFunc g_swr_get_out_samples;
extern SwrCloseFunc g_swr_close;
extern AvChannelLayoutFromMaskFunc g_av_channel_layout_from_mask;
extern AvChannelLayoutCopyFunc g_av_channel_layout_copy;
extern AvChannelLayoutDefaultFunc g_av_channel_layout_default;
extern AvAudioFifoAllocFunc g_av_audio_fifo_alloc;
extern AvAudioFifoFreeFunc g_av_audio_fifo_free;
extern AvAudioFifoReallocFunc g_av_audio_fifo_realloc;
extern AvAudioFifoWriteFunc g_av_audio_fifo_write;
extern AvAudioFifoReadFunc g_av_audio_fifo_read;
extern AvAudioFifoSizeFunc g_av_audio_fifo_size;
extern AvSamplesAllocFunc g_av_samples_alloc;
extern AvSamplesCopyFunc g_av_samples_copy;
extern AvSamplesSetSilenceFunc g_av_samples_set_silence;
extern AvMakeErrorStringFunc g_av_make_error_string;
extern AvRescaleFunc g_av_rescale;

// 函数初始化
void InitFFunctionReplacer();

// 函数恢复
void RestoreFFunctionReplacer();

// 设置mock函数
void SetMockFunctions(MockLibavCodec* codec, MockLibSwresample* swr, MockLibavUtil* util);

} // namespace Sharing
} // namespace OHOS

#endif // TESTS_UNITTEST_CODEC_MOCK_FFMPEG_FUNCTION_REPLACER_H
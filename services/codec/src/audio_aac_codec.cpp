/*
 * Copyright (c) 2023 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "audio_aac_codec.h"
#include <stdint.h>
#include "sharing_log.h"
namespace OHOS {
namespace Sharing {
// AudioAACDecoder::AudioAACDecoder(TRANSPORT_TYPE type)
AudioAACDecoder::AudioAACDecoder()
{
    SHARING_LOGD("trace.");
    // type_ = type;
}

AudioAACDecoder::~AudioAACDecoder()
{
    SHARING_LOGD("trace.");
    // if (aacDecoderHandle_ != nullptr) {
    //     aacDecoder_Close(aacDecoderHandle_);
    //     aacDecoderHandle_ = nullptr;
    // }
    // if (outBuffer_ != nullptr) {
    //     delete[] outBuffer_;
    //     outBuffer_ = nullptr;
    // }
}

int32_t AudioAACDecoder::Init()
{
    SHARING_LOGD("trace.");
    // aacDecoderHandle_ = aacDecoder_Open(type_, numberOfLayers);
    // if (aacDecoderHandle_ == nullptr) {
    //     SHARING_LOGE("aacDecoder_Open error!");
    //     return -1;
    // }
    // if (type_ == TT_MP4_RAW) {
    //     UCHAR lc_conf[] = {0xBA, 0X88, 0x00, 0x00};
    //     UCHAR *conf[] = {lc_conf};
    //     static UINT conf_len = sizeof(lc_conf);
    //     errorCode_ = aacDecoder_ConfigRaw(aacDecoderHandle_, conf, &conf_len);
    //     if (errorCode_ > 0) {
    //         SHARING_LOGE("aacDecoder_Config error!");
    //         return false;
    //     }
    // }
    // aacDecoder_SetParam(aacDecoderHandle_, AAC_CONCEAL_METHOD, concealMethod_);
    // aacDecoder_SetParam(aacDecoderHandle_, AAC_PCM_MIN_OUTPUT_CHANNELS, channels_);
    // aacDecoder_SetParam(aacDecoderHandle_, AAC_PCM_MAX_OUTPUT_CHANNELS, channels_);
    // outBuffer_ = new char[maxOutBufferSize];
    // if (outBuffer_ == nullptr) {
    //     SHARING_LOGE("aacDecoder_Config outbuffer alloc failed!");
    //     return -1;
    // }

    return 0;
}

void AudioAACDecoder::OnFrame(const Frame::Ptr &frame)
{
    // if (frame == nullptr) {
    //     SHARING_LOGE("frame is nullptr!");
    //     return;
    // }
    // if (aacDecoderHandle_ == nullptr) {
    //     SHARING_LOGE("AudioAACDecoder OnFrame error\n!");
    //     return;
    // }
    // int32_t ret = 0;
    // uint32_t outSize = maxOutBufferSize;
    // uint32_t inSize = frame->Size();
    // uint32_t vaildSize = frame->Size();
    // UCHAR *inBuf = (UCHAR *)frame->Data();

    // do {
    //     ret = aacDecoder_Fill(aacDecoderHandle_, &inBuf, &inSize, &vaildSize);
    //     if (ret != AAC_DEC_OK) {
    //         SHARING_LOGE("aacDecoder_Fill error ret: %{public}d.", ret);
    //     }
    //     memset(outBuffer_, 0, maxOutBufferSize);
    //     ret = aacDecoder_DecodeFrame(aacDecoderHandle_, (INT_PCM *)outBuffer_, outSize / sizeof(INT_PCM), flags_);
    //     if (ret == AAC_DEC_OK) {
    //         CStreamInfo *info = aacDecoder_GetStreamInfo(aacDecoderHandle_);
    //         uint32_t output_len = info->frameSize * info->numChannels * sizeof(INT_PCM);
    //         auto pcmFrame = FrameImpl::Create();
    //         pcmFrame->codecId_ = CODEC_PCM;
    //         pcmFrame->Assign(outBuffer_, output_len);
    //         DeliverFrame(pcmFrame);

    //     } else if (ret != AAC_DEC_NOT_ENOUGH_BITS) {
    //         if (ret == AAC_DEC_TRANSPORT_SYNC_ERROR) {
    //             SHARING_LOGD("aacDecoder_DecodeFrame error ret: %{public}d.", ret);
    //         } else {
    //             SHARING_LOGE("aacDecoder_DecodeFrame error ret: %{public}d.", ret);
    //         }

    //         return;
    //     }
    // } while (vaildSize > 0 && ret != AAC_DEC_NOT_ENOUGH_BITS);
}

} // namespace Sharing
} // namespace OHOS

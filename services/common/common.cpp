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

#include "common.h"

namespace OHOS {
namespace Sharing {

#define VIDEO_RESOLUTION_640 640
#define VIDEO_RESOLUTION_480 480
#define VIDEO_RESOLUTION_1280 1280
#define VIDEO_RESOLUTION_720 720
#define VIDEO_RESOLUTION_1920 1920
#define VIDEO_RESOLUTION_1080 1080

#define VIDEO_FRAME_RATE_60 60
#define VIDEO_FRAME_RATE_25 25
#define VIDEO_FRAME_RATE_30 30

#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_SAMPLE_BIT 16
#define AUDIO_SAMEPLE_CHANNEL 2

void Common::SetVideoTrack(VideoTrack &videoTrack, VideoFormat videoFormat)
{
    switch (videoFormat) {
        case VideoFormat::VIDEO_640X480_60:
            videoTrack.codecId = CodecId::CODEC_H264;
            videoTrack.width = VIDEO_RESOLUTION_640;
            videoTrack.height = VIDEO_RESOLUTION_480;
            videoTrack.frameRate = VIDEO_FRAME_RATE_60;
            break;
        case VideoFormat::VIDEO_1280X720_25:
            videoTrack.codecId = CodecId::CODEC_H264;
            videoTrack.width = VIDEO_RESOLUTION_1280;
            videoTrack.height = VIDEO_RESOLUTION_720;
            videoTrack.frameRate = VIDEO_FRAME_RATE_25;
            break;
        case VideoFormat::VIDEO_1280X720_30:
            videoTrack.codecId = CodecId::CODEC_H264;
            videoTrack.width = VIDEO_RESOLUTION_1280;
            videoTrack.height = VIDEO_RESOLUTION_720;
            videoTrack.frameRate = VIDEO_FRAME_RATE_30;
            break;
        case VideoFormat::VIDEO_1920X1080_25:
            videoTrack.codecId = CodecId::CODEC_H264;
            videoTrack.width = VIDEO_RESOLUTION_1920;
            videoTrack.height = VIDEO_RESOLUTION_1080;
            videoTrack.frameRate = VIDEO_FRAME_RATE_25;
            break;
        case VideoFormat::VIDEO_1920X1080_30:
            videoTrack.codecId = CodecId::CODEC_H264;
            videoTrack.width = VIDEO_RESOLUTION_1920;
            videoTrack.height = VIDEO_RESOLUTION_1080;
            videoTrack.frameRate = VIDEO_FRAME_RATE_30;
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }
}

void Common::SetAudioTrack(AudioTrack &audioTrack, AudioFormat audioFormat)
{
    switch (audioFormat) {
        case AudioFormat::AUDIO_44100_8_2:
        case AudioFormat::AUDIO_44100_16_2:
            break;
        case AudioFormat::AUDIO_48000_16_2:
            audioTrack.codecId = CodecId::CODEC_AAC;
            audioTrack.sampleRate = AUDIO_SAMPLE_RATE;
            audioTrack.sampleBit = AUDIO_SAMPLE_BIT;
            audioTrack.channels = AUDIO_SAMEPLE_CHANNEL;
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }
}
} // namespace Sharing
} // namespace OHOS
/*
 * Copyright (c) 2023 Shenzhen Kaihong DID Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
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

void Common::SetVideoTrack(VideoTrack &videoTrack, VideoFormat videoFormat)
{
    switch (videoFormat) {
        case VideoFormat::VIDEO_640x480_60:
            videoTrack.codecId = CodecId::CODEC_H264;
            videoTrack.width = 640;
            videoTrack.height = 480;
            videoTrack.frameRate = 60;
            break;
        case VideoFormat::VIDEO_1280x720_25:
            videoTrack.codecId = CodecId::CODEC_H264;
            videoTrack.width = 1280;
            videoTrack.height = 720;
            videoTrack.frameRate = 25;
            break;
        case VideoFormat::VIDEO_1280x720_30:
            videoTrack.codecId = CodecId::CODEC_H264;
            videoTrack.width = 1280;
            videoTrack.height = 720;
            videoTrack.frameRate = 30;
            break;
        case VideoFormat::VIDEO_1920x1080_25:
            videoTrack.codecId = CodecId::CODEC_H264;
            videoTrack.width = 1920;
            videoTrack.height = 1080;
            videoTrack.frameRate = 25;
            break;
        case VideoFormat::VIDEO_1920x1080_30:
            videoTrack.codecId = CodecId::CODEC_H264;
            videoTrack.width = 1920;
            videoTrack.height = 1080;
            videoTrack.frameRate = 30;
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
            audioTrack.sampleRate = 48000;
            audioTrack.sampleBit = 16;
            audioTrack.channels = 2;
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }
}
} // namespace Sharing
} // namespace OHOS
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

#include "loop_back_demo.h"
#include <thread>
#include "audio_capturer.h"
#include "audio_errors.h"
#include "audio_info.h"
#include "audio_log.h"
#include "audio_renderer.h"
using namespace OHOS::AudioStandard;

namespace OHOS {
namespace AudioStandard {
LoopBackDemo::LoopBackDemo(){};
LoopBackDemo::~LoopBackDemo()
{
    audioCapturer_->Release();
    audioRenderer_->Release();
}

void LoopBackDemo::InitializeCapturerOptions(AudioStandard::AudioCapturerOptions &capturerOptions)
{
    capturerOptions.streamInfo.samplingRate = AudioSamplingRate::SAMPLE_RATE_44100;
    capturerOptions.streamInfo.encoding = AudioEncodingType::ENCODING_PCM;
    capturerOptions.streamInfo.format = AudioSampleFormat::SAMPLE_S16LE;
    capturerOptions.streamInfo.channels = AudioChannel::STEREO;
    capturerOptions.capturerInfo.sourceType = SourceType::SOURCE_TYPE_MIC;
    capturerOptions.capturerInfo.capturerFlags = 0;
    return;
}

int32_t LoopBackDemo::InitAudioCapturer()
{
    InitializeCapturerOptions(capturerOptions_);
    audioCapturer_ = AudioCapturer::Create(capturerOptions_);
    if (audioCapturer_ == nullptr) {
        AUDIO_INFO_LOG("loopback: create capture fail");
        return -1;
    }
    AUDIO_INFO_LOG("loopback: create capture success");
    return 0;
}

int32_t LoopBackDemo::StartCapture()
{
    InitAudioCapturer();
    if (audioCapturer_ == nullptr) {
        AUDIO_INFO_LOG("loopback: start capture fail - invalid capturer");
        return -1;
    }
    if (isRunningCapture_) {
        return -1;
    }

    int32_t ret = 0;
    bool isStarted = audioCapturer_->Start();
    if (!isStarted) {
        AUDIO_INFO_LOG("loopback: start capture fail");
        return -1;
    }

    ret = audioCapturer_->GetBufferSize(bufferLen_);
    if (ret < AudioStandard::SUCCESS) {
        bufferLen_ = 0;
        AUDIO_INFO_LOG("loopback: get capture buffer fail - GetBufferSize fail  ret:%{public}d", ret);
        return -1;
    }
    isRunningCapture_ = true;
    AUDIO_INFO_LOG("loopback: audio capture start successful");
    return 0;
}

void LoopBackDemo::InitializeRenderOptions(AudioRendererParams &rendererParams)
{
    rendererParams.sampleFormat = SAMPLE_S16LE;
    rendererParams.sampleRate = SAMPLE_RATE_44100;
    rendererParams.channelCount = STEREO;
    rendererParams.encodingType = ENCODING_PCM;
    return;
}

int32_t LoopBackDemo::InitAudioRenderer()
{
    audioRenderer_ = AudioRenderer::Create(AudioStreamType::STREAM_MUSIC);
    if (!audioRenderer_) {
        AUDIO_INFO_LOG("loopback: Prepare audioRenderer_ is NULL");
        return -1;
    }
    InitializeRenderOptions(rendererParams_);
    int res = audioRenderer_->SetParams(rendererParams_);
    AUDIO_INFO_LOG("loopback: Prepare audioRenderer_ success.");
    return res;
}

int32_t LoopBackDemo::StartRender()
{
    int ret = 0;
    ret = InitAudioRenderer();
    if (ret < 0)
        AUDIO_INFO_LOG("loopback: init audioRenderer_ fail");
    if (!audioRenderer_) {
        AUDIO_INFO_LOG("loopback: audioRenderer_ is NULL");
        return -1;
    }
    bool isStarted = audioRenderer_->Start();
    if (!isStarted) {
        AUDIO_INFO_LOG("loopback: Start audioRenderer_ fail");
        return -1;
    }
    isRunningRender_ = true;
    AUDIO_INFO_LOG("loopback: audio render start successful");
    return 0;
}

void LoopBackDemo::loopback()
{
    StartCapture();
    StartRender();

    uint8_t *frame = (uint8_t *)malloc(bufferLen_ * 2);
    if (frame == nullptr) {
        return;
    }
    while (isRunningCapture_ && isRunningRender_) {
        int temp = audioCapturer_->Read(*frame, bufferLen_, isBlockingRead_);
        audioRenderer_->Write(frame, temp);
    }
    return;
}
void LoopBackDemo::CaptureThreadWorker() {}
void LoopBackDemo::RendererThread() {}

bool LoopBackDemo::GetPermission()
{
    uint64_t tokenId;
    const char *perms[2];
    perms[0] = "ohos.permission.CAMERA";
    perms[1] = "ohos.permission.MICROPHONE";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 2,
        .aclsNum = 0,
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "loop_back_demo",
        .aplStr = "system_basic",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    if (tokenId == 0) {
        AUDIO_INFO_LOG("GetCameraPermission GetAccessTokenId failed");
        return false;
    }
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    return true;
}
} // namespace AudioStandard
} // namespace OHOS

int main()
{
    AUDIO_INFO_LOG("loopback: begin");
    LoopBackDemo *test = new LoopBackDemo();
    test->GetPermission();
    setuid(1041); // audio - 1041
    test->loopback();
    // TestCmdVector();
    AUDIO_INFO_LOG("loopback: exit demo!");
    return 0;
}
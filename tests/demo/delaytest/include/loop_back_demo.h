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

#ifndef LOOP_BACK_DEMO_H
#define LOOP_BACK_DEMO_H

#include "access_token.h"
#include "accesstoken_kit.h"
#include "audio_capturer.h"
#include "audio_renderer.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

namespace OHOS {
namespace AudioStandard {
class LoopBackDemo {
public:
    LoopBackDemo();
    ~LoopBackDemo();

    void InitializeRenderOptions(AudioRendererParams &rendererParams);
    void InitializeCapturerOptions(AudioCapturerOptions &capturerOptions);

    int32_t StartRender();
    int32_t InitAudioRenderer();

    int32_t StartCapture();
    int32_t InitAudioCapturer();

    void RendererThread();
    void CaptureThreadWorker();

    void loopback();
    bool GetPermission();

private:
    bool isBlockingRead_ = true;
    bool isRunningRender_ = false;
    bool isRunningCapture_ = false;

    size_t bufferLen_ = 0;

    std::unique_ptr<AudioCapturer> audioCapturer_ = nullptr;
    std::unique_ptr<AudioRenderer> audioRenderer_ = nullptr;

    AudioRendererParams rendererParams_;
    AudioCapturerOptions capturerOptions_;
};
} // namespace AudioStandard
} // namespace OHOS
#endif // AUDIO_CAPTURER_UNIT_TEST_H

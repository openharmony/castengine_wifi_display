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

#include "audio_capturer.h"
#include "audio_renderer.h"
#include "access_token.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

namespace OHOS {
namespace AudioStandard {
class LoopBackDemo  {
public:
    LoopBackDemo();
    ~LoopBackDemo();
    void InitializeCapturerOptions(AudioCapturerOptions &capturerOptions);//初始化麦克风参数
    void InitializeRenderOptions(AudioRendererParams &rendererParams);//初始化喇叭参数
    int32_t InitAudioCapturer();//创建capture
    int32_t StartCapture();//运行capture
    int32_t InitAudioRenderer();//创建render
    int32_t StartRender();//运行render
    void loopback();//声音回环
    void CaptureThreadWorker();//capture线程创建
    void RendererThread();//renderer线程创建
    bool GetPermission(); //获取权限
private:
    AudioCapturerOptions capturerOptions_;
    std::unique_ptr<AudioCapturer> audioCapturer_ = nullptr;
    // std::mutex mutex_capture;
    bool isRunningCapture_ = false;
    //std::thread captureThread_;
    size_t bufferLen_ = 0;
    bool isBlockingRead_ = true;
    // bool isInit = false;
    bool isRunningRender_ = false;

    std::unique_ptr<AudioRenderer> audioRenderer_ = nullptr;
    //CacheQueue *cacheQueue_ = nullptr;
    // std::mutex mutex_renderer;
    AudioRendererParams rendererParams_ ;
    // std::thread *rendererThread_ = nullptr;
};
} // namespace AudioStandard
} // namespace OHOS

#endif // AUDIO_CAPTURER_UNIT_TEST_H

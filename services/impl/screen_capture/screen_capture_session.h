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

#ifndef OHOS_SHARING_SCREEN_CAPTURE_SESSION_H
#define OHOS_SHARING_SCREEN_CAPTURE_SESSION_H

#include "agent/session/base_session.h"

namespace OHOS {
namespace Sharing {
class ScreenCaptureSession : public BaseSession {
public:
    ScreenCaptureSession();
    ~ScreenCaptureSession() override;

    int32_t HandleEvent(SharingEvent &event) override;

    void UpdateOperation(SessionStatusMsg::Ptr &statusMsg) override;
    void UpdateMediaStatus(SessionStatusMsg::Ptr &statusMsg) override;

protected:
    void NotifyProsumerInit(SessionStatusMsg::Ptr &statusMsg);
    void NotifyAgentSessionStatus(SessionStatusMsg::Ptr &statusMsg);

private:
    void HandleSessionInit(SharingEvent &event);
    void HandleProsumerInitState(SharingEvent &event);

private:
    uint64_t screenId_ = 0;
    MediaType captureType_ = MEDIA_TYPE_AV;
    AudioFormat audioFormat_ = AUDIO_8000_8_1;
    VideoFormat videoFormat_ = VIDEO_1920X1080_30;
};

} // namespace Sharing
} // namespace OHOS
#endif

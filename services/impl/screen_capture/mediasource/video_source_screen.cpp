/*
 * Copyright (c) 2023 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#include "video_source_screen.h"
#include "screen.h"
#include "sharing_log.h"

namespace OHOS {
namespace Sharing {
using namespace std;

void VideoSourceScreen::ScreenGroupListener::OnChange(const std::vector<uint64_t> &screenIds,
                                                      Rosen::ScreenGroupChangeEvent event)
{
    SHARING_LOGD("trace.");
    switch (event) {
        case Rosen::ScreenGroupChangeEvent::ADD_TO_GROUP:
            SHARING_LOGD("ADD_TO_GROUP done!");
            break;
        case Rosen::ScreenGroupChangeEvent::REMOVE_FROM_GROUP:
            SHARING_LOGD("REMOVE_FROM_GROUP done!");
            break;
        case Rosen::ScreenGroupChangeEvent::CHANGE_GROUP:
            SHARING_LOGD("CHANGE_GROUP done!");
            break;
        default:
            break;
    }
}

VideoSourceScreen::~VideoSourceScreen()
{
    SHARING_LOGD("trace.");
    encoderSurface_ = nullptr;
}

int32_t VideoSourceScreen::InitScreenSource(const VideoSourceConfigure &configure)
{
    SHARING_LOGD("trace.");
    if (encoderSurface_ == nullptr) {
        SHARING_LOGE("encoderSurface_ is null!");
        return ERR_GENERAL_ERROR;
    }

    RegisterScreenGroupListener();
    CreateVirtualScreen(configure);
    srcScreenId_ = configure.srcScreenId_;
    return ERR_OK;
}

int32_t VideoSourceScreen::ReleaseScreenBuffer() const
{
    SHARING_LOGD("trace.");
    return ERR_OK;
}

int32_t VideoSourceScreen::RegisterScreenGroupListener()
{
    SHARING_LOGD("trace.");
    if (screenGroupListener_ == nullptr) {
        screenGroupListener_ = new ScreenGroupListener();
    }
    auto ret = Rosen::ScreenManager::GetInstance().RegisterScreenGroupListener(screenGroupListener_);
    if (ret != OHOS::Rosen::DMError::DM_OK) {
        SHARING_LOGE("RegisterScreenGroupListener Failed!");
        return ERR_GENERAL_ERROR;
    }
    SHARING_LOGD("Register successed!");
    return ERR_OK;
}

int32_t VideoSourceScreen::UnregisterScreenGroupListener() const
{
    SHARING_LOGD("trace.");
    auto ret = Rosen::ScreenManager::GetInstance().UnregisterScreenGroupListener(screenGroupListener_);
    if (ret != OHOS::Rosen::DMError::DM_OK) {
        SHARING_LOGE("UnregisterScreenGroupListener Failed!");
        return ERR_GENERAL_ERROR;
    }
    return ERR_OK;
}

uint64_t VideoSourceScreen::CreateVirtualScreen(const VideoSourceConfigure &configure)
{
    SHARING_LOGD("CreateVirtualScreen, width: %{public}u, height: %{public}u.", configure.screenWidth_,
                 configure.screenHeight_);
    std::string screenName = "MIRACAST_HOME_SCREEN";
    Rosen::VirtualScreenOption option = {screenName,
                                         configure.screenWidth_,
                                         configure.screenHeight_,
                                         DEFAULT_SCREEN_DENSITY,
                                         encoderSurface_,
                                         DEFAULT_SCREEN_FLAGS,
                                         false};

    screenId_ = Rosen::ScreenManager::GetInstance().CreateVirtualScreen(option);
    SHARING_LOGD("virtualScreen id is: %{public}" PRIu64 ".", screenId_);

    return screenId_;
}

int32_t VideoSourceScreen::DestroyVirtualScreen() const
{
    SHARING_LOGD("trace.");
    if (screenId_ == SCREEN_ID_INVALID) {
        SHARING_LOGE("Failed, invalid screenId!");
        return ERR_GENERAL_ERROR;
    }
    SHARING_LOGD("Destroy virtual screen, screenId: %{public}" PRIu64 ".", screenId_);
    Rosen::DMError err = Rosen::ScreenManager::GetInstance().DestroyVirtualScreen(screenId_);
    if (err != Rosen::DMError::DM_OK) {
        SHARING_LOGE("Destroy virtual screen failed, screenId:%{public}" PRIu64 "!", screenId_);
        return ERR_GENERAL_ERROR;
    }
    return ERR_OK;
}

void VideoSourceScreen::StartScreenSourceCapture()
{
    SHARING_LOGD("trace.");
    std::vector<uint64_t> mirrorIds;
    mirrorIds.push_back(screenId_);
    Rosen::ScreenId groupId = 0;
    Rosen::DMError err = Rosen::ScreenManager::GetInstance().MakeMirror(srcScreenId_, mirrorIds, groupId);
    if (err != Rosen::DMError::DM_OK) {
        SHARING_LOGE("MakeMirror failed, screenId:%{public}" PRIu64 "!", screenId_);
    }
}

void VideoSourceScreen::StopScreenSourceCapture()
{
    SHARING_LOGD("trace.");
    UnregisterScreenGroupListener();
    int32_t ret = DestroyVirtualScreen();
    if (ret != ERR_OK) {
        SHARING_LOGE("Destroy virtual screen failed!");
    } else {
        SHARING_LOGD("Destroy virtual screen success!");
    }
}

int32_t VideoSourceScreen::SetEncoderSurface(sptr<OHOS::Surface> surface)
{
    SHARING_LOGD("trace.");
    if (screenId_ == SCREEN_ID_INVALID) {
        SHARING_LOGE("Failed, invalid screenId!");
    }
    if (surface == nullptr) {
        SHARING_LOGE("Surface is nullptr!");
        return ERR_GENERAL_ERROR;
    }
    SHARING_LOGD("Set surface for virtual screen, screenId: %{public}" PRIu64 ".", screenId_);
    Rosen::DMError err = Rosen::ScreenManager::GetInstance().SetVirtualScreenSurface(screenId_, surface);
    if (err != Rosen::DMError::DM_OK) {
        SHARING_LOGE("Set surface for virtual screen failed, screenId:%{public}" PRIu64 "!", screenId_);
        return ERR_GENERAL_ERROR;
    }
    return ERR_OK;
}

void VideoSourceScreen::RemoveScreenFromGroup() const
{
    SHARING_LOGD("trace.");
    if (screenId_ != SCREEN_ID_INVALID) {
        SHARING_LOGE("Failed, invalid screenId!");
    }
    SHARING_LOGD("Remove screen from group, screenId: %{publid}" PRIu64 ".", screenId_);
    std::vector<uint64_t> screenIds;
    screenIds.push_back(screenId_);
    Rosen::DMError err = Rosen::ScreenManager::GetInstance().RemoveVirtualScreenFromGroup(screenIds);
    if (err != Rosen::DMError::DM_OK) {
        SHARING_LOGE("Remove screen from group failed, screenId:%{public}" PRIu64 "!", screenId_);
    }
}
} // namespace Sharing
} // namespace OHOS

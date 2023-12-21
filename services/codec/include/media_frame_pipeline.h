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

#ifndef OHOS_SHARING_MEDIA_FRAME_PIPELINE_H
#define OHOS_SHARING_MEDIA_FRAME_PIPELINE_H

#include <cstdint>
#include <list>
#include <shared_mutex>
#include <string>
#include "frame.h"

namespace OHOS {
namespace Sharing {

class FrameDestination;
class FrameSource : public std::enable_shared_from_this<FrameSource> {
public:
    FrameSource() = default;
    virtual ~FrameSource();

    void AddAudioDestination(std::shared_ptr<FrameDestination> dst);
    void RemoveAudioDestination(std::shared_ptr<FrameDestination> dst);

protected:
    void DeliverFrame(const Frame::Ptr &frame);

private:
    std::shared_mutex audioDstsMutex_;
    std::list<std::shared_ptr<FrameDestination>> audioDsts_;
};

class FrameDestination {
public:
    FrameDestination() = default;
    virtual ~FrameDestination() = default;

    virtual void OnFrame(const Frame::Ptr &frame) = 0;

    void UnsetAudioSource();
    void SetAudioSource(std::shared_ptr<FrameSource> src);

    bool HasAudioSource()
    {
        return audioSrc_ != nullptr;
    }

private:
    std::shared_mutex audioSrcMutex_;
    std::shared_ptr<FrameSource> audioSrc_ = nullptr;
};
} // namespace Sharing
} // namespace OHOS
#endif
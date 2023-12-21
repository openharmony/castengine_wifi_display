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

#ifndef OHOS_SHARING_SCENE_SERVICE_H
#define OHOS_SHARING_SCENE_SERVICE_H

#include <memory>
#include "scene_format.h"

namespace OHOS {
namespace Sharing {

class SceneServiceListener {
public:
    using Ptr = std::shared_ptr<SceneServiceListener>;

    virtual ~SceneServiceListener() = default;

    virtual int32_t OnServiceInfo(SceneFormat &formatData, SceneFormat &formatReply) = 0;
};

class SceneService {
public:
    using Ptr = std::shared_ptr<SceneService>;

    virtual ~SceneService() = default;

    uint32_t GetInteractionId()
    {
        SHARING_LOGD("trace.");
        return interactionId_;
    }

    void SetInteractionId(uint32_t interactionId)
    {
        SHARING_LOGD("trace.");
        interactionId_ = interactionId;
    }

    void SetListener(std::weak_ptr<SceneServiceListener> listener)
    {
        SHARING_LOGD("trace.");
        serviceListener_ = listener;
    }

public:
    virtual void HandleMsg(SceneFormat &formatData) = 0;

protected:
    uint32_t interactionId_ = -1;
    std::weak_ptr<SceneServiceListener> serviceListener_;
};

} // namespace Sharing
} // namespace OHOS
#endif
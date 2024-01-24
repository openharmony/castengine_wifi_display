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

#ifndef OHOS_SHARING_CHANNEL_MGR_H
#define OHOS_SHARING_CHANNEL_MGR_H

#include <memory>
#include <singleton.h>
#include <cstdint>
#include <unordered_map>
#include "event/handle_event_base.h"

namespace OHOS {
namespace Sharing {

class MediaChannel;
class ChannelManager : public Singleton<ChannelManager>,
                       public EventEmitter,
                       public HandleEventBase {
    friend class Singleton<ChannelManager>;

public:
    ChannelManager();
    virtual ~ChannelManager();

    void Init();
    int32_t HandleEvent(SharingEvent &event);
    std::shared_ptr<MediaChannel> GetMediaChannel(uint32_t mediaChannelId);

private:
    SharingErrorCode HandleMediaChannelCreate(SharingEvent &event);
    SharingErrorCode HandleMediaChannelDestroy(SharingEvent &event);

private:
    std::mutex mutex_;
    std::mutex mixMutex_;

    std::unordered_map<uint32_t, std::shared_ptr<MediaChannel>> mediaChannels_;
};

} // namespace Sharing
} // namespace OHOS
#endif
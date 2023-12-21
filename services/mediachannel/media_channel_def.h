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

#ifndef OHOS_SHARING_MEDIA_CHANNEL_DEF_H
#define OHOS_SHARING_MEDIA_CHANNEL_DEF_H
#include "common/const_def.h"
#include "utils/data_buffer.h"

namespace OHOS {
namespace Sharing {

enum TransportType { UDP_CLIENT, UDP_SERVER, TCP_CLIENT, TCP_SERVER };

constexpr size_t MAX_BUFFER_CAPACITY = 800;
constexpr size_t BUFFER_CAPACITY_INCREMENT = 50;

enum ProsumerNotifyStatus {
    PROSUMER_NOTIFY_INIT_SUCCESS,
    PROSUMER_NOTIFY_START_SUCCESS,
    PROSUMER_NOTIFY_STOP_SUCCESS,
    PROSUMER_NOTIFY_PAUSE_SUCCESS,
    PROSUMER_NOTIFY_RESUME_SUCCESS,
    PROSUMER_NOTIFY_DESTROY_SUCCESS,
    PROSUMER_NOTIFY_ERROR,
    PROSUMER_NOTIFY_PRIVATE_EVENT,
};

enum ProsumerOptRunningStatus {
    PROSUMER_INIT,
    PROSUMER_START,
    PROSUMER_PAUSE,
    PROSUMER_RESUME,
    PROSUMER_STOP,
    PROSUMER_DESTROY
};

enum PlayConntrollerNotifyStatus {
    CONNTROLLER_NOTIFY_ACCELERATION,
    CONNTROLLER_NOTIFY_KEYMOD_START,
    CONNTROLLER_NOTIFY_KEYMOD_STOP,
    CONNTROLLER_NOTIFY_DECODER_DIED,
};

struct MediaData {
    using Ptr = std::shared_ptr<MediaData>;

    bool isRaw;
    bool keyFrame;
    uint32_t ssrc;
    uint64_t pts;
    MediaType mediaType;
    DataBuffer::Ptr buff;
};

} // namespace Sharing
} // namespace OHOS
#endif
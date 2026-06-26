/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef OHOS_SHARING_SOURCE_SESSION_DEF_H
#define OHOS_SHARING_SOURCE_SESSION_DEF_H

#include "common/event_comm.h"
#include "impl/scene/wfd/wfd_def.h"
#include "wfd_session_def.h"

namespace OHOS {
namespace Sharing {

struct WfdSourceSessionEventMsg : public SessionEventMsg {
    using Ptr = std::shared_ptr<WfdSourceSessionEventMsg>;

    uint32_t sinkAgentId;
    uint16_t localPort = 0;
    uint16_t remotePort = 0;

    std::string ip;
    std::string mac;

    AudioFormat audioFormat = AUDIO_48000_16_2;
    VideoFormat videoFormat = VIDEO_1920X1080_30;
};

} // namespace Sharing
} // namespace OHOS
#endif

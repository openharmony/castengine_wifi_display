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

#ifndef OHOS_INTER_IPC_MESSAGE_BIND_MARCROS_H
#define OHOS_INTER_IPC_MESSAGE_BIND_MARCROS_H
#include "common/sharing_log.h"
namespace OHOS {
namespace Sharing {

#define BEGIN_IPC_MESSAGE_ENCODE_BIND                                                                                 \
    virtual int32_t OnIpcMessage(const int32_t nMsgType, MessageParcel &ipcMsg, std::shared_ptr<BaseMsg> &sharingMsg) \
    {                                                                                                                 \
        switch (nMsgType) {
#define IPC_MESSAGE_ENCODE_BIND(SERVICE_TYPE, MESSAGE_TYPE)                        \
    case MESSAGE_TYPE: {                                                           \
        auto message = static_pointer_cast<SERVICE_TYPE>(sharingMsg);              \
        if (message) {                                                             \
            message->IpcSerialize(ipcMsg);                                         \
        } else {                                                                   \
            SHARING_LOGE("encode error, message type: %{public}d.", MESSAGE_TYPE); \
        }                                                                          \
        break;                                                                     \
    }

#define END_IPC_MESSAGE_ENCODE_BIND                                                                     \
    default: {                                                                                          \
        SHARING_LOGE("cann't process, may be message type error, message type: %{public}d.", nMsgType); \
        break;                                                                                          \
    }                                                                                                   \
        }                                                                                               \
        return 0;                                                                                       \
        }

#define BEGIN_IPC_MESSAGE_DECODE_BIND                                                                                 \
    virtual int32_t OnIpcMessage(const int32_t nMsgType, std::shared_ptr<BaseMsg> &sharingMsg, MessageParcel &ipcMsg) \
    {                                                                                                                 \
        switch (nMsgType) {
#define IPC_MESSAGE_DECODE_BIND(SERVICE_TYPE, MESSAGE_TYPE)                        \
    case MESSAGE_TYPE: {                                                           \
        auto message = std::make_shared<SERVICE_TYPE>();                           \
        if (0 == message->IpcDeserialize(ipcMsg)) {                                \
            sharingMsg = std::move(message);                                       \
            return 0;                                                              \
        } else {                                                                   \
            SHARING_LOGE("decode error, message type: %{public}d.", MESSAGE_TYPE); \
        }                                                                          \
        break;                                                                     \
    }

#define END_IPC_MESSAGE_DECODE_BIND                                                                     \
    default: {                                                                                          \
        SHARING_LOGE("cann't process, may be message type error, message type: %{public}d.", nMsgType); \
        break;                                                                                          \
    }                                                                                                   \
        }                                                                                               \
        return 0;                                                                                       \
        }

} // namespace Sharing
} // namespace OHOS

#endif

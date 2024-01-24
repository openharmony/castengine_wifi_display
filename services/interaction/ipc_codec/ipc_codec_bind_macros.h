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

#ifndef OHOS_INTER_IPC_CODEC_BIND_MARCROS_H
#define OHOS_INTER_IPC_CODEC_BIND_MARCROS_H

#include "ipc_codec.h"

namespace OHOS {
namespace Sharing {

#define IPC_BIND_ATTR0                           \
    int32_t IpcSerialize(MessageParcel &pacel)   \
    {                                            \
        return 0;                                \
    }                                            \
    int32_t IpcDeserialize(MessageParcel &pacel) \
    {                                            \
        return 0;                                \
    }

#define IPC_BIND_ATTR(...)                                                \
    int32_t IpcSerialize(MessageParcel &pacel)                            \
    {                                                                     \
        return IpcSerialize(pacel, __VA_ARGS__);                          \
    }                                                                     \
    template <class T, class... Args>                                     \
    int32_t IpcSerialize(MessageParcel &pacel, T first, Args... last)     \
    {                                                                     \
        if (!IPC_CODEC::IpcEncodeBindAttr(pacel, first)) {                \
            return -1;                                                    \
        }                                                                 \
        return IpcSerialize(pacel, last...);                              \
    }                                                                     \
    template <class T>                                                    \
    int32_t IpcSerialize(MessageParcel &pacel, T t)                       \
    {                                                                     \
        if (!IPC_CODEC::IpcEncodeBindAttr(pacel, t)) {                    \
            return -1;                                                    \
        }                                                                 \
        return 0;                                                         \
    }                                                                     \
                                                                          \
    int32_t IpcDeserialize(MessageParcel &pacel)                          \
    {                                                                     \
        return IpcDeserialize(pacel, __VA_ARGS__);                        \
    }                                                                     \
    template <class T, class... Args>                                     \
    int32_t IpcDeserialize(MessageParcel &pacel, T &first, Args &...last) \
    {                                                                     \
        if (!IPC_CODEC::IpcDecodeBindAttr(pacel, first)) {                \
            return -1;                                                    \
        }                                                                 \
        return IpcDeserialize(pacel, last...);                            \
    }                                                                     \
    template <class T>                                                    \
    int32_t IpcDeserialize(MessageParcel &pacel, T &t)                    \
    {                                                                     \
        if (!IPC_CODEC::IpcDecodeBindAttr(pacel, t)) {                    \
            return -1;                                                    \
        }                                                                 \
        return 0;                                                         \
    }

} // namespace Sharing
} // namespace OHOS

#endif

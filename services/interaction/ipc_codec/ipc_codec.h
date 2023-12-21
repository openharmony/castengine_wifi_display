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

#ifndef OHOS_INTER_IPC_CODEC_H
#define OHOS_INTER_IPC_CODEC_H

#include "iremote_object.h"
#include "message_parcel.h"

namespace OHOS {
namespace Sharing {
namespace IPC_CODEC {

bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, bool attr);
bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, int8_t attr);
bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, int16_t attr);
bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, int32_t attr);
bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, int64_t attr);
bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, uint8_t attr);
bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, uint16_t attr);
bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, uint32_t attr);
bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, uint64_t attr);
bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, float attr);
bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, double attr);
bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, std::string &attr);

bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, sptr<IRemoteObject> &attr);

template <typename TYPE>
bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, TYPE &t)
{
    if (0 == t.IpcSerialize(pIpcMsg)) {
        return true;
    }

    return false;
}

template <typename TYPE>
bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, std::vector<TYPE> &vec)
{
    int32_t size = (int32_t)vec.size();
    IpcEncodeBindAttr(pIpcMsg, size);
    for (auto &item : vec) {
        if (!IpcEncodeBindAttr(pIpcMsg, item)) {
            return false;
        }
    }

    return true;
}

bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, bool &attr);
bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, int8_t &attr);
bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, int16_t &attr);
bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, int32_t &attr);
bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, int64_t &attr);
bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, uint8_t &attr);
bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, uint16_t &attr);
bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, uint32_t &attr);
bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, uint64_t &attr);
bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, float &attr);
bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, double &attr);
bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, std::string &attr);
bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, sptr<IRemoteObject> &attr);

template <typename TYPE>
bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, TYPE &t)
{
    if (0 == t.IpcDeserialize(pIpcMsg)) {
        return true;
    }

    return false;
}

template <typename TYPE>
bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, std::vector<TYPE> &vec)
{
    int32_t size;
    IpcDecodeBindAttr(pIpcMsg, size);
    vec.resize(size);

    for (int i = 0; i < size; i++) {
        if (!IpcDecodeBindAttr(pIpcMsg, vec[i])) {
            return false;
        }
    }

    return true;
}

} // namespace IPC_CODEC
} // namespace Sharing
} // namespace OHOS

#endif

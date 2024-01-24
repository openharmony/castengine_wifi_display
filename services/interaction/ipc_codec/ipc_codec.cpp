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

#include "ipc_codec.h"
#include "common/sharing_log.h"

namespace OHOS {
namespace Sharing {
namespace IPC_CODEC {

bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, bool attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.WriteBool(attr);
}

bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, int8_t attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.WriteInt8(attr);
}

bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, int16_t attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.WriteInt16(attr);
}

bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, int32_t attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.WriteInt32(attr);
}

bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, int64_t attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.WriteInt64(attr);
}

bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, uint8_t attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.WriteUint8(attr);
}

bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, uint16_t attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.WriteUint16(attr);
}

bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, uint32_t attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.WriteUint32(attr);
}

bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, uint64_t attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.WriteUint64(attr);
}

bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, float attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.WriteFloat(attr);
}

bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, double attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.WriteDouble(attr);
}

bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, std::string &attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.WriteString(attr);
}

bool IpcEncodeBindAttr(MessageParcel &pIpcMsg, sptr<IRemoteObject> &attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.WriteRemoteObject(attr);
}

bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, bool &attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.ReadBool(attr);
}

bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, int8_t &attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.ReadInt8(attr);
}

bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, int16_t &attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.ReadInt16(attr);
}

bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, int32_t &attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.ReadInt32(attr);
}

bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, int64_t &attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.ReadInt64(attr);
}

bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, uint8_t &attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.ReadUint8(attr);
}

bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, uint16_t &attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.ReadUint16(attr);
}

bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, uint32_t &attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.ReadUint32(attr);
}

bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, uint64_t &attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.ReadUint64(attr);
}

bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, float &attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.ReadFloat(attr);
}

bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, double &attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.ReadDouble(attr);
}

bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, std::string &attr)
{
    SHARING_LOGD("trace.");
    return pIpcMsg.ReadString(attr);
}

bool IpcDecodeBindAttr(MessageParcel &pIpcMsg, sptr<IRemoteObject> &attr)
{
    SHARING_LOGD("trace.");
    attr = pIpcMsg.ReadRemoteObject();
    if (attr == nullptr) {
        return false;
    }

    return true;
}

} // namespace IPC_CODEC
} // namespace Sharing
} // namespace OHOS
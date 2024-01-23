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

#ifndef OHOS_SHARING_BUFFER_READER_H
#define OHOS_SHARING_BUFFER_READER_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace OHOS {
namespace Sharing {
class DataBuffer {
public:
    using Ptr = std::shared_ptr<DataBuffer>;

    DataBuffer() = default;
    explicit DataBuffer(int32_t size);

    DataBuffer(const DataBuffer &other) noexcept;
    DataBuffer &operator=(const DataBuffer &other) noexcept;
    DataBuffer(DataBuffer &&other) noexcept;
    DataBuffer &operator=(DataBuffer &&other) noexcept;

    virtual ~DataBuffer();

    virtual uint8_t *Data()
    {
        return data_;
    }

    virtual int32_t Size() const
    {
        return size_;
    }

    int32_t Capacity()
    {
        return capacity_;
    }

    void UpdateSize(int32_t size)
    {
        if (size <= capacity_) {
            size_ = size;
        }
    }

    const char *Peek() const
    {
        return (char *)data_;
    }

    void SetSize(int32_t size)
    {
        size_ = size;
    }

    virtual void Clear()
    {
        if (data_) {
            delete[] data_;
            data_ = nullptr;
        }

        size_ = 0;
        capacity_ = 0;
    }

    void Append(const char *data, int32_t dataLen)
    {
        PushData(data, dataLen);
    }

    void Append(const uint8_t *data, int32_t dataLen)
    {
        PushData((const char *)data, dataLen);
    }

    void Append(uint8_t c)
    {
        PushData((char *)&c, 1);
    }

    void Assign(const char *data, int32_t dataLen)
    {
        ReplaceData(data, dataLen);
    }

    void Assign(uint8_t c)
    {
        ReplaceData((char *)&c, 1);
    }

public:
    void Resize(int32_t size);
    void SetCapacity(int32_t capacity);
    void PushData(const char *data, int32_t dataLen);
    void ReplaceData(const char *data, int32_t dataLen);

private:
    int32_t size_ = 0;
    int32_t capacity_ = 0;
    uint8_t *data_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS
#endif
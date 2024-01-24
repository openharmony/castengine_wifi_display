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

#include "data_buffer.h"
#include <cstring>
#include <unistd.h>
#include <securec.h>

namespace OHOS {
namespace Sharing {

DataBuffer::DataBuffer(int size)
{
    data_ = new uint8_t[size + 1];
    data_[size] = '\0';
    capacity_ = size;
    size_ = 0;
}

DataBuffer::DataBuffer(const DataBuffer &other) noexcept
{
    if (other.data_ && other.size_) {
        capacity_ = other.size_;
        data_ = new uint8_t[capacity_ + 1];
        memcpy_s(data_, capacity_ + 1, other.data_, other.size_);
        size_ = other.size_;
    }
}

DataBuffer &DataBuffer::operator=(const DataBuffer &other) noexcept
{
    if (this != &other) {
        if (other.data_ && other.size_) {
            capacity_ = other.size_;
            delete[] data_;
            data_ = new uint8_t[capacity_ + 1];
            memcpy_s(data_, capacity_ + 1, other.data_, other.size_);
            size_ = other.size_;
        }
    }

    return *this;
}

DataBuffer::DataBuffer(DataBuffer &&other) noexcept
{
    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}

DataBuffer &DataBuffer::operator=(DataBuffer &&other) noexcept
{
    if (this != &other) {
        if (data_) {
            delete[] data_;
        }
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    return *this;
}

DataBuffer::~DataBuffer()
{
    delete[] data_;
    capacity_ = 0;
    size_ = 0;
}

void DataBuffer::Resize(int size)
{
    if (size > capacity_) {
        capacity_ = size;
        auto data2 = new uint8_t[capacity_];
        if (data_ && size_ > 0) {
            auto ret = memcpy_s(data2, capacity_, data_, size_);
            if (ret != EOK) {
                return;
            }
            delete[] data_;
        }
        data_ = data2;
    } else if (size < capacity_) {
        if (data_) {
            delete[] data_;
        }
        capacity_ = size;
        data_ = new uint8_t[capacity_];
        size_ = 0;
    }
}

void DataBuffer::PushData(const char *data, int dataLen)
{
    if (!data) {
        return;
    }

    if (dataLen + size_ <= capacity_) {
        auto ret = memcpy_s(data_ + size_, capacity_, data, dataLen);
        if (ret != EOK) {
            return;
        }
        size_ += dataLen;
    } else {
        capacity_ = size_ + dataLen;
        auto newBuffer = new (std::nothrow) uint8_t[capacity_];
        if (!newBuffer) {
            return;
        }
        if (data_) {
            auto ret = memcpy_s(newBuffer, capacity_, data_, size_);
            if (ret != EOK) {
                return;
            }
        }
        auto ret = memcpy_s(newBuffer + size_, capacity_, data, dataLen);
        if (ret != EOK) {
            return;
        }
        delete[] data_;
        data_ = newBuffer;
        size_ = capacity_;
    }
}

void DataBuffer::ReplaceData(const char *data, int dataLen)
{
    if (!data) {
        return;
    }

    if (dataLen > capacity_) {
        if (data_)
            delete[] data_;
        capacity_ = dataLen;
        data_ = new uint8_t[capacity_];
    }

    auto ret = memcpy_s(data_, capacity_, data, dataLen);
    if (ret != EOK) {
        return;
    }
    size_ = dataLen;
}

void DataBuffer::SetCapacity(int capacity)
{
    if (data_) {
        delete[] data_;
    }
    
    if (capacity == 0) {
        return;
    }
    if (capacity > 0) {
        data_ = new uint8_t[capacity];
    }
    capacity_ = capacity;
}

} // namespace Sharing
} // namespace OHOS
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

#ifndef OHOS_SHARING_CIRCULAR_BUFFER_H
#define OHOS_SHARING_CIRCULAR_BUFFER_H

#include <cstddef>
#include <deque>
#include <memory>
#include "common/sharing_log.h"
namespace OHOS {
namespace Sharing {

template <class T>
class circular_buffer {
public:
    typedef typename std::deque<T>::iterator Iterator;

    circular_buffer() = default;

    explicit circular_buffer(int32_t capacity)
    {
        if (capacity <= 0) {
            return;
        } else {
            capacity_ = capacity;
        }
    }

    explicit circular_buffer(circular_buffer& copy)
    {
        capacity_ = copy.capacity();
        T *temp = copy.begin();
        while (temp != copy.end()) {
            circular_buffer_.push_back(*temp);
            temp++;
        }
    }

    ~circular_buffer()
    {
        if (!circular_buffer_.empty()) {
            circular_buffer_.clear();
        }
    }

private:
    size_t capacity_ = 0;
    std::deque<T> circular_buffer_;

public:
    void clear()
    {
        circular_buffer_.clear();
        SHARING_LOGD("size is %{public}zu", size());
        set_capacity(capacity_);
    }

    void pop_back()
    {
        circular_buffer_.pop_back();
    }

    void pop_front()
    {
        circular_buffer_.pop_front();
    }

    void push_back(T item)
    {
        if (size() >= capacity()) {
            circular_buffer_.pop_front();
        }

        circular_buffer_.push_back(item);
    }

    void push_front(T &item)
    {
        if (size() >= capacity()) {
            circular_buffer_.pop_back();
        }

        circular_buffer_.push_front(item);
    }

    void set_capacity(size_t new_capacity)
    {
        capacity_ = new_capacity;
    }

    bool full()
    {
        return size() == capacity_;
    }

    bool empty()
    {
        return size() == 0;
    }

    size_t size()
    {
        return circular_buffer_.size();
    }

    size_t reserve()
    {
        return capacity() - size();
    }

    size_t capacity()
    {
        return capacity_;
    }

    T &at(size_t index)
    {
        return circular_buffer_[index];
    }

    T &back()
    {
        return circular_buffer_.back();
    }

    T &front()
    {
        return circular_buffer_.front();
    }

    Iterator begin()
    {
        return circular_buffer_.begin();
    }

    Iterator end()
    {
        return circular_buffer_.end();
    }

    T &operator[](int32_t index)
    {
        return circular_buffer_[index];
    }

    circular_buffer &operator=(circular_buffer &copy)
    {
        if (this == &copy) {
            return *this;
        }

        clear();
        capacity_ = copy.capacity();
        T *temp = copy.begin();
        while (temp != copy.end()) {
            circular_buffer_.push_back(*temp);
            temp++;
        }
    }
};

} // namespace Sharing
} // namespace OHOS
#endif
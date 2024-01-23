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

#include "data_queue.h"
#include "common/media_log.h"
namespace OHOS {
namespace Sharing {

template <class T>
DataQueue<T>::DataQueue(const size_t sizeMax) : sizeMax_(sizeMax)
{
    quitFlag_ = false;
    finishFlag_ = false;
}

template <class T>
DataQueue<T>::~DataQueue()
{
    Quit();
}

template <class T>
bool DataQueue<T>::Push(const T &data)
{
    std::lock_guard<std::mutex> lock(mutex_);
    while (!quitFlag_ && !finishFlag_) {
        if (queue_.size() < sizeMax_) {
            queue_.push(std::move(data));
            emptyQueue_.notify_all();
            return true;
        } else {
            SHARING_LOGD("DataQueue push failed 1.");
            return false;
        }
    }

    SHARING_LOGD("DataQueue push failed.");
    return false;
}

template <class T>
bool DataQueue<T>::Pop(T &data)
{
    std::lock_guard<std::mutex> lock(mutex_);
    while (!quitFlag_) {
        if (!queue_.empty()) {
            data = std::move(queue_.front());
            queue_.pop();
            fullQueue_.notify_all();
            return true;
        } else if (queue_.empty() && finishFlag_) {
            return false;
        } else {
            return false;
        }
    }

    return false;
}

template <class T>
void DataQueue<T>::Finished()
{
    finishFlag_ = true;
    emptyQueue_.notify_all();
}

template <class T>
void DataQueue<T>::Quit()
{
    quitFlag_ = true;
    emptyQueue_.notify_all();
    fullQueue_.notify_all();
}

template <class T>
uint16_t DataQueue<T>::Size()
{
    return static_cast<int>(queue_.size());
}

template <class T>
bool DataQueue<T>::IsEmpty()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return (0 == queue_.size());
}

template <class T>
bool DataQueue<T>::Clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    while (!queue_.empty()) {
        queue_.pop();
    }

    return true;
}

template <class T>
std::shared_ptr<T> DataQueue<T>::Pop(void)
{
    std::unique_lock<std::mutex> lock(mutex_);
    std::shared_ptr<T> res = nullptr;
    while (!quitFlag_) {
        if (!queue_.empty()) {
            res = std::make_shared<T>(queue_.front());
            queue_.pop();
            fullQueue_.notify_all();
            return res;
        } else if (queue_.empty() && finishFlag_) {
            return res;
        } else {
            emptyQueue_.wait(lock);
        }
    }

    return nullptr;
}

} // namespace Sharing
} // namespace OHOS
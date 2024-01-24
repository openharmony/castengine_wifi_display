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

#ifndef OHOS_SHARING_DATA_QUEUE_H
#define OHOS_SHARING_DATA_QUEUE_H
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>

namespace OHOS {
namespace Sharing {

template <class T>
class DataQueue {
public:
    explicit DataQueue(const size_t sizeMax);
    DataQueue(const DataQueue &) = delete;
    DataQueue &operator=(const DataQueue &) = delete;
    ~DataQueue();

    void Quit();
    void Finished();
    bool Clear();
    bool IsEmpty();
    bool Pop(T &data);
    bool Push(const T &data);
    uint16_t Size();

private:
    std::shared_ptr<T> Pop(void);

protected:
    std::queue<T> queue_;

private:
    std::mutex mutex_;
    std::atomic_bool quitFlag_;
    std::atomic_bool finishFlag_;
    std::condition_variable fullQueue_;
    std::condition_variable emptyQueue_;
    typename std::queue<T>::size_type sizeMax_;
};

} // namespace Sharing
} // namespace OHOS
#endif
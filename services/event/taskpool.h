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

#ifndef OHOS_SHARING_THREAD_POOL_H
#define OHOS_SHARING_THREAD_POOL_H

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include "event_base.h"
#include "nocopyable.h"

namespace OHOS {
namespace Sharing {

class TaskPool : public NoCopyable {
public:
    using Task = int32_t(const SharingEvent &);
    using BindedTask = int32_t();

    TaskPool();
    ~TaskPool();

    virtual inline void SetMaxTaskNum(const size_t maxSize)
    {
        maxTaskNum_ = maxSize;
    }

    virtual inline size_t GetMaxTaskNum() const
    {
        return maxTaskNum_;
    }

    virtual inline size_t GetTaskNum() const
    {
        return tasks_.size();
    };

public:
    virtual void Stop();
    virtual void SetTimeoutInterval(uint32_t ms);
    virtual void PushTask(std::packaged_task<BindedTask> &task);

    virtual int32_t Start(int32_t threadsNum);

protected:
    virtual void TaskMainWorker();
    virtual bool IsOverload() const;

protected:
    bool isRunning_ = false;
    size_t maxTaskNum_ = 0;
    std::mutex taskMutex_;
    std::condition_variable hasTask_;
    std::condition_variable acceptNewTask_;
    std::chrono::milliseconds timeoutInterval_;
    std::vector<std::thread> threads_;
    std::deque<std::packaged_task<BindedTask>> tasks_;
};

} // namespace Sharing
} // namespace OHOS
#endif
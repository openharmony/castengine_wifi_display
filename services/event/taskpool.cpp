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

#include "taskpool.h"
namespace OHOS {
namespace Sharing {

TaskPool::TaskPool()
{
    SHARING_LOGD("trace.");
}

TaskPool::~TaskPool()
{
    SHARING_LOGD("trace.");
    if (isRunning_) {
        Stop();
    }
}

int32_t TaskPool::Start(int32_t threadsNum)
{
    SHARING_LOGD("trace.");
    if (!threads_.empty()) {
        SHARING_LOGE("Before start, theads is not empty.");
        return -1;
    }

    if (threadsNum <= 0) {
        SHARING_LOGE("threadNum is illegal, %{public}d.", threadsNum);
        return -1;
    }

    isRunning_ = true;
    threads_.reserve(threadsNum);
    for (int32_t i = 0; i < threadsNum; ++i) {
        threads_.push_back(std::thread(&TaskPool::TaskMainWorker, this));
        std::string name = "taskpool" + std::to_string(i);
        pthread_setname_np(threads_.back().native_handle(), name.c_str());
    }

    return 0;
}

void TaskPool::Stop()
{
    SHARING_LOGD("trace.");
    {
        std::unique_lock<std::mutex> lock(taskMutex_);
        isRunning_ = false;
        hasTask_.notify_all();
    }

    for (auto &e : threads_) {
        e.join();
    }
}

void TaskPool::PushTask(std::packaged_task<BindedTask> &task)
{
    SHARING_LOGD("trace.");
    if (threads_.empty()) {
    } else {
        std::unique_lock<std::mutex> lock(taskMutex_);
        while (IsOverload()) {
            SHARING_LOGE("task pool is over load.");
            acceptNewTask_.wait(lock);
        }
        tasks_.emplace_back(std::move(task));
        hasTask_.notify_one();
    }
}

bool TaskPool::IsOverload() const
{
    return (maxTaskNum_ > 0) && (tasks_.size() >= maxTaskNum_);
}

void TaskPool::TaskMainWorker()
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::mutex> lock(taskMutex_);
    while (isRunning_) {
        if (tasks_.empty() && isRunning_) {
            hasTask_.wait(lock);
        } else {
            std::packaged_task<BindedTask> task = std::move(tasks_.front());
            tasks_.pop_front();
            acceptNewTask_.notify_one();
            lock.unlock();
            SHARING_LOGD("task main worker unlocked.");
            task();
            lock.lock();
            SHARING_LOGD("task main worker locked.");
        }
    }
}

void TaskPool::SetTimeoutInterval(uint32_t ms)
{
    SHARING_LOGD("trace.");
    timeoutInterval_ = std::chrono::milliseconds(ms);
}

} // namespace Sharing
} // namespace OHOS

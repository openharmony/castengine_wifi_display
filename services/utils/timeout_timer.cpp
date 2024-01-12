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

#include "timeout_timer.h"
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <sys/time.h>
#include "common/media_log.h"
namespace OHOS {
namespace Sharing {

#define TIMER_TIMEOUT 2
TimeoutTimer::TimeoutTimer(std::string info)
{
    SHARING_LOGD("trace.");
    thread_ = std::make_unique<std::thread>(&TimeoutTimer::MainLoop, this);
    pthread_setname_np(thread_->native_handle(), info.c_str());
}

TimeoutTimer::~TimeoutTimer()
{
    SHARING_LOGD("dtor in %{public}d.", state_);
    {
        std::unique_lock<std::mutex> taskLock(taskMutex_);
        if (state_ == State::WAITING) {
            state_ = State::EXITED;
            taskSignal_.notify_all();
        } else if (state_ == State::WORKING) {
            state_ = State::EXITED;
            cancelSignal_.notify_all();
        } else {
            state_ = State::EXITED;
        }
    }

    SHARING_LOGD("thread join %{public}d.", state_);
    if (thread_->joinable()) {
        thread_->join();
    }
    SHARING_LOGD("dtor out %{public}d.", state_);
}

void TimeoutTimer::StartTimer(int timeout, std::string info, std::function<void()> callback, bool reuse)
{
    SHARING_LOGD("add timeout timer (%{public}s).", info.c_str());
    std::lock_guard<std::mutex> lock(taskMutex_);
    reuse_ = reuse;
    timeout_ = timeout;
    if (state_ == State::WORKING) {
        SHARING_LOGD("cancel timeout timer (%{public}s).", taskName_.c_str());
        state_ = State::CANCELLED;
        cancelSignal_.notify_all();

        std::unique_lock<std::mutex> waitLock(waitMutex_);
        waitSignal_.wait_for(waitLock, std::chrono::milliseconds(TIMER_TIMEOUT));
    }
    taskName_ = std::move(info);
    if (callback) {
        callback_ = std::move(callback);
    }
    taskSignal_.notify_all();
    SHARING_LOGD("start timeout timer (%{public}s) leave.", info.c_str());
}

void TimeoutTimer::StopTimer()
{
    SHARING_LOGD("cancel timeout timer (%{public}s).", taskName_.c_str());
    std::lock_guard<std::mutex> lock(taskMutex_);

    if (state_ == State::WORKING) {
        state_ = State::CANCELLED;
        cancelSignal_.notify_all();
        std::unique_lock<std::mutex> waitLock(waitMutex_);
        waitSignal_.wait_for(waitLock, std::chrono::milliseconds(TIMER_TIMEOUT));
    }
    SHARING_LOGD("cancel timeout timer (%{public}s) leave.", taskName_.c_str());
}

void TimeoutTimer::MainLoop()
{
    SHARING_LOGD("trace.");
    while (state_ != State::EXITED) {
        std::unique_lock<std::mutex> taskLock(taskMutex_);
        if (state_ == State::EXITED) {
            break;
        }

        state_ = State::WAITING;
        waitSignal_.notify_all();
        if (!reuse_) {
            taskSignal_.wait(taskLock);
        }
    
        if (state_ == State::EXITED) {
            break;
        }

        state_ = State::WORKING;
        SHARING_LOGI("start timeout timer(%{public}s).", taskName_.c_str());
        cancelSignal_.wait_for(taskLock, std::chrono::seconds(timeout_));
        if (state_ == State::WORKING && callback_) {
            SHARING_LOGI("invoke timeout timer(%{public}s) callback.", taskName_.c_str());
            callback_();
        }
        SHARING_LOGI("end timeout timer(%{public}s).", taskName_.c_str());
    }
    SHARING_LOGD("exit.");
}

void TimeoutTimer::SetTimeoutCallback(std::function<void()> callback)
{
    callback_ = std::move(callback);
}
} // namespace Sharing
} // namespace OHOS
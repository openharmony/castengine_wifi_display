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

#ifndef OHOS_SHARING_TIMEOUT_TIMER_CPP
#define OHOS_SHARING_TIMEOUT_TIMER_CPP

#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <utility>

namespace OHOS {
namespace Sharing {
class TimeoutTimer {
public:
    explicit TimeoutTimer(std::string info = "TimeoutTimer");
    ~TimeoutTimer();

    void StopTimer();
    void SetTimeoutCallback(std::function<void()> callback);
    void StartTimer(int timeout, std::string info = "none", std::function<void()> callback = nullptr,
        bool reuse = false);

private:
    void MainLoop();

private:
    enum State { INIT, WAITING, WORKING, CANCELLED, EXITED };

    bool reuse_ = false;

    int timeout_ = 0;

    std::string taskName_;
    std::mutex taskMutex_;
    std::mutex waitMutex_;
    std::mutex cancelMutex_;
    std::mutex operateMutex_;
    std::function<void()> callback_;
    std::condition_variable taskSignal_;
    std::condition_variable waitSignal_;
    std::condition_variable cancelSignal_;
    std::unique_ptr<std::thread> thread_;

    State state_ = INIT;
};
} // namespace Sharing
} // namespace OHOS
#endif // OHOS_SHARING_TIMEOUT_TIMER_CPP
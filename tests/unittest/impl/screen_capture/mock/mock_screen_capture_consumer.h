/*
 * Copyright (c) 2024 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_SHARING_MOCK_SCREEN_CAPTURE_CONSUMER_H
#define OHOS_SHARING_MOCK_SCREEN_CAPTURE_CONSUMER_H

#include <gmock/gmock.h>
#include "buffer_dispatcher.h"
#include "mock_media_channel.h"
#define private public
#define protected public
#include "screen_capture_consumer.h"
#undef private
#undef protected

namespace OHOS {
namespace Sharing {

class MockScreenCaptureConsumer : public ScreenCaptureConsumer {
public:
    void Initialize(void)
    {
        mediaChannel_ = std::make_shared<MockMediaChannel>();
        dispatcher_ = std::make_shared<BufferDispatcher>();
        std::weak_ptr<MockMediaChannel> weakListener(mediaChannel_);
        SetConsumerListener(weakListener);
    }

    void DeInitialize(void)
    {
        dispatcher_ = nullptr;
        mediaChannel_ = nullptr;
    }
public:
    std::shared_ptr<MockMediaChannel> mediaChannel_;
    std::shared_ptr<BufferDispatcher> dispatcher_;
};

} // namespace Sharing
} // namespace OHOS
#endif

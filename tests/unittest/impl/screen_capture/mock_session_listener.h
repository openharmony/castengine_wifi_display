/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef OHOS_SHARING_MOCK_SESSION_LISTENER_H
#define OHOS_SHARING_MOCK_SESSION_LISTENER_H

#include "agent/session/isession_listener.h"
#include "gmock/gmock.h"

namespace OHOS {
namespace Sharing {

/**
 * @brief Mock ISessionListener 用于测试
 */
class MockSessionListener : public ISessionListener {
public:
    MOCK_METHOD(void, OnSessionNotify, (const SessionStatusMsg::Ptr &statusMsg), (override));
};

} // namespace Sharing
} // namespace OHOS

#endif // OHOS_SHARING_MOCK_SESSION_LISTENER_H

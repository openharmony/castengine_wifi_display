/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef OHOS_SHARING_SCREEN_CAPTURE_SESSION_DT_TEST_H
#define OHOS_SHARING_SCREEN_CAPTURE_SESSION_DT_TEST_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "screen_capture_session.h"
#include "screen_capture_def.h"
#include "mock_session_listener.h"

namespace OHOS {
namespace Sharing {

/**
 * @brief 屏幕捕获会话设备测试类
 *
 * 该类包含对ScreenCaptureSession的各种DT测试用例，测试覆盖：
 * 1. 构造和销毁
 * 2. 事件处理（所有分支）
 * 3. 状态管理
 * 4. 媒体控制
 * 5. 资源释放
 * 6. 错误处理
 * 7. 边界条件
 */
class ScreenCaptureSessionDTTest : public testing::Test {
public:
    /**
     * @brief 测试夹具的初始化
     */
    void SetUp() override;
    
    /**
     * @brief 测试夹具的清理
     */
    void TearDown() override;

protected:
    ScreenCaptureSession::Ptr session_;        /**< 被测试的屏幕捕获会话对象 */
};

} // namespace Sharing
} // namespace OHOS

#endif // OHOS_SHARING_SCREEN_CAPTURE_SESSION_DT_TEST_H

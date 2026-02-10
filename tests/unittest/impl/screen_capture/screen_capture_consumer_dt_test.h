/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef OHOS_SHARING_SCREEN_CAPTURE_CONSUMER_DT_TEST_H
#define OHOS_SHARING_SCREEN_CAPTURE_CONSUMER_DT_TEST_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "screen_capture_consumer.h"
#include "screen_capture_def.h"

namespace OHOS {
namespace Sharing {

/**
 * @brief 屏幕捕获消费者设备测试类
 *
 * 该类包含对ScreenCaptureConsumer的各种DT测试用例，测试覆盖：
 * 1. 构造和销毁
 * 2. 初始化功能
 * 3. 启动和停止控制
 * 4. 事件处理
 * 5. 数据处理
 * 6. 状态管理
 * 7. 资源释放
 * 8. 错误处理
 * 9. 边界条件
 */
class ScreenCaptureConsumerDTTest : public testing::Test {
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
    ScreenCaptureConsumer::Ptr consumer_;        /**< 被测试的屏幕捕获消费者对象 */
};

} // namespace Sharing
} // namespace OHOS

#endif // OHOS_SHARING_SCREEN_CAPTURE_CONSUMER_DT_TEST_H
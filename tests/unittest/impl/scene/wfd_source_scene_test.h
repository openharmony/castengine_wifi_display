/*
 * Copyright (c) 2024 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#ifndef OHOS_SHARING_WFD_SOURCE_SCENE_TEST_H
#define OHOS_SHARING_WFD_SOURCE_SCENE_TEST_H

#include <gtest/gtest.h>
#include "mock/mock_wfd_source_scene.h"

namespace OHOS {
namespace Sharing {

class WfdSourceSceneTest : public testing::Test {
public:
    // SetUpTestCase: Called before all test cases
    static void SetUpTestCase(void);
    // TearDownTestCase: Called after all test case
    static void TearDownTestCase(void);
    // SetUp: Called before each test cases
    void SetUp(void);
    // TearDown: Called after each test cases
    void TearDown(void);
    void BackupConnDev(void);
    void RestoreConnDev(void);

public:
    static inline std::shared_ptr<MockWfdSourceScene> sourceScene_;
    ConnectionInfo connectionInfo;
};

} // namespace Sharing
} // namespace OHOS
#endif

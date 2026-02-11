/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef TESTS_UNITTEST_CODEC_MOCK_MOCK_GLOBAL_H
#define TESTS_UNITTEST_CODEC_MOCK_MOCK_GLOBAL_H

#include "mock_libavcodec.h"
#include "mock_libswresample.h"
#include "mock_libavutil.h"

namespace OHOS {
namespace Sharing {

// Global mock manager class
class MockGlobalManager {
public:
    static MockGlobalManager& GetInstance()
    {
        static MockGlobalManager instance;
        return instance;
    }

    void SetMocks(MockLibavCodec* codec, MockLibSwresample* swr, MockLibavUtil* util)
    {
        mockCodec_ = codec;
        mockSwr_ = swr;
        mockUtil_ = util;
    }

    MockLibavCodec* GetCodecMock() const { return mockCodec_; }
    MockLibSwresample* GetSwrMock() const { return mockSwr_; }
    MockLibavUtil* GetUtilMock() const { return mockUtil_; }

private:
    MockGlobalManager() = default;
    ~MockGlobalManager() = default;
    MockGlobalManager(const MockGlobalManager&) = delete;
    MockGlobalManager& operator=(const MockGlobalManager&) = delete;

    MockLibavCodec* mockCodec_ = nullptr;
    MockLibSwresample* mockSwr_ = nullptr;
    MockLibavUtil* mockUtil_ = nullptr;
};

} // namespace Sharing
} // namespace OHOS

#endif // TESTS_UNITTEST_CODEC_MOCK_MOCK_GLOBAL_H
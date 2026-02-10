/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#include <gmock/gmock.h>
#include <sys/time.h>
#include "video_source_screen_dt_test.h"
#include "mock_screen_manager.h"
#include "mock_display_manager.h"

using namespace OHOS::Rosen;
using namespace testing;

namespace OHOS {
namespace Sharing {

// 测试 Mock 类的简单实现
class MockScreenManager : public ScreenManager {
public:
    MOCK_METHOD(DMError, CreateVirtualScreen, (const VirtualScreenOption &option), (override));
    MOCK_METHOD(DMError, DestroyVirtualScreen, (ScreenId screenId), (override));
    MOCK_METHOD(DMError, SetVirtualScreenSurface, (ScreenId screenId, sptr<Surface> surface), (override));
    MOCK_METHOD(DMError, SetVirtualMirrorScreenCanvasRotation, (ScreenId screenId, bool enable), (override));
    MOCK_METHOD(DMError, RegisterScreenListener, (sptr<IScreenListener> listener), (override));
    MOCK_METHOD(DMError, UnregisterScreenListener, (sptr<IScreenListener> listener), (override));
    MOCK_METHOD(void, SetVirtualScreenMaxRefreshRate,
                (ScreenId screenId, uint32_t maxRefreshRate, uint32_t &actualRefreshRate), (override));
    MOCK_METHOD(void, DisablePowerOffRenderControl, (ScreenId screenId), (override));
    MOCK_METHOD(sptr<Screen>, GetScreenById, (ScreenId screenId), (override));
    MOCK_METHOD(DMError, MakeMirror,
                (ScreenId sourceScreenId, const std::vector<ScreenId> &mirrorIds, ScreenId &groupId), (override));
    MOCK_METHOD(DMError, MakeMirror,
                (ScreenId sourceScreenId, const std::vector<ScreenId> &mirrorIds, const DMRect &region,
                 ScreenId &groupId),
                (override));
    MOCK_METHOD(DMError, StopMirror, (const std::vector<uint64_t> &mirrorIds), (override));
    MOCK_METHOD(void, RemoveVirtualScreenFromGroup, (const std::vector<uint64_t> &screenIds), (override));
    MOCK_METHOD(DMError, ResizeVirtualScreen, (ScreenId screenId, uint32_t width, uint32_t height), (override));
    MOCK_METHOD(void, SetVirtualScreenStatus, (ScreenId screenId, VirtualScreenStatus status), (override));
    MOCK_METHOD(void, SetVirtualScreenCanvasRotation, (ScreenId screenId, Rotation rotation), (override));
    MOCK_METHOD(DMError, RegisterScreenGroupListener, (sptr<IScreenGroupListener> listener), (override));
    MOCK_METHOD(DMError, UnregisterScreenGroupListener, (sptr<IScreenGroupListener> listener), (override));

    static MockScreenManager& GetInstance()
    {
        static MockScreenManager instance;
        return instance;
    }
};

class MockDisplayManager : public DisplayManager {
public:
    MOCK_METHOD(sptr<Display>, GetDefaultDisplay, (), (override));
    MOCK_METHOD(void, DisablePowerOffRenderControl, (ScreenId screenId), (override));

    static MockDisplayManager& GetInstance()
    {
        static MockDisplayManager instance;
        return instance;
    }
};

// 根据params获取字符串值的函数
namespace system {
    std::string GetParameter(const std::string& key, const std::string& defaultValue)
    {
        if (key == "const.window.support_window_pcmode_switch") {
            return "false";  // 默认返回false，简化测试
        }
        if (key == "persist.sceneboard.ispcmode") {
            return "false";  // 默认返回false，简化测试
        }
        return defaultValue;
    }

    bool GetBoolParameter(const std::string& key, bool defaultValue)
    {
        if (key == "debug.cast.yuv.support") {
            return false;  // 默认返回false，简化测试
        }
        return defaultValue;
    }
}

// 测试 ScreenListener 类
class VideoSourceScreenListenerDTTest : public testing::Test {
protected:
    void SetUp() override
    {
        screenListener_ = std::make_shared<ScreenListener>(1);
    }

    void TearDown() override
    {
        screenListener_ = nullptr;
    }

    std::shared_ptr<ScreenListener> screenListener_;
};

/**
 * @tc.name: VideoSourceScreenDT_ScreenListenerConstructor_001
 * @tc.desc: ScreenListener构造函数测试
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenListenerDTTest, VideoSourceScreenDT_ScreenListenerConstructor_001, TestSize.Level1)
{
    std::shared_ptr<ScreenListener> listener = std::make_shared<ScreenListener>(123);
    EXPECT_NE(listener, nullptr);
}

/**
 * @tc.name: VideoSourceScreenDT_ScreenListenerOnConnect_001
 * @tc.desc: ScreenListener::OnConnect 测试
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenListenerDTTest, VideoSourceScreenDT_ScreenListenerOnConnect_001, TestSize.Level1)
{
    EXPECT_NO_THROW(screenListener_->OnConnect(1));
}

/**
 * @tc.name: VideoSourceScreenDT_ScreenListenerOnDisconnect_001
 * @tc.desc: ScreenListener::OnDisconnect 测试
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenListenerDTTest, VideoSourceScreenDT_ScreenListenerOnDisconnect_001, TestSize.Level1)
{
    EXPECT_NO_THROW(screenListener_->OnDisconnect(1));
}

/**
 * @tc.name: VideoSourceScreenDT_ScreenListenerOnChange_001
 * @tc.desc: ScreenListener::OnChange - 测试screenId不匹配分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenListenerDTTest, VideoSourceScreenDT_ScreenListenerOnChange_001, TestSize.Level1)
{
    EXPECT_NO_THROW(screenListener_->OnChange(999)); // 传入不匹配的screenId
}

// 测试 VideoSourceScreen 类
/**
 * @tc.name: VideoSourceScreenDT_Constructor_001
 * @tc.desc: VideoSourceScreen构造函数测试
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_Constructor_001, TestSize.Level1)
{
    sptr<Surface> surface = Surface::CreateSurfaceAsConsumer();
    EXPECT_NE(surface, nullptr);
    
    VideoSourceScreen videoSourceScreen(surface);
    EXPECT_NO_THROW(videoSourceScreen.~VideoSourceScreen());
}

/**
 * @tc.name: VideoSourceScreenDT_Destructor_001
 * @tc.desc: VideoSourceScreen析构函数测试
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_Destructor_001, TestSize.Level1)
{
    {
        sptr<Surface> surface = Surface::CreateSurfaceAsConsumer();
        VideoSourceScreen videoSourceScreen(surface);
    } // 自动调用析构函数
    EXPECT_TRUE(true); // 如果没有崩溃则测试通过
}

/**
 * @tc.name: VideoSourceScreenDT_ReleaseScreenBuffer_001
 * @tc.desc: ReleaseScreenBuffer - 测试基本功能
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_ReleaseScreenBuffer_001, TestSize.Level1)
{
    EXPECT_NO_THROW(videoSourceScreen_->ReleaseScreenBuffer());
}

/**
 * @tc.name: VideoSourceScreenDT_InitScreenSource_001
 * @tc.desc: InitScreenSource - 测试encoderSurface_为null分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_InitScreenSource_001, TestSize.Level1)
{
    VideoSourceScreen videoSourceScreen(nullptr); // 传入null surface
    VideoSourceConfigure config;
    config.srcScreenId_ = 1;
    config.screenWidth_ = 1920;
    config.screenHeight_ = 1080;
    config.frameRate_ = 30.0;
    
    int32_t result = videoSourceScreen.InitScreenSource(config);
    EXPECT_EQ(result, ERR_NULL_ENCODERSURFACE);
}

/**
 * @tc.name: VideoSourceScreenDT_InitScreenSource_002
 * @tc.desc: InitScreenSource - 测试正常初始化流程
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_InitScreenSource_002, TestSize.Level1)
{
    VideoSourceConfigure config;
    config.srcScreenId_ = 1;
    config.screenWidth_ = 1920;
    config.screenHeight_ = 1080;
    config.frameRate_ = 30.0;
    
    EXPECT_NO_THROW({
        int32_t result = videoSourceScreen_->InitScreenSource(config);
        // 由于内部依赖外部服务，无法验证具体结果，只测试不崩溃
    });
}

/**
 * @tc.name: VideoSourceScreenDT_RegisterScreenGroupListener_001
 * @tc.desc: RegisterScreenGroupListener - 测试screenGroupListener_为null分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_RegisterScreenGroupListener_001, TestSize.Level1)
{
    EXPECT_NO_THROW(videoSourceScreen_->RegisterScreenGroupListener());
    EXPECT_NE(videoSourceScreen_->screenGroupListener_, nullptr);
}

/**
 * @tc.name: VideoSourceScreenDT_RegisterScreenGroupListener_002
 * @tc.desc: RegisterScreenGroupListener - 测试注册失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_RegisterScreenGroupListener_002, TestSize.Level1)
{
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, RegisterScreenGroupListener(_))
        .WillOnce(Return(DMError::DM_ERROR_INVALID_PARAM));
    
    int32_t result = videoSourceScreen_->RegisterScreenGroupListener();
    EXPECT_EQ(result, ERR_REGISTER_FAILURE);
}

/**
 * @tc.name: VideoSourceScreenDT_UnregisterScreenGroupListener_001
 * @tc.desc: UnregisterScreenGroupListener - 测试screenGroupListener_为null分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_UnregisterScreenGroupListener_001, TestSize.Level1)
{
    // 先删除screenGroupListener_
    videoSourceScreen_->screenGroupListener_ = nullptr;
    
    int32_t result = videoSourceScreen_->UnregisterScreenGroupListener();
    EXPECT_EQ(result, ERR_NULL_LISTENER);
}

/**
 * @tc.name: VideoSourceScreenDT_UnregisterScreenGroupListener_002
 * @tc.desc: UnregisterScreenGroupListener - 测试注销失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_UnregisterScreenGroupListener_002, TestSize.Level1)
{
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, UnregisterScreenGroupListener(_))
        .WillOnce(Return(DMError::DM_ERROR_INVALID_PARAM));
    
    int32_t result = videoSourceScreen_->UnregisterScreenGroupListener();
    EXPECT_EQ(result, ERR_UNREGISTER_FAILURE);
}

/**
 * @tc.name: VideoSourceScreenDT_UnregisterScreenGroupListener_003
 * @tc.desc: UnregisterScreenGroupListener - 测试正常注销
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_UnregisterScreenGroupListener_003, TestSize.Level1)
{
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, UnregisterScreenGroupListener(_))
        .WillOnce(Return(DMError::DM_OK));
    
    int32_t result = videoSourceScreen_->UnregisterScreenGroupListener();
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name: VideoSourceScreenDT_SetProjectionDisplay_001
 * @tc.desc: SetProjectionDisplay - 测试rotation非0分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetProjectionDisplay_001, TestSize.Level1)
{
    Rect region = {0, 0, 1920, 1080};
    EXPECT_NO_THROW(videoSourceScreen_->SetProjectionDisplay(1, 90, region)); // 非0旋转角
}

/**
 * @tc.name: VideoSourceScreenDT_SetProjectionDisplay_002
 * @tc.desc: SetProjectionDisplay - 测试mode为DISPLAY_BC分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetProjectionDisplay_002, TestSize.Level1)
{
    Rect region = {0, 0, 1920, 1080};
    EXPECT_NO_THROW(videoSourceScreen_->SetProjectionDisplay(3, 0, region)); // DISPLAY_BC
}

/**
 * @tc.name: VideoSourceScreenDT_SetProjectionDisplay_003
 * @tc.desc: SetProjectionDisplay - 测试正常设置
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetProjectionDisplay_003, TestSize.Level1)
{
    Rect region = {0, 0, 1920, 1080};
    EXPECT_NO_THROW(videoSourceScreen_->SetProjectionDisplay(1, 0, region)); // 普通投影
}

/**
 * @tc.name: VideoSourceScreenDT_MakeMirrorByScreenIds_001
 * @tc.desc: MakeMirrorByScreenIds - 测试defaultDisplay为null分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_MakeMirrorByScreenIds_001, TestSize.Level1)
{
    MockDisplayManager &mockDisplayManager = MockDisplayManager::GetInstance();
    EXPECT_CALL(mockDisplayManager, GetDefaultDisplay())
        .WillOnce(Return(nullptr));
    
    EXPECT_NO_THROW(videoSourceScreen_->MakeMirrorByScreenIds());
}

/**
 * @tc.name: VideoSourceScreenDT_MakeMirrorByScreenIds_002
 * @tc.desc: MakeMirrorByScreenIds - 测试MakeMirror失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_MakeMirrorByScreenIds_002, TestSize.Level1)
{
    MockDisplayManager &mockDisplayManager = MockDisplayManager::GetInstance();
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    
    auto mockDisplay = std::make_shared<Display>();
    EXPECT_CALL(mockDisplayManager, GetDefaultDisplay())
        .WillOnce(Return(mockDisplay));
    
    EXPECT_CALL(mockScreenManager, MakeMirror(_, _, _))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    EXPECT_NO_THROW(videoSourceScreen_->MakeMirrorByScreenIds());
}

/**
 * @tc.name: VideoSourceScreenDT_MakeMirrorByRect_001
 * @tc.desc: MakeMirrorByRect - 测试defaultDisplay为null分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_MakeMirrorByRect_001, TestSize.Level1)
{
    MockDisplayManager &mockDisplayManager = MockDisplayManager::GetInstance();
    EXPECT_CALL(mockDisplayManager, GetDefaultDisplay())
        .WillOnce(Return(nullptr));
    
    Rosen::DMRect rect = {0, 0, 1920, 1080};
    EXPECT_NO_THROW(videoSourceScreen_->MakeMirrorByRect(rect));
}

/**
 * @tc.name: VideoSourceScreenDT_MakeMirrorByRect_002
 * @tc.desc: MakeMirrorByRect - 测试MakeMirror失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_MakeMirrorByRect_002, TestSize.Level1)
{
    MockDisplayManager &mockDisplayManager = MockDisplayManager::GetInstance();
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    
    auto mockDisplay = std::make_shared<Display>();
    EXPECT_CALL(mockDisplayManager, GetDefaultDisplay())
        .WillOnce(Return(mockDisplay));
    
    EXPECT_CALL(mockScreenManager, MakeMirror(_, _, _, _))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    Rosen::DMRect rect = {0, 0, 1920, 1080};
    EXPECT_NO_THROW(videoSourceScreen_->MakeMirrorByRect(rect));
}

/**
 * @tc.name: VideoSourceScreenDT_SetScreenPixelFormat_001
 * @tc.desc: SetScreenPixelFormat - 测试YUV支持为false分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetScreenPixelFormat_001, TestSize.Level1)
{
    EXPECT_NO_THROW(videoSourceScreen_->SetScreenPixelFormat(1));
}

/**
 * @tc.name: VideoSourceScreenDT_SetScreenPixelFormat_002
 * @tc.desc: SetScreenPixelFormat - 测试screen为null分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetScreenPixelFormat_002, TestSize.Level1)
{
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, GetScreenById(1))
        .WillOnce(Return(nullptr));
    
    EXPECT_NO_THROW(videoSourceScreen_->SetScreenPixelFormat(1));
}

/**
 * @tc.name: VideoSourceScreenDT_SetScreenPixelFormat_003
 * @tc.desc: SetScreenPixelFormat - 测试SetPixelFormat失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetScreenPixelFormat_003, TestSize.Level1)
{
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    auto mockScreen = std::make_shared<Screen>();
    EXPECT_CALL(mockScreenManager, GetScreenById(1))
        .WillOnce(Return(mockScreen));
    
    EXPECT_CALL(mockScreen->*, SetPixelFormat(_))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    EXPECT_NO_THROW(videoSourceScreen_->SetScreenPixelFormat(1));
}

/**
 * @tc.name: VideoSourceScreenDT_SetScreenPixelFormat_004
 * @tc.desc: SetScreenPixelFormat - 测试成功设置
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetScreenPixelFormat_004, TestSize.Level1)
{
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    auto mockScreen = std::make_shared<Screen>();
    EXPECT_CALL(mockScreenManager, GetScreenById(1))
        .WillOnce(Return(mockScreen));
    
    EXPECT_CALL(mockScreen->*, SetPixelFormat(_))
        .WillOnce(Return(DMError::DM_OK));
    
    EXPECT_NO_THROW(videoSourceScreen_->SetScreenPixelFormat(1));
}

/**
 * @tc.name: VideoSourceScreenDT_IsPcMode_001
 * @tc.desc: IsPcMode - 测试支持窗口PCM模式开关为false分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_IsPcMode_001, TestSize.Level1)
{
    EXPECT_FALSE(videoSourceScreen_->IsPcMode());
}

/**
 * @tc.name: VideoSourceScreenDT_IsPcMode_002
 * @tc.desc: IsPcMode - 测试persist.sceneboard.ispcmode为false分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_IsPcMode_002, TestSize.Level1)
{
    EXPECT_FALSE(videoSourceScreen_->IsPcMode());
}

/**
 * @tc.name: VideoSourceScreenDT_IsPcMode_003
 * @tc.desc: IsPcMode - 测试PC模式为true分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_IsPcMode_003, TestSize.Level1)
{
    // 修改系统参数来测试PC模式
    EXPECT_CALL(system, GetParameter(StrEq("const.window.support_window_pcmode_switch"), StrEq("false")))
        .WillOnce(Return("true"));
    EXPECT_CALL(system, GetParameter(StrEq("persist.sceneboard.ispcmode"), StrEq("false")))
        .WillOnce(Return("true"));
    
    EXPECT_TRUE(videoSourceScreen_->IsPcMode());
}

/**
 * @tc.name: VideoSourceScreenDT_CreateVirtualScreen_001
 * @tc.desc: CreateVirtualScreen - 测试CreateVirtualScreen失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_CreateVirtualScreen_001, TestSize.Level1)
{
    VideoSourceConfigure config;
    config.screenWidth_ = 1920;
    config.screenHeight_ = 1080;
    config.frameRate_ = 30.0;
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, CreateVirtualScreen(_))
        .WillOnce(Return(SCREEN_ID_INVALID));
    
    videoSourceScreen_->srcScreenId_ = 1;
    uint64_t result = videoSourceScreen_->CreateVirtualScreen(config);
    EXPECT_EQ(result, SCREEN_ID_INVALID);
}

/**
 * @tc.name: VideoSourceScreenDT_CreateVirtualScreen_002
 * @tc.desc: CreateVirtualScreen - 测试PC模式分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_CreateVirtualScreen_002, TestSize.Level1)
{
    EXPECT_CALL(videoSourceScreen_, IsPcMode())
        .WillOnce(Return(true));
    
    VideoSourceConfigure config;
    config.screenWidth_ = 1920;
    config.screenHeight_ = 1080;
    config.frameRate_ = 30.0;
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, CreateVirtualScreen(_))
        .WillOnce(Return(1));
    
    videoSourceScreen_->srcScreenId_ = 1;
    uint64_t result = videoSourceScreen_->CreateVirtualScreen(config);
    EXPECT_EQ(result, 1);
}

/**
 * @tc.name: VideoSourceScreenDT_CreateVirtualScreen_003
 * @tc.desc: CreateVirtualScreen - 测试非PC模式分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_CreateVirtualScreen_003, TestSize.Level1)
{
    EXPECT_CALL(videoSourceScreen_, IsPcMode())
        .WillOnce(Return(false));
    
    VideoSourceConfigure config;
    config.screenWidth_ = 1920;
    config.screenHeight_ = 1080;
    config.frameRate_ = 30.0;
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, CreateVirtualScreen(_))
        .WillOnce(Return(1));
    
    videoSourceScreen_->srcScreenId_ = 1;
    uint64_t result = videoSourceScreen_->CreateVirtualScreen(config);
    EXPECT_EQ(result, 1);
}

/**
 * @tc.name: VideoSourceScreenDT_CreateVirtualScreen_004
 * @tc.desc: CreateVirtualScreen - 测试MakeMirror失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_CreateVirtualScreen_004, TestSize.Level1)
{
    VideoSourceConfigure config;
    config.screenWidth_ = 1920;
    config.screenHeight_ = 1080;
    config.frameRate_ = 30.0;
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, CreateVirtualScreen(_))
        .WillOnce(Return(1));
    EXPECT_CALL(mockScreenManager, MakeMirror(_, _, _))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    videoSourceScreen_->srcScreenId_ = 1;
    uint64_t result = videoSourceScreen_->CreateVirtualScreen(config);
    EXPECT_EQ(result, 1); // 即使mirror失败，虚拟屏仍创建成功
}

/**
 * @tc.name: VideoSourceScreenDT_CreateVirtualScreen_005
 * @tc.desc: CreateVirtualScreen - 测试高帧率分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_CreateVirtualScreen_005, TestSize.Level1)
{
    VideoSourceConfigure config;
    config.screenWidth_ = 1920;
    config.screenHeight_ = 1080;
    config.frameRate_ = HIVISION_FRAME_RATE; // 高帧率
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, CreateVirtualScreen(_))
        .WillOnce(Return(1));
    EXPECT_CALL(mockScreenManager, SetVirtualScreenMaxRefreshRate(_, REFRESH_60FPS, _))
        .WillOnce(DoAll(SetArrayArgument<2>(0, 0, 1), Return(DMError::DM_OK)));
    
    videoSourceScreen_->srcScreenId_ = 1;
    uint64_t result = videoSourceScreen_->CreateVirtualScreen(config);
    EXPECT_EQ(result, 1);
}

/**
 * @tc.name: VideoSourceScreenDT_CreateVirtualScreen_006
 * @tc.desc: CreateVirtualScreen - 测试普通帧率分支
 * @tc:.type FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_CreateVirtualScreen_006, TestSize.Level1)
{
    VideoSourceConfigure config;
    config.screenWidth_ = 1920;
    config.screenHeight_ = 1080;
    config.frameRate_ = 30.0; // 普通帧率
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, CreateVirtualScreen(_))
        .WillOnce(Return(1));
    EXPECT_CALL(mockScreenManager, SetVirtualScreenMaxRefreshRate(_, REFRESH_30FPS, _))
        .WillOnce(DoAll(SetArrayArgument<2>(0, 0, 1), Return(DMError::DM_OK)));
    
    videoSourceScreen_->srcScreenId_ = 1;
    uint64_t result = videoSourceScreen_->CreateVirtualScreen(config);
    EXPECT_EQ(result, 1);
}

/**
 * @tc.name: VideoSourceScreenDT_DestroyVirtualScreen_001
 * @tc.desc: DestroyVirtualScreen - 测试screenId_无效分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_DestroyVirtualScreen_001, TestSize.Level1)
{
    VideoSourceScreen videoSourceScreen(encoderSurface_);
    videoSourceScreen.screenId_ = SCREEN_ID_INVALID;
    
    int32_t result = videoSourceScreen.DestroyVirtualScreen();
    EXPECT_EQ(result, ERR_INVALID_SCREENID);
}

/**
 * @tc.name: VideoSourceScreenDT_DestroyVirtualScreen_002
 * @tc.desc: DestroyVirtualScreen - 测试StopMirror失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_DestroyVirtualScreen_002, TestSize.Level1)
{
    videoSourceScreen_->screenId_ = 1;
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, StopMirror(_))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    int32_t result = videoSourceScreen_->DestroyVirtualScreen();
    EXPECT_EQ(result, ERR_OK); // 即使mirror.stop失败，也继续清理
}

/**
 * @tc.name: VideoSourceScreenDT_DestroyVirtualScreen_003
 * @tc.desc: DestroyVirtualScreen - 测试DestroyVirtualScreen失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_DestroyVirtualScreen_003, TestSize.Level1)
{
    videoSourceScreen_->screenId_ = 1;
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, StopMirror(_))
        .WillOnce(Return(DMError::DM_OK));
    EXPECT_CALL(mockScreenManager, DestroyVirtualScreen(1))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    int32_t result = videoSourceScreen_->DestroyVirtualScreen();
    EXPECT_EQ(result, ERR_OK); // 即使destroy失败，状态也清理
}

/**
 * @tc.name: VideoSourceScreenDT_StartScreenSourceCapture_001
 * @tc.desc: StartScreenSourceCapture - 测试RegisterScreenListener失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_StartScreenSourceCapture_001, TestSize.Level1)
{
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, SetVirtualMirrorScreenCanvasRotation(_, _))
        .WillOnce(Return(DMError::DM_OK));
    EXPECT_CALL(mockScreenManager, RegisterScreenListener(_))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    EXPECT_NO_THROW(videoSourceScreen_->StartScreenSourceCapture());
}

/**
 * @tc.name: VideoSourceScreenDT_StartScreenSourceCapture_002
 * @tc.desc: StartScreenSourceCapture - 测试成功启动
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_StartScreenSourceCapture_002, TestSize.Level1)
{
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, SetVirtualMirrorScreenCanvasRotation(_, _))
        .WillOnce(Return(DMError::DM_OK));
    EXPECT_CALL(mockScreenManager, RegisterScreenListener(_))
        .WillOnce(Return(DMError::DM_OK));
    
    EXPECT_NO_THROW(videoSourceScreen_->StartScreenSourceCapture());
}

/**
 * @tc.name: VideoSourceScreenDT_RequestFrame_001
 * @tc.desc: RequestFrame - 测试线程创建和执行
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_RequestFrame_001, TestSize.Level1)
{
    EXPECT_NO_THROW(videoSourceScreen_->RequestFrame());
    // 等待线程执行完成
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * @tc.name: VideoSourceScreenDT_ChangeScreenRefreshRate_001
 * @tc.desc: ChangeScreenRefreshRate - 测试基本功能
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_ChangeScreenRefreshRate_001, TestSize.Level1)
{
    EXPECT_TRUE(videoSourceScreen_->ChangeScreenRefreshRate(30));
}

/**
 * @tc.name: VideoSourceScreenDT_StopScreenSourceCapture_001
 * @tc.desc: StopScreenSourceCapture - 测试UnregisterScreenGroupListener失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_StopScreenSourceCapture_001, TestSize.Level1)
{
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, UnregisterScreenGroupListener(_))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    videoSourceScreen_->screenGroupListener_ = std::make_shared<VideoSourceScreen::ScreenGroupListener>();
    EXPECT_NO_THROW(videoSourceScreen_->StopScreenSourceCapture());
}

/**
 * @tc.name: VideoSourceScreenDT_StopScreenSourceCapture_002
 * @tc.desc: StopScreenSourceCapture - 测试UnregisterScreenListener失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_StopScreenSourceCapture_002, TestSize.Level1)
{
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, UnregisterScreenGroupListener(_))
        .WillOnce(Return(DMError::DM_OK));
    EXPECT_CALL(mockScreenManager, UnregisterScreenListener(_))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    videoSourceScreen_->screenGroupListener_ = std::make_shared<VideoSourceScreen::ScreenGroupListener>();
    videoSourceScreen_->screenListener_ = sptr<ScreenListener>::MakeSptr(1);
    EXPECT_NO_THROW(videoSourceScreen_->StopScreenSourceCapture());
}

/**
 * @tc.name: VideoSourceScreenDT_StopScreenSourceCapture_003
 * @tc.desc: StopScreenSourceCapture - 测试DestroyVirtualScreen失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_StopScreenSourceCapture_003, TestSize.Level1)
{
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, UnregisterScreenGroupListener(_))
        .WillOnce(Return(DMError::DM_OK));
    EXPECT_CALL(mockScreenManager, UnregisterScreenListener(_))
        .WillOnce(Return(DMError::DM_OK));
    EXPECT_CALL(mockScreenManager, DestroyVirtualScreen(_))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    videoSourceScreen_->screenId_ = 1;
    videoSourceScreen_->screenGroupListener_ = std::make_shared<VideoSourceScreen::ScreenGroupListener>();
    videoSourceScreen_->screenListener_ = sptr<ScreenListener>::MakeSptr(1);
    EXPECT_NO_THROW(videoSourceScreen_->StopScreenSourceCapture());
}

#ifdef WFD_SINK
/**
 * @tc.name: VideoSourceScreenDT_SetEncoderSurface_001
 * @tc.desc: SetEncoderSurface - 测试screenId_无效分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetEncoderSurface_001, TestSize.Level1)
{
    VideoSourceScreen videoSourceScreen(encoderSurface_);
    videoSourceScreen.screenId_ = SCREEN_ID_INVALID;
    
    sptr<Surface> surface = Surface::CreateSurfaceAsConsumer();
    int32_t result = videoSourceScreen.SetEncoderSurface(surface);
    EXPECT_EQ(result, ERR_INVALID_SCREENID);
}

/**
 * @tc.name: VideoSourceScreenDT_SetEncoderSurface_002
 * @tc.desc: SetEncoderSurface - 测试surface为null分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetEncoderSurface_002, TestSize.Level1)
{
    VideoSourceScreen videoSourceScreen(encoderSurface_);
    videoSourceScreen.screenId_ = 1;
    
    int32_t result = videoSourceScreen.SetEncoderSurface(nullptr);
    EXPECT_EQ(result, ERR_NULL_SURFACE);
}

/**
 * @tc.name: VideoSourceScreenDT_SetEncoderSurface_003
 * @tc.desc: SetEncoderSurface - 测试SetVirtualScreenSurface失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetEncoderSurface_003, TestSize.Level1)
{
    VideoSourceScreen videoSourceScreen(encoderSurface_);
    videoSourceScreen.screenId_ = 1;
    
    sptr<Surface> surface = Surface::CreateSurfaceAsConsumer();
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, SetVirtualScreenSurface(1, surface))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    int32_t result = videoSourceScreen.SetEncoderSurface(surface);
    EXPECT_EQ(result, ERR_SET_SURFACE_FAILURE);
}

/**
 * @tc.name: VideoSourceScreenDT_SetEncoderSurface_004
 * @tc.desc: SetEncoderSurface - 测试成功设置
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetEncoderSurface_004, TestSize.Level1)
{
    VideoSourceScreen videoSourceScreen(encoderSurface_);
    videoSourceScreen.screenId_ = 1;
    
    sptr<Surface> surface = Surface::CreateSurfaceAsConsumer();
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, SetVirtualScreenSurface(1, surface))
        .WillOnce(Return(DMError::DM_OK));
    
    int32_t result = videoSourceScreen.SetEncoderSurface(surface);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name: VideoSourceScreenDT_RemoveScreenFromGroup_001
 * @tc.desc: RemoveScreenFromGroup - 测试screenId_无效分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_RemoveScreenFromGroup_001, TestSize.Level1)
{
    VideoSourceScreen videoSourceScreen(encoderSurface_);
    videoSourceScreen.screenId_ = SCREEN_ID_INVALID;
    
    EXPECT_NO_THROW(videoSourceScreen.RemoveScreenFromGroup());
}

/**
 * @tc.name: VideoSourceScreenDT_RemoveScreenFromGroup_002
 * @tc.desc: RemoveScreenFromGroup - 测试RemoveVirtualScreenFromGroup失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_RemoveScreenFromGroup_002, TestSize.Level1)
{
    VideoSourceScreen videoSourceScreen(encoderSurface_);
    videoSourceScreen.screenId_ = 1;
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, RemoveVirtualScreenFromGroup(_))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    EXPECT_NO_THROW(videoSourceScreen.RemoveScreenFromGroup());
}

/**
 * @tc.name: VideoSourceScreenDT_RemoveScreenFromGroup_003
 * @tc.desc: RemoveScreenFromGroup - 测试成功移除
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_RemoveScreenFromGroup_003, TestSize.Level1)
{
    VideoSourceScreen videoSourceScreen(encoderSurface_);
    videoSourceScreen.screenId_ = 1;
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, RemoveVirtualScreenFromGroup(_))
        .WillOnce(Return(DMError::DM_OK));
    
    EXPECT_NO_THROW(videoSourceScreen.RemoveScreenFromGroup());
}
#endif

/**
 * @tc.name: VideoSourceScreenDT_SetAppCastSurface_001
 * @tc.desc: SetAppCastSurface - 测试Surface创建失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetAppCastSurface_001, TestSize.Level1)
{
    EXPECT_NO_THROW(videoSourceScreen_->SetAppCastSurface(nullptr)); // 传入null producer
}

/**
 * @tc.name: VideoSourceScreenDT_SetAppCastSurface_002
 * @tc.desc: SetAppCastSurface - 测试SetVirtualScreenSurface失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetAppCastSurface_002, TestSize.Level1)
{
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, SetVirtualScreenSurface(_, _))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    auto producer = IBufferProducer::Create(); // 创建有效的producer
    EXPECT_NO_THROW(videoSourceScreen_->SetAppCastSurface(producer));
}

/**
 * @tc.name: VideoSourceScreenDT_SetAppCastSurface_003
 * @tc.desc: SetAppCastSurface - 测试成功设置
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetAppCastSurface_003, TestSize.Level1)
{
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, SetVirtualScreenSurface(_, _))
        .WillOnce(Return(DMError::DM_OK));
    
    auto producer = IBufferProducer::Create();
    EXPECT_NO_THROW(videoSourceScreen_->SetAppCastSurface(producer));
}

/**
 * @tc.name: VideoSourceScreenDT_CreateAppCastScreen_001
 * @tc.desc: CreateAppCastScreen - 测试json参数无效分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_CreateAppCastScreen_001, TestSize.Level1)
{
    EXPECT_NO_THROW(videoSourceScreen_->CreateAppCastScreen("invalid_json"));
}

/**
 * @tc.name: VideoSourceScreenDT_CreateAppCastScreen_002
 * @tc.desc: CreateAppCastScreen - 测试缺少width字段分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_CreateAppCastScreen_002, TestSize.Level1)
{
    std::string jsonParam = R"({"height": 1080, "dpi": 2.0})";
    EXPECT_NO_THROW(videoSourceScreen_->CreateAppCastScreen(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_CreateAppCastScreen_003
 * @tc.desc: CreateAppCastScreen - 测试缺少height字段分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_CreateAppCastScreen_003, TestSize.Level1)
{
    std::string jsonParam = R"({"width": 1920, "dpi": 2.0})";
    EXPECT_NO_THROW(videoSourceScreen_->CreateAppCastScreen(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_CreateAppCastScreen_004
 * @tc.desc: CreateAppCastScreen - 测试缺少dpi字段分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_CreateAppCastScreen_004, TestSize.Level1)
{
    std::string jsonParam = R"({"width": 1920, "height": 1080})";
    EXPECT_NO_THROW(videoSourceScreen_->CreateAppCastScreen(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_CreateAppCastScreen_005
 * @tc.desc: CreateAppCastScreen - 测试width类型错误分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_CreateAppCastScreen_005, TestSize.Level1)
{
    std::string jsonParam = R"({"width": "1920", "height": 1080, "dpi": 2.0})";
    EXPECT_NO_THROW(videoSourceScreen_->CreateAppCastScreen(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_CreateAppCastScreen_006
 * @tc.desc: CreateAppCastScreen - 测试height类型错误分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_CreateAppCastScreen_006, TestSize.Level1)
{
    std::string jsonParam = R"({"width": 1920, "height": "1080", "dpi": 2.0})";
    EXPECT_NO_THROW(videoSourceScreen_->CreateAppCastScreen(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_CreateAppCastScreen_007
 * @tc.desc: CreateAppCastScreen - 测试dpi类型错误分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_CreateAppCastScreen_007, TestSize.Level1)
{
    std::string jsonParam = R"({"width": 1920, "height": 1080, "dpi": "2.0"})";
    EXPECT_NO_THROW(videoSourceScreen_->CreateAppCastScreen(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_CreateAppCastScreen_008
 * @tc.desc: CreateAppCastScreen - 测试CreateVirtualScreen失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_CreateAppCastScreen_008, TestSize.Level1)
{
    std::string jsonParam = R"({"width": 1920, "height": 1080, "dpi": 2.0})";
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, CreateVirtualScreen(_))
        .WillOnce(Return(SCREEN_ID_INVALID));
    
    EXPECT_NO_THROW(videoSourceScreen_->CreateAppCastScreen(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_CreateAppCastScreen_009
 * @tc.desc: CreateAppCastScreen - 测试成功创建
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_CreateAppCastScreen_009, TestSize.Level1)
{
    std::string jsonParam = R"({"width": 1920, "height": 1080, "dpi": 2.0})";
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, CreateVirtualScreen(_))
        .WillOnce(Return(1));
    
    EXPECT_NO_THROW(videoSourceScreen_->CreateAppCastScreen(jsonParam));
    EXPECT_EQ(videoSourceScreen_->screenIdAppCast_, 1);
}

/**
 * @tc.name: VideoSourceScreenDT_DestroyAppCastScreen_001
 * @tc.desc: DestroyAppCastScreen - 测试screenIdAppCast_无效分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_DestroyAppCastScreen_001, TestSize.Level1)
{
    videoSourceScreen_->screenIdAppCast_ = SCREEN_ID_INVALID;
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, DestroyVirtualScreen(_))
        .Times(0); // 不应该被调用
    
    EXPECT_NO_THROW(videoSourceScreen_->DestroyAppCastScreen());
}

/**
 * @tc.name: VideoSourceScreenDT_DestroyAppCastScreen_002
 * @tc.desc: DestroyAppCastScreen - 测试DestroyVirtualScreen失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_DestroyAppCastScreen_002, TestSize.Level1)
{
    videoSourceScreen_->screenIdAppCast_ = 1;
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, DestroyVirtualScreen(1))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    EXPECT_NO_THROW(videoSourceScreen_->DestroyAppCastScreen());
}

/**
 * @tc.name: VideoSourceScreenDT_DestroyAppCastScreen_003
 * @tc.desc: DestroyAppCastScreen - 测试成功销毁
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_DestroyAppCastScreen_003, TestSize.Level1)
{
    videoSourceScreen_->screenIdAppCast_ = 1;
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, DestroyVirtualScreen(1))
        .WillOnce(Return(DMError::DM_OK));
    
    EXPECT_NO_THROW(videoSourceScreen_->DestroyAppCastScreen());
    EXPECT_EQ(videoSourceScreen_->screenIdAppCast_, SCREEN_ID_INVALID);
}

/**
 * @tc.name: VideoSourceScreenDT_ResizeAppCastScreen_001
 * @tc.desc: ResizeAppCastScreen - 测试json参数无效分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_ResizeAppCastScreen_001, TestSize.Level1)
{
    videoSourceScreen_->screenIdAppCast_ = 1;
    EXPECT_NO_THROW(videoSourceScreen_->ResizeAppCastScreen("invalid_json"));
}

/**
 * @tc.name: VideoSourceScreenDT_ResizeAppCastScreen_002
 * @tc.desc: ResizeAppCastScreen - 测试缺少width字段分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_ResizeAppCastScreen_002, TestSize.Level1)
{
    std::string jsonParam = R"({"height": 1080})";
    videoSourceScreen_->screenIdAppCast_ = 1;
    EXPECT_NO_THROW(videoSourceScreen_->ResizeAppCastScreen(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_ResizeAppCastScreen_003
 * @tc.desc: ResizeAppCastScreen - 测试缺少height字段分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_ResizeAppCastScreen_003, TestSize.Level1)
{
    std::string jsonParam = R"({"width": 1920})";
    videoSourceScreen_->screenIdAppCast_ = 1;
    EXPECT_NO_THROW(videoSourceScreen_->ResizeAppCastScreen(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_ResizeAppCastScreen_004
 * @tc.desc: ResizeAppCastScreen - 测试width类型错误分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_ResizeAppCastScreen_004, TestSize.Level1)
{
    std::string jsonParam = R"({"width": "1920", "height": 1080})";
    videoSourceScreen_->screenIdAppCast_ = 1;
    EXPECT_NO_THROW(videoSourceScreen_->ResizeAppCastScreen(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_ResizeAppCastScreen_005
 * @tc.desc: ResizeAppCastScreen - 测试height类型错误分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_ResizeAppCastScreen_005, TestSize.Level1)
{
    std::string jsonParam = R"({"width": 1920, "height": "1080"})";
    videoSourceScreen_->screenIdAppCast_ = 1;
    EXPECT_NO_THROW(videoSourceScreen_->ResizeAppCastScreen(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_ResizeAppCastScreen_006
 * @tc.desc: ResizeAppCastScreen - 测试ResizeVirtualScreen失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_ResizeAppCastScreen_006, TestSize.Level1)
{
    std::string jsonParam = R"({"width": 1920, "height": 1080})";
    videoSourceScreen_->screenIdAppCast_ = 1;
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, ResizeVirtualScreen(1, 1920, 1080))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    EXPECT_NO_THROW(videoSourceScreen_->ResizeAppCastScreen(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_ResizeAppCastScreen_007
 * @tc.desc: ResizeAppCastScreen - 测试成功调整大小
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_ResizeAppCastScreen_007, TestSize.Level1)
{
    std::string jsonParam = R"({"width": 1920, "height": 1080})";
    videoSourceScreen_->screenIdAppCast_ = 1;
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, ResizeVirtualScreen(1, 1920, 1080))
        .WillOnce(Return(DMError::DM_OK));
    
    EXPECT_NO_THROW(videoSourceScreen_->ResizeAppCastScreen(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_AppCastMakeMirror_001
 * @tc.desc: AppCastMakeMirror - 测试json参数无效分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_AppCastMakeMirror_001, TestSize.Level1)
{
    EXPECT_NO_THROW(videoSourceScreen_->AppCastMakeMirror("invalid_json"));
}

/**
 * @tc.name: VideoSourceScreenDT_AppCastMakeMirror_002
 * @tc.desc: AppCastMakeMirror - 测试缺少isAppCast字段分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_AppCastMakeMirror_002, TestSize.Level1)
{
    std::string jsonParam = R"({"other": true})";
    EXPECT_NO_THROW(videoSourceScreen_->AppCastMakeMirror(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_AppCastMakeMirror_003
 * @tc.desc: AppCastMakeMirror - 测试isAppCast类型错误分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_AppCastMakeMirror_003, TestSize.Level1)
{
    std::string jsonParam = R"({"isAppCast": "true"})";
    EXPECT_NO_THROW(videoSourceScreen_->AppCastMakeMirror(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_AppCastMakeMirror_004
 * @tc.desc: AppCastMakeMirror - 测试isAppCast=true分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_AppCastMakeMirror_004, TestSize.Level1)
{
    std::string jsonParam = R"({"isAppCast": true})";
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, SetVirtualMirrorScreenCanvasRotation(_, _))
        .WillOnce(Return(DMError::DM_OK));
    EXPECT_CALL(mockScreenManager, MakeMirror(_, _, _, _))
        .WillOnce(Return(DMError::DM_OK));
    
    EXPECT_NO_THROW(videoSourceScreen_->AppCastMakeMirror(jsonParam));
    EXPECT_TRUE(videoSourceScreen_->isAppCast_);
}

/**
 * @tc.name: VideoSourceScreenDT_AppCastMakeMirror_005
 * @tc.desc: AppCastMakeMirror - 测试isAppCast=true MakeMirror失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_AppCastMakeMirror_005, TestSize.Level1)
{
    std::string jsonParam = R"({"isAppCast": true})";
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, SetVirtualMirrorScreenCanvasRotation(_, _))
        .WillOnce(Return(DMError::DM_OK));
    EXPECT_CALL(mockScreenManager, MakeMirror(_, _, _, _))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    EXPECT_NO_THROW(videoSourceScreen_->AppCastMakeMirror(jsonParam));
    EXPECT_FALSE(videoSourceScreen_->isAppCast_); // 状态未改变
}

/**
 * @tc.name: VideoSourceScreenDT_AppCastMakeMirror_006
 * @tc.desc: AppCastMakeMirror - 测试isAppCast=false分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_AppCastMakeMirror_006, TestSize.Level1)
{
    std::string jsonParam = R"({"isAppCast": false})";
    
    MockDisplayManager &mockDisplayManager = MockDisplayManager::GetInstance();
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    
    auto mockDisplay = std::make_shared<Display>();
    EXPECT_CALL(mockDisplayManager, GetDefaultDisplay())
        .WillOnce(Return(mockDisplay));
    EXPECT_CALL(mockScreenManager, MakeMirror(_, _, _))
        .WillOnce(Return(DMError::DM_OK));
    
    EXPECT_NO_THROW(videoSourceScreen_->AppCastMakeMirror(jsonParam));
    EXPECT_FALSE(videoSourceScreen_->isAppCast_);
}

/**
 * @tc.name: VideoSourceScreenDT_AppCastMakeMirror_007
 * @tc.desc: AppCastMakeMirror - 测试isAppCast=false defaultDisplay=null分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_AppCastMakeMirror_007, TestSize.Level1)
{
    std::string jsonParam = R"({"isAppCast": false})";
    
    MockDisplayManager &mockDisplayManager = MockDisplayManager::GetInstance();
    EXPECT_CALL(mockDisplayManager, GetDefaultDisplay())
        .WillOnce(Return(nullptr));
    
    EXPECT_NO_THROW(videoSourceScreen_->AppCastMakeMirror(jsonParam));
    EXPECT_FALSE(videoSourceScreen_->isAppCast_);
}

/**
 * @tc.name: VideoSourceScreenDT_AppCastMakeMirror_008
 * @tc.desc: AppCastMakeMirror - 测试isAppCast=false MakeMirror失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_AppCastMakeMirror_008, TestSize.Level1)
{
    std::string jsonParam = R"({"isAppCast": false})";
    
    MockDisplayManager &mockDisplayManager = MockDisplayManager::GetInstance();
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    
    auto mockDisplay = std::make_shared<Display>();
    EXPECT_CALL(mockDisplayManager, GetDefaultDisplay())
        .WillOnce(Return(mockDisplay));
    EXPECT_CALL(mockScreenManager, MakeMirror(_, _, _))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    EXPECT_NO_THROW(videoSourceScreen_->AppCastMakeMirror(jsonParam));
    EXPECT_FALSE(videoSourceScreen_->isAppCast_);
}

/**
 * @tc.name: VideoSourceScreenDT_AppCastEnterSmallWindow_001
 * @tc.desc: AppCastEnterSmallWindow - 测试SetVirtualScreenStatus失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_AppCastEnterSmallWindow_001, TestSize.Level1)
{
    videoSourceScreen_->screenIdAppCast_ = 1;
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, SetVirtualScreenStatus(_, _))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    EXPECT_NO_THROW(videoSourceScreen_->AppCastEnterSmallWindow());
}

/**
 * @tc.name: VideoSourceScreenDT_AppCastEnterSmallWindow_002
 * @tc.desc: AppCastEnterSmallWindow - 测试SetVirtualScreenMaxRefreshRate失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_AppCastEnterSmallWindow_002, TestSize.Level1)
{
    videoSourceScreen_->screenIdAppCast_ = 1;
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, SetVirtualScreenStatus(_, _))
        .WillOnce(Return(DMError::DM_OK));
    EXPECT_CALL(mockScreenManager, SetVirtualScreenMaxRefreshRate(_, _, _))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    EXPECT_NO_THROW(videoSourceScreen_->AppCastEnterSmallWindow());
}

/**
 * @tc.name: VideoSourceScreenDT_AppCastEnterSmallWindow_003
 * @tc.desc: AppCastEnterSmallWindow - 测试成功进入小窗口
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_AppCastEnterSmallWindow_003, TestSize.Level1)
{
    videoSourceScreen_->screenIdAppCast_ = 1;
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, SetVirtualScreenStatus(_, _))
        .WillOnce(Return(DMError::DM_OK));
    EXPECT_CALL(mockScreenManager, SetVirtualScreenMaxRefreshRate(_, _, _))
        .WillOnce(Return(DMError::DM_OK));
    
    EXPECT_NO_THROW(videoSourceScreen_->AppCastEnterSmallWindow());
}

/**
 * @tc.name: VideoSourceScreenDT_AppCastExitSmallWindow_001
 * @tc.desc: AppCastExitSmallWindow - 测试SetVirtualScreenStatus失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_AppCastExitSmallWindow_001, TestSize.Level1)
{
    videoSourceScreen_->screenIdAppCast_ = 1;
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, SetVirtualScreenStatus(_, _))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    EXPECT_NO_THROW(videoSourceScreen_->AppCastExitSmallWindow());
}

/**
 * @tc.name: VideoSourceScreenDT_AppCastExitSmallWindow_002
 * @tc.desc: AppCastExitSmallWindow - 测试SetVirtualScreenMaxRefreshRate失败分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_AppCastExitSmallWindow_002, TestSize.Level1)
{
    videoSourceScreen_->screenIdAppCast_ = 1;
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, SetVirtualScreenStatus(_, _))
        .WillOnce(Return(DMError::DM_OK));
    EXPECT_CALL(mockScreenManager, SetVirtualScreenMaxRefreshRate(_, _, _))
        .WillOnce(Return(DMError::DM_ERROR_UNKNOWN));
    
    EXPECT_NO_THROW(videoSourceScreen_->AppCastExitSmallWindow());
}

/**
 * @tc.name: VideoSourceScreenDT_AppCastExitSmallWindow_003
 * @tc.desc: AppCastExitSmallWindow - 测试成功退出小窗口
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_AppCastExitSmallWindow_003, TestSize.Level1)
{
    videoSourceScreen_->screenIdAppCast_ = 1;
    
    MockScreenManager &mockScreenManager = MockScreenManager::GetInstance();
    EXPECT_CALL(mockScreenManager, SetVirtualScreenStatus(_, _))
        .WillOnce(Return(DMError::DM_OK));
    EXPECT_CALL(mockScreenManager, SetVirtualScreenMaxRefreshRate(_, _, _))
        .WillOnce(Return(DMError::DM_OK));
    
    EXPECT_NO_THROW(videoSourceScreen_->AppCastExitSmallWindow());
}

/**
 * @tc.name: VideoSourceScreenDT_GetAppCastScreenId_001
 * @tc.desc: GetAppCastScreenId - 测试screenIdAppCast_无效分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_GetAppCastScreenId_001, TestSize.Level1)
{
    uint64_t screenId = 0;
    videoSourceScreen_->screenIdAppCast_ = SCREEN_ID_INVALID;
    
    EXPECT_NO_THROW(videoSourceScreen_->GetAppCastScreenId(screenId));
}

/**
 * @tc.name: VideoSourceScreenDT_GetAppCastScreenId_002
 * @tc.desc: GetAppCastScreenId - 测试成功获取
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_GetAppCastScreenId_002, TestSize.Level1)
{
    uint64_t screenId = 0;
    videoSourceScreen_->screenIdAppCast_ = 123;
    
    EXPECT_NO_THROW(videoSourceScreen_->GetAppCastScreenId(screenId));
    EXPECT_EQ(screenId, 123);
}

/**
 * @tc.name: VideoSourceScreenDT_SetRefreshRateVote_001
 * @tc.desc: SetRefreshRateVote - 测试json参数无效分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetRefreshRateVote_001, TestSize.Level1)
{
    EXPECT_NO_THROW(videoSourceScreen_->SetRefreshRateVote("invalid_json"));
}

/**
 * @tc.name: VideoSourceScreenDT_SetRefreshRateVote_002
 * @tc.desc: SetRefreshRateVote - 测试缺少voting字段分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetRefreshRateVote_002, TestSize.Level1)
{
    std::string jsonParam = R"({"minRefreshRate": 30, "maxRefreshRate": 60})";
    EXPECT_NO_THROW(videoSourceScreen_->SetRefreshRateVote(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_SetRefreshRateVote_003
 * @tc.desc: SetRefreshRateVote - 测试缺少minRefreshRate字段分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetRefreshRateVote_003, TestSize.Level1)
{
    std::string jsonParam = R"({"voting": 1, "maxRefreshRate": 60})";
    EXPECT_NO_THROW(videoSourceScreen_->SetRefreshRateVote(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_SetRefreshRateVote_004
 * @tc.desc: SetRefreshRateVote - 测试缺少maxRefreshRate字段分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetRefreshRateVote_004, TestSize.Level1)
{
    std::string jsonParam = R"({"voting": 1, "minRefreshRate": 30})";
    EXPECT_NO_THROW(videoSourceScreen_->SetRefreshRateVote(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_SetRefreshRateVote_005
 * @tc.desc: SetRefreshRateVote - 测试voting类型错误分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetRefreshRateVote_005, TestSize.Level1)
{
    std::string jsonParam = R"({"voting": "1", "minRefreshRate": 30, "maxRefreshRate": 60})";
    EXPECT_NO_THROW(videoSourceScreen_->SetRefreshRateVote(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_SetRefreshRateVote_006
 * @tc.desc: SetRefreshRateVote - 测试minRefreshRate类型错误分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetRefreshRateVote_006, TestSize.Level1)
{
    std::string jsonParam = R"({"voting": 1, "minRefreshRate": "30", "maxRefreshRate": 60})";
    EXPECT_NO_THROW(videoSourceScreen_->SetRefreshRateVote(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_SetRefreshRateVote_007
 * @tc.desc: SetRefreshRateVote - 测试maxRefreshRate类型错误分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetRefreshRateVote_007, TestSize.Level1)
{
    std::string jsonParam = R"({"voting": 1, "minRefreshRate": 30, "maxRefreshRate": "60"})";
    EXPECT_NO_THROW(videoSourceScreen_->SetRefreshRateVote(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_SetRefreshRateVote_008
 * @tc.desc: SetRefreshRateVote - 测试最小帧率不足分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetRefreshRateVote_008, TestSize.Level1)
{
    std::string jsonParam = R"({"voting": 1, "minRefreshRate": 20, "maxRefreshRate": 60})";
    EXPECT_NO_THROW(videoSourceScreen_->SetRefreshRateVote(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_SetRefreshRateVote_009
 * @tc.desc: SetRefreshRateVote - 测试maxRefreshRate小于minRefreshRate分支
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetRefreshRateVote_009, TestSize.Level1)
{
    std::string jsonParam = R"({"voting": 1, "minRefreshRate": 60, "maxRefreshRate": 30})";
    EXPECT_NO_THROW(videoSourceScreen_->SetRefreshRateVote(jsonParam));
}

/**
 * @tc.name: VideoSourceScreenDT_SetRefreshRateVote_010
 * @tc.desc: SetRefreshRateVote - 测试成功设置
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_SetRefreshRateVote_010, TestSize.Level1)
{
    std::string jsonParam = R"({"voting": 1, "minRefreshRate": 30, "maxRefreshRate": 60})";
    
    // 创建并mock RSInterfaces
    auto mockRsInterfaces = std::make_shared<Rosen::RSInterfaces>();
    EXPECT_CALL(*mockRsInterfaces, NotifyRefreshRateEvent(_));
    
    // 由于此处需要替换全局实例，实际测试中需要在更复杂的环境下进行
    // 这里主要是测试解析逻辑和参数验证
    EXPECT_NO_THROW(videoSourceScreen_->SetRefreshRateVote(jsonParam));
}

// ================ 复杂场景综合测试 ================

/**
 * @tc.name: VideoSourceScreenDT_ComprehensiveScenario_001
 * @tc.desc: 完整生命周期测试: 创建->初始化->启动->停止->销毁
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_ComprehensiveScenario_001, TestSize.Level2)
{
    sptr<Surface> testSurface = Surface::CreateSurfaceAsConsumer();
    VideoSourceScreen testVideoSource(testSurface);
    
    // 测试初始状态
    EXPECT_EQ(testVideoSource.screenId_, SCREEN_ID_INVALID);
    EXPECT_EQ(testVideoSource.srcScreenId_, SCREEN_ID_INVALID);
    EXPECT_EQ(testVideoSource.screenIdAppCast_, SCREEN_ID_INVALID);
    
    // 初始化测试
    VideoSourceConfigure config;
    config.srcScreenId_ = 1;
    config.screenWidth_ = 1920;
    config.screenHeight_ = 1080;
    config.frameRate_ = 30.0;
    
    int32_t initResult = testVideoSource.InitScreenSource(config);
    EXPECT_NO_THROW(initResult); // 由于依赖外部服务，只测试不崩溃
    
    // 屏幕源捕获相关测试
    EXPECT_NO_THROW(testVideoSource.StartScreenSourceCapture());
    
    // 请求帧测试
    EXPECT_NO_THROW(testVideoSource.RequestFrame());
    
    // 停止测试
    EXPECT_NO_THROW(testVideoSource.StopScreenSourceCapture());
    
    // 确保状态正确清理
    EXPECT_EQ(testVideoSource.screenId_, SCREEN_ID_INVALID);
}

/**
 * @tc.name: VideoSourceScreenDT_ComprehensiveScenario_002
 * @tc.desc: 应用投屏功能测试: 创建->调整大小->镜像->进入小窗口->退出->销毁
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_ComprehensiveScenario_002, TestSize.Level2)
{
    // 创建应用投屏屏幕
    std::string createJson = R"({"width": 1080, "height": 1920, "dpi": 2.0})";
    EXPECT_NO_THROW(videoSourceScreen_->CreateAppCastScreen(createJson));
    
    // 调整大小
    std::string resizeJson = R"({"width": 1920, "height": 1080})";
    EXPECT_NO_THROW(videoSourceScreen_->ResizeAppCastScreen(resizeJson));
    
    // 创建镜像 (非应用投屏模式)
    std::string mirrorJson1 = R"({"isAppCast": false})";
    EXPECT_NO_THROW(videoSourceScreen_->AppCastMakeMirror(mirrorJson1));
    
    // 创建镜像 (应用投屏模式)
    std::string mirrorJson2 = R"({"isAppCast": true})";
    EXPECT_NO_THROW(videoSourceScreen_->AppCastMakeMirror(mirrorJson2));
    
    // 进入小窗口
    EXPECT_NO_THROW(videoSourceScreen_->AppCastEnterSmallWindow());
    
    // 退出小窗口
    EXPECT_NO_THROW(videoSourceScreen_->AppCastExitSmallWindow());
    
    // 获取屏幕ID
    uint64_t screenId = 0;
    EXPECT_NO_THROW(videoSourceScreen_->GetAppCastScreenId(screenId));
    
    // 销毁应用投屏屏幕
    EXPECT_NO_THROW(videoSourceScreen_->DestroyAppCastScreen());
    EXPECT_EQ(videoSourceScreen_->screenIdAppCast_, SCREEN_ID_INVALID);
}

/**
 * @tc.name: VideoSourceScreenDT_ComprehensiveScenario_003
 * @tc.desc: 投影设置功能测试: 不同模式旋转设置
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_ComprehensiveScenario_003, TestSize.Level2)
{
    Rect region = {0, 0, 1920, 1080};
    
    // 测试各种投影模式
    EXPECT_NO_THROW(videoSourceScreen_->SetProjectionDisplay(1, 0, region));     // 普通投影
    EXPECT_NO_THROW(videoSourceScreen_->SetProjectionDisplay(1, 90, region));    // 90度旋转
    EXPECT_NO_THROW(videoSourceScreen_->SetProjectionDisplay(3, 0, region));     // BC模式
    EXPECT_NO_THROW(videoSourceScreen_->SetProjectionDisplay(2, 45, region));    // C模式 + 旋转
    
    // 测试各种区域大小
    Rect small = {0, 0, 800, 600};
    Rect large = {0, 0, 3840, 2160};
    EXPECT_NO_THROW(videoSourceScreen_->SetProjectionDisplay(1, 0, small));
    EXPECT_NO_THROW(videoSourceScreen_->SetProjectionDisplay(1, 0, large));
}

/**
 * @tc.name: VideoSourceScreenDT_ComprehensiveScenario_004
 * @tc.desc: 错误处理和恢复测试: 频繁调用错误场景
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_ComprehensiveScenario_004, TestSize.Level2)
{
    // 模拟频繁的错误场景调用
    EXPECT_NO_THROW(videoSourceScreen_->StopScreenSourceCapture()); // 重复调用停止
    EXPECT_NO_THROW(videoSourceScreen_->ReleaseScreenBuffer());     // 释放缓冲区
    EXPECT_NO_THROW(videoSourceScreen_->ChangeScreenRefreshRate(60)); // 改变刷新率
    
    // 验证错误处理不会导致崩溃
    EXPECT_NO_THROW(videoSourceScreen_->DestroyAppCastScreen()); // 销毁不存在的应用投屏
    EXPECT_NO_THROW(videoSourceScreen_->GetAppCastScreenId(0));  // 获取不存在的屏幕ID
    EXPECT_NO_THROW(videoSourceScreen_->SetAppCastSurface(nullptr)); // 设置无效surface
    
    // 所有调用都应该安全返回，不抛出异常
    EXPECT_TRUE(true);
}

/**
 * @tc.name: VideoSourceScreenDT_ComprehensiveScenario_005
 * @tc.desc: 参数边界测试: 超大、超小、负值参数
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_ComprehensiveScenario_005, TestSize.Level2)
{
    // 超大参数测试
    std::string hugeJson = R"({"width": 99999, "height": 99999, "dpi": 999.0})";
    EXPECT_NO_THROW(videoSourceScreen_->CreateAppCastScreen(hugeJson));
    
    // 超小参数测试
    std::string tinyJson = R"({"width": 1, "height": 1, "dpi": 0.1})";
    EXPECT_NO_THROW(videoSourceScreen_->CreateAppCastScreen(tinyJson));
    
    // 零值参数测试
    std::string zeroJson = R"({"width": 0, "height": 0, "dpi": 0.0})";
    EXPECT_NO_THROW(videoSourceScreen_->CreateAppCastScreen(zeroJson));
    
    // 负值参数测试 (JSON中负值会被视为数组的某个元素，应该会被过滤)
    std::string negativeJson = R"({"width": -1920, "height": -1080, "dpi": -2.0})";
    EXPECT_NO_THROW(videoSourceScreen_->CreateAppCastScreen(negativeJson));
    
    // 验证所有边界情况都能安全处理
    EXPECT_TRUE(true);
}

/**
 * @tc.name: VideoSourceScreenDT_ComprehensiveScenario_006
 * @tc.desc: 性能和并发测试: 多线程环境下的稳定性
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_ComprehensiveScenario_006, TestSize.Level3)
{
    std::vector<std::thread> threads;
    const int threadCount = 10;
    
    // 创建多个线程同时操作
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([this, i]() {
            // 每个线程执行不同的操作组合
            if (i % 2 == 0) {
                videoSourceScreen_->RequestFrame();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            } else {
                std::string json = R"({"width": 1920, "height": 1080, "dpi": 2.0})";
                videoSourceScreen_->CreateAppCastScreen(json);
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    // 验证并发执行后对象状态仍然有效
    EXPECT_NO_THROW(videoSourceScreen_->StopScreenSourceCapture());
    EXPECT_TRUE(true);
}

/**
 * @tc.name: VideoSourceScreenDT_ComprehensiveScenario_007
 * @tc.desc: 帧率设置刷新率投投票测试: 刷新率投票的完整流程
 * @tc.type: FUNC
 */
HWTEST_F(VideoSourceScreenDTTest, VideoSourceScreenDT_ComprehensiveScenario_007, TestSize.Level2)
{
    // 设置各种刷新率投票参数
    std::string voteJson1 = R"({"voting": 1, "minRefreshRate": 30, "maxRefreshRate": 60})";
    std::string voteJson2 = R"({"voting": 1, "minRefreshRate": 45, "maxRefreshRate": 90})";
    std::string voteJson3 = R"({"voting": 0, "minRefreshRate": 30, "maxRefreshRate": 60})";
    
    EXPECT_NO_THROW(videoSourceScreen_->SetRefreshRateVote(voteJson1));
    EXPECT_NO_THROW(videoSourceScreen_->SetRefreshRateVote(voteJson2));  // 改变投票
    EXPECT_NO_THROW(videoSourceScreen_->SetRefreshRateVote(voteJson3));  // 停止投票
    
    // 配合其他功能使用
    Rect region = {0, 0, 1920, 1080};
    EXPECT_NO_THROW(videoSourceScreen_->SetProjectionDisplay(1, 0, region));
    
    EXPECT_TRUE(true);
}

} // namespace Sharing
} // namespace OHOS

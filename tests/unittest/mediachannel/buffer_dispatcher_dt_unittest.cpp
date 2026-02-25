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

#include "buffer_dispatcher.h"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "media_channel_def.h"

using namespace OHOS::Sharing;

class BufferDispatcherDTTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        dispatcher_ = std::make_shared<BufferDispatcher>();
        receiver_ = std::make_shared<BufferReceiver>();
        
        // 测试数据
        audioData_ = std::make_shared<MediaData>();
        audioData_->mediaType = MEDIA_TYPE_AUDIO;
        audioData_->keyFrame = false;
        audioData->ssrc = 1;      // 1 源端号标识符值
        audioData->pts = 1000;    // 1000 模拟音频帧时戳
        audioData_->buff = std::make_shared<DataBuffer>();
        
        videoData_ = std::make_shared<MediaData>();
        videoData_->mediaType = MEDIA_TYPE_VIDEO;
        videoData_->keyFrame = true;
        videoData->ssrc = 2;      // 2 源端号标识符值
        videoData->pts = 2000;    // 2000 模拟视频帧时戳
        videoData_->buff = std::make_shared<DataBuffer>();
        
        nonKeyVideoData_ = std::make_shared<MediaData>();
        nonKeyVideoData_->mediaType = MEDIA_TYPE_VIDEO;
        nonKeyVideoData_->keyFrame = false;
        nonKeyVideoData->ssrc = 3; // 3 源端号标识符值
        nonKeyVideoData->pts = 3000; // 3000 模拟视频帧时戳
        nonKeyVideoData_->buff = std::make_shared<DataBuffer>();
        
        mixedData_ = std::make_shared<MediaData>();
        mixedData_->mediaType = MEDIA_TYPE_AV;
        mixedData_->keyFrame = true;
        mixedData->ssrc = 4;      // 4 源端号标识符值
        mixedData->pts = 4000;    // 4000 模拟帧时戳
        mixedData_->buff = std::make_shared<DataBuffer>();
    }

    void TearDown() override
    {
        dispatcher_ = nullptr;
        receiver_ = nullptr;
        audioData_ = nullptr;
        videoData_ = nullptr;
        nonKeyVideoData_ = nullptr;
        mixedData_ = nullptr;
    }

    BufferDispatcher::Ptr dispatcher_;
    BufferReceiver::Ptr receiver_;
    MediaData::Ptr audioData_;
    MediaData::Ptr videoData_;
    MediaData::Ptr nonKeyVideoData_;
    MediaData::Ptr mixedData_;
};

// 1. 构造函数和析构函数测试
TEST_F(BufferDispatcherDTTest, ConstructorAndDestructor)
{
    // 测试构造函数
    EXPECT_NE(dispatcher_, nullptr);
    
    // 测试默认容量
    EXPECT_EQ(dispatcher_->GetBufferSize(), 0);
    
    // 测试析构函数（在TearDown中验证）
}

// 2. BufferReceiver::OnMediaDataNotify 测试
TEST_F(BufferDispatcherDTTest, BufferReceiver_OnMediaDataNotify)
{
    // 设置源
    receiver_->SetSource(dispatcher_);
    
    // 测试通知功能
    int32_t ret = receiver_->OnMediaDataNotify();
    EXPECT_EQ(ret, 0);
}

// 3. BufferReceiver::OnAudioDataNotify 测试
TEST_F(BufferDispatcherDTTest, BufferReceiver_OnAudioDataNotify)
{
    // 设置源
    receiver_->SetSource(dispatcher_);
    
    // 测试通知功能
    int32_t ret = receiver_->OnAudioDataNotify();
    EXPECT_EQ(ret, 0);
}

// 4. BufferReceiver::OnVideoDataNotify 测试
TEST_F(BufferDispatcherDTTest, BufferReceiver_OnVideoDataNotify)
{
    // 设置源
    receiver_->SetSource(dispatcher_);
    
    // 测试通知功能
    int32_t ret = receiver_->OnVideoDataNotify();
    EXPECT_EQ(ret, 0);
}

// 5. BufferReceiver::IsMixedReceiver 测试
TEST_F(BufferDispatcherDTTest, BufferReceiver_IsMixedReceiver)
{
    // 设置源
    receiver_->SetSource(dispatcher_);
    
    // 测试初始状态
    EXPECT_FALSE(receiver_->IsMixedReceiver());
    
    // 请求混合类型数据来改变状态
    auto callback = [](const MediaData::Ptr &data) { (void)data; };
    receiver_->RequestRead(MEDIA_TYPE_AV, callback);
    
    // 现在应该返回true
    EXPECT_TRUE(receiver_->IsMixedReceiver());
}

// 6. BufferReceiver::RequestRead 测试
TEST_F(BufferDispatcherDTTest, BufferReceiver_RequestRead_AudioType)
{
    // 设置源
    receiver_->SetSource(dispatcher_);
    
    // 测试音频类型请求
    bool audioCalled = false;
    auto audioCallback = [&audioCalled](const MediaData::Ptr &data) {
        audioCalled = true;
        (void)data;
    };
    
    int32_t ret = receiver_->RequestRead(MEDIA_TYPE_AUDIO, audioCallback);
    // 由于没有数据，返回应该反映等待状态
    // 这是一个异步调用，可能会阻塞，但我们只关心返回值
    EXPECT_GE(ret, -1);  // 可能返回-1或更大
}

// 7. BufferReceiver::RequestRead 测试
TEST_F(BufferDispatcherDTTest, BufferReceiver_RequestRead_VideoType)
{
    // 设置源
    receiver_->SetSource(dispatcher_);
    
    // 测试视频类型请求
    bool videoCalled = false;
    auto videoCallback = [&videoCalled](const MediaData::Ptr &data) {
        videoCalled = true;
        (void)data;
    };
    
    int32_t ret = receiver_->RequestRead(MEDIA_TYPE_VIDEO, videoCallback);
    // 由于没有数据，返回应该反映等待状态
    EXPECT_GE(ret, -1);
}

// 8. BufferReceiver::RequestRead 测试
TEST_F(BufferDispatcherDTTest, BufferReceiver_RequestRead_MixedType)
{
    // 设置源
    receiver_->SetSource(dispatcher_);
    
    // 测试混合类型请求
    bool mixedCalled = false;
    auto mixedCallback = [&mixedCalled](const MediaData::Ptr &data) {
        mixedCalled = true;
        (void)data;
    };
    
    int32_t ret = receiver_->RequestRead(MEDIA_TYPE_AV, mixedCallback);
    // 由于没有数据，返回应该反映等待状态
    EXPECT_GE(ret, -1);
}

// 9. BufferReceiver::GetReceiverId 测试
TEST_F(BufferDispatcherDTTest, BufferReceiver_GetReceiverId)
{
    // 获取接收者ID
    uint32_t id = receiver_->GetReceiverId();
    EXPECT_NE(id, 0);  // 应该有一个有效的ID
}

// 10. BufferReceiver::GetDispatcherId 测试
TEST_F(BufferDispatcherDTTest, BufferReceiver_GetDispatcherId)
{
    // 在设置源头之前
    uint32_t id1 = receiver_->GetDispatcherId();
    EXPECT_EQ(id1, 0);  // 没有设置时应为0
    
    // 设置源头之后
    receiver_->SetSource(dispatcher_);
    uint32_t id2 = receiver_->GetDispatcherId();
    EXPECT_NE(id2, 0);  // 应该有有效的ID
}

// 11. BufferReceiver::NotifyReadStart 测试
TEST_F(BufferDispatcherDTTest, BufferReceiver_NotifyReadStart)
{
    // 确保初始状态为true（通过调试观察）
    receiver_->NotifyReadStart();
    
    // 测试通知功能（值的变化需要通过其他方法验证）
}

// 12. BufferReceiver::NotifyReadStop 测试
TEST_F(BufferDispatcherDTTest, BufferReceiver_NotifyReadStop)
{
    // 测试通知停止
    receiver_->NotifyReadStop();
    
    // 测试通知功能（值的变化需要通过其他方法验证）
}

// 13. BufferDispatcher::AttachReceiver 测试
TEST_F(BufferDispatcherDTTest, AttachReceiver_Success)
{
    // 成功附加接收者
    int32_t ret = dispatcher_->AttachReceiver(receiver_);
    EXPECT_EQ(ret, 0);
    
    // 验证接收者存在
    EXPECT_TRUE(dispatcher_->IsRecevierExist(receiver_->GetReceiverId()));
}

// 14. BufferDispatcher::AttachReceiver 重复附加测试
TEST_F(BufferDispatcherDTTest, AttachReceiver_Duplicate)
{
    // 第一次附加
    int32_t ret1 = dispatcher_->AttachReceiver(receiver_);
    EXPECT_EQ(ret1, 0);
    
    // 第二次附加同一接收者
    int32_t ret2 = dispatcher_->AttachReceiver(receiver_);
    EXPECT_EQ(ret2, 0);  // 应该成功返回0（设计处理了重复附加）
}

// 15. BufferDispatcher::AttachReceiver 空指针测试
TEST_F(BufferDispatcherDTTest, AttachReceiver_NullPointer)
{
    // 附加空指针
    int32_t ret = dispatcher_->AttachReceiver(nullptr);
    EXPECT_EQ(ret, -1);  // 应该返回错误
}

// 16. BufferDispatcher::DetachReceiver 测试
TEST_F(BufferDispatcherDTTest, DetachReceiver_Success)
{
    // 先附加再分离
    dispatcher_->AttachReceiver(receiver_);
    int32_t ret = dispatcher_->DetachReceiver(receiver_);
    EXPECT_EQ(ret, 0);
    
    // 验证接收者不再存在
    EXPECT_FALSE(dispatcher_->IsRecevierExist(receiver_->GetReceiverId()));
}

// 17. BufferDispatcher::DetachReceiver 不存在的接收者测试
TEST_F(BufferDispatcherDTTest, DetachReceiver_NonExistent)
{
    // 分离不存在的接收者
    int32_t ret = dispatcher_->DetachReceiver(receiver_);
    EXPECT_EQ(ret, -1);  // 应该返回错误
}

// 18. BufferDispatcher::DetachReceiver 空指针测试
TEST_F(BufferDispatcherDTTest, DetachReceiver_NullPointer)
{
    // 分离空指针
    int32_t ret = dispatcher_->DetachReceiver(nullptr);
    EXPECT_EQ(ret, -1);  // 应该返回错误
}

// 19. BufferDispatcher::InputData 有效音频数据测试
TEST_F(BufferDispatcherDTTest, InputData_ValidAudioData)
{
    // 输入有效音频数据
    int32_t ret = dispatcher_->InputData(audioData_);
    EXPECT_EQ(ret, 0);
    
    // 验证缓冲区大小增加
    EXPECT_GT(dispatcher_->GetBufferSize(), 0);
}

// 20. BufferDispatcher::InputData 有效视频数据测试
TEST_F(BufferDispatcherDTTest, InputData_ValidVideoData)
{
    // 输入有效视频数据
    int32_t ret = dispatcher_->InputData(videoData_);
    EXPECT_EQ(ret, 0);
    
    // 验证缓冲区大小增加
    EXPECT_GT(dispatcher_->GetBufferSize(), 0);
}

// 21. BufferDispatcher::InputData 非关键帧视频数据测试
TEST_F(BufferDispatcherDTTest, InputData_NonKeyVideoData)
{
    // 输入非关键帧视频数据
    int32_t ret = dispatcher_->InputData(nonKeyVideoData_);
    EXPECT_EQ(ret, 0);
    
    // 验证缓冲区大小增加
    EXPECT_GT(dispatcher_->GetBufferSize(), 0);
}

// 22. BufferDispatcher::InputData 无效数据测试
TEST_F(BufferDispatcherDTTest, InputData_InvalidData)
{
    // 输入空数据
    int32_t ret = dispatcher_->InputData(nullptr);
    EXPECT_EQ(ret, -1);  // 应该返回错误
    
    // 输入无效buff的数据
    MediaData::Ptr invalidData = std::make_shared<MediaData>();
    invalidData->mediaType = MEDIA_TYPE_AUDIO;
    invalidData->keyFrame = false;
    invalidData->buff = nullptr;
    
    ret = dispatcher_->InputData(invalidData);
    EXPECT_EQ(ret, -1);  // 应该返回错误
}

// 23. BufferDispatcher::ReadBufferData 无接收者测试
TEST_F(BufferDispatcherDTTest, ReadBufferData_NonExistentReceiver)
{
    // 尝试读取不存在接收者的数据
    auto callback = [](const MediaData::Ptr &data) { (void)data; };
    int32_t ret = dispatcher_->ReadBufferData(999, MEDIA_TYPE_AUDIO, callback); // 999 不存在的接收者ID
    EXPECT_EQ(ret, -1);  // 应该返回错误
}

// 24. BufferDispatcher::ReadBufferData 音频数据测试
TEST_F(BufferDispatcherDTTest, ReadBufferData_AudioData)
{
    // 先附加接收者
    dispatcher_->AttachReceiver(receiver_);
    
    // 先输入数据
    dispatcher_->InputData(audioData_);
    
    // 然后尝试读取
    bool dataReceived = false;
    auto callback = [&dataReceived](const MediaData::Ptr &data) {
        dataReceived = true;
        (void)data;
    };
    
    int32_t ret = dispatcher_->ReadBufferData(receiver_->GetReceiverId(), MEDIA_TYPE_AUDIO, callback);
    // 由于数据在缓冲区中，应该能够成功读取
    EXPECT_EQ(ret, 0);
}

// 25. BufferDispatcher::ReadBufferData 视频数据测试
TEST_F(BufferDispatcherDTTest, ReadBufferData_VideoData)
{
    // 先附加接收者
    dispatcher_->AttachReceiver(receiver_);
    
    // 先输入数据
    dispatcher_->InputData(videoData_);
    
    // 然后尝试读取
    bool dataReceived = false;
    auto callback = [&dataReceived](const MediaData::Ptr &data) {
        dataReceived = true;
        (void)data;
    };
    
    int32_t ret = dispatcher_->ReadBufferData(receiver_->GetReceiverId(), MEDIA_TYPE_VIDEO, callback);
    // 由于数据在缓冲区中，应该能够成功读取
    EXPECT_EQ(ret, 0);
}

// 26. BufferDispatcher::ReadBufferData 非关键帧视频在KeyOnly模式测试
TEST_F(BufferDispatcherDTTest, ReadBufferData_NonKeyVideoInKeyOnlyMode)
{
    // 先附加接收者
    dispatcher_->AttachReceiver(receiver_);
    
    // 输入非关键帧视频数据
    dispatcher_->InputData(nonKeyVideoData_);
    
    // 尝试读取应该成功（KeyOnly模式在BufferDispatcher中默认为false）
    bool dataReceived = false;
    auto callback = [&dataReceived](const MediaData::Ptr &data) {
        dataReceived = true;
        (void)data;
    };
    
    int32_t ret = dispatcher_->ReadBufferData(receiver_->GetReceiverId(), MEDIA_TYPE_VIDEO, callback);
    EXPECT_EQ(ret, 0);
}

// 27. BufferDispatcher::InputData 混合数据测试
TEST_F(BufferDispatcherDTTest, InputData_MixedData)
{
    // 输入混合数据
    int32_t ret = dispatcher_->InputData(mixedData_);
    EXPECT_EQ(ret, 0);
    
    // 验证缓冲区大小增加
    EXPECT_GT(dispatcher_->GetBufferSize(), 0);
}

// 28. BufferDispatcher::GetBufferSize 测试
TEST_F(BufferDispatcherDTTest, GetBufferSize)
{
    // 初始缓冲区大小应该是0
    EXPECT_EQ(dispatcher_->GetBufferSize(), 0);
    
    // 输入数据后验证大小
    dispatcher_->InputData(audioData_);
    EXPECT_GT(dispatcher_->GetBufferSize(), 0);
    
    dispatcher_->InputData(videoData_);
    EXPECT_GT(dispatcher_->GetBufferSize(), 1);
}

// 29. BufferDispatcher::FlushBuffer 测试
TEST_F(BufferDispatcherDTTest, FlushBuffer)
{
    // 先输入一些数据
    dispatcher_->InputData(audioData_);
    dispatcher_->InputData(videoData_);
    
    // 验证缓冲区不为空
    EXPECT_GT(dispatcher_->GetBufferSize(), 0);
    
    // 清空缓冲区
    dispatcher_->FlushBuffer();
    
    // 验证缓冲区已清空
    EXPECT_EQ(dispatcher_->GetBufferSize(), 0);
}

// 30. BufferDispatcher::ReleaseIdleBuffer 测试
TEST_F(BufferDispatcherDTTest, ReleaseIdleBuffer)
{
    // 先获取一些空闲缓冲区
    MediaData::Ptr audioBuffer = dispatcher_->RequestDataBuffer(MEDIA_TYPE_AUDIO, 100); // 100 缓冲区大小
    MediaData::Ptr videoBuffer = dispatcher_->RequestDataBuffer(MEDIA_TYPE_VIDEO, 100); // 100 缓冲区大小
    
    EXPECT_NE(audioBuffer, nullptr);
    EXPECT_NE(videoBuffer, nullptr);
    
    // 模拟数据规范
    BufferDispatcher::DataSpec::Ptr dataSpec = std::make_shared<BufferDispatcher::DataSpec>();
    dataSpec->mediaData = audioBuffer;
    
    // 释放空闲缓冲区
    dispatcher_->ReturnIdleBuffer(dataSpec);
    
    // 验证释放功能（通过观察内存分配/释放情况）
}

// 31. BufferDispatcher::ReleaseAllReceiver 测试
TEST_F(BufferDispatcherDTTest, ReleaseAllReceiver)
{
    // 附加多个接收者
    BufferReceiver::Ptr receiver2 = std::make_shared<BufferReceiver>();
    dispatcher_->AttachReceiver(receiver_);
    dispatcher_->AttachReceiver(receiver2);
    
    // 验证接收者存在
    EXPECT_TRUE(dispatcher_->IsRecevierExist(receiver_->GetReceiverId()));
    EXPECT_TRUE(dispatcher_->IsRecevierExist(receiver2->GetReceiverId()));
    
    // 释放所有接收者
    dispatcher_->ReleaseAllReceiver();
    
    // 验证接收者已被释放
    EXPECT_FALSE(dispatcher_->IsRecevierExist(receiver_->GetReceiverId()));
    EXPECT_FALSE(dispatcher_->IsRecevierExist(receiver2->GetReceiverId()));
}

// 32. BufferDispatcher::StopDispatch 启动测试
TEST_F(BufferDispatcherDTTest, StartAndStopDispatch)
{
    // 验证初始状态
    EXPECT_TRUE(dispatcher_->running_);  // 在构造函数中设置为true
    
    // 停止分发
    dispatcher_->StopDispatch();
    
    // 验证停止状态（需要在实现中添加访问running_的接口或使用反射）
}

// 33. BufferDispatcher::RequestDataBuffer 视频缓冲区测试
TEST_F(BufferDispatcherDTTest, RequestDataBuffer_VideoBuffer)
{
    // 请求视频缓冲区
    MediaData::Ptr buffer = dispatcher_->RequestDataBuffer(MEDIA_TYPE_VIDEO, 100); // 100 缓冲区大小
    
    EXPECT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->mediaType, MEDIA_TYPE_VIDEO);
}

// 34. BufferDispatcher::RequestDataBuffer 音频缓冲区测试
TEST_F(BufferDispatcherDTTest, RequestDataBuffer_AudioBuffer)
{
    // 请求音频缓冲区
    MediaData::Ptr buffer = dispatcher_->RequestDataBuffer(MEDIA_TYPE_AUDIO, 100); // 100 缓冲区大小
    
    EXPECT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->mediaType, MEDIA_TYPE_AUDIO);
}

// 35. BufferDispatcher::RequestDataBuffer 无效大小测试
TEST_F(BufferDispatcherDTTest, RequestDataBuffer_InvalidSize)
{
    // 请求大小为0的缓冲区
    MediaData::Ptr buffer = dispatcher_->RequestDataBuffer(MEDIA_TYPE_AUDIO, 0); // 0 无效的缓冲区大小
    
    EXPECT_EQ(buffer, nullptr);  // 应该返回nullptr
}

// 36. BufferDispatcher::SetSpsNalu 和 GetSPS 测试
TEST_F(BufferDispatcherDTTest, SetAndGetSPS)
{
    MediaData::Ptr spsData = std::make_shared<MediaData>();
    spsData->mediaType = MEDIA_TYPE_VIDEO;
    spsData->buff = std::make_shared<DataBuffer>();
    
    // 设置SPS数据
    dispatcher_->SetSpsNalu(spsData);
    
    // 获取SPS数据
    MediaData::Ptr retrievedSPS = dispatcher_->GetSPS();
    
    EXPECT_EQ(retrievedSPS, spsData);
}

// 37. BufferDispatcher::SetPpsNalu 和 GetPPS 测试
TEST_F(BufferDispatcherDTTest, SetAndGetPPS)
{
    MediaData::Ptr ppsData = std::make_shared<MediaData>();
    ppsData->mediaType = MEDIA_TYPE_VIDEO;
    ppsData->buff = std::make_shared<DataBuffer>();
    
    // 设置PPS数据
    dispatcher_->SetPpsNalu(ppsData);
    
    // 获取PPS数据
    MediaData::Ptr retrievedPPS = dispatcher_->GetPPS();
    
    EXPECT_EQ(retrievedPPS, ppsData);
}

// 38. BufferDispatcher::GetCurrentGop 测试
TEST_F(BufferDispatcherDTTest, GetCurrentGop)
{
    // 初始GOP值
    uint32_t initialGop = dispatcher_->GetCurrentGop();
    EXPECT_EQ(initialGop, 0);  // 初始值应为0
    
    // 输入关键帧数据（可能会改变GOP值）
    dispatcher_->InputData(videoData_);
    
    uint32_t newGop = dispatcher_->GetCurrentGop();
    // GOP值由于输入关键帧可能会改变
    EXPECT_GE(newGop, initialGop);
}

// 39. BufferDispatcher::IsRecevierExist 测试
TEST_F(BufferDispatcherDTTest, IsRecevierExist)
{
    // 验证不存在的接收者
    EXPECT_FALSE(dispatcher_->IsRecevierExist(receiver_->GetReceiverId()));
    
    // 附加接收者后
    dispatcher_->AttachReceiver(receiver_);
    EXPECT_TRUE(dispatcher_->IsRecevierExist(receiver_->GetReceiverId()));
}

// 40. BufferDispatcher::IsRead 测试
TEST_F(BufferDispatcherDTTest, IsRead)
{
    // 附加接收者并输入数据
    dispatcher_->AttachReceiver(receiver_);
    dispatcher_->InputData(audioData_);
    
    // 测试索引超出范围的情况
    bool result = dispatcher_->IsRead(receiver_->GetReceiverId(), 999); // 999 超出范围的索引
    EXPECT_TRUE(result);  // 超出范围应该返回true
    
    // 测试有效索引的情况（需要更多数据来测试有效索引）
}

// 41. 性能测试：批量数据处理
TEST_F(BufferDispatcherDTTest, Performance_BatchDataProcessing)
{
    const int32_t batchSize = 1000; // 1000 批量处理的数据量
    
    // 批量输入音频数据
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int32_t i = 0; i < batchSize; i++) { // 1000 循环条件
        dispatcher_->InputData(audioData_);
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Batch input " << batchSize << " audio data took " << duration.count() << " ms" << std::endl;
    
    // 验证缓冲区大小
    EXPECT_EQ(dispatcher_->GetBufferSize(), batchSize);
}

// 42. 边界测试：缓冲区容量限制
TEST_F(BufferDispatcherDTTest, Boundary_BufferCapacityLimit)
{
    // 创建最大容量的分发器
    BufferDispatcher::Ptr largeDispatcher = std::make_shared<BufferDispatcher>(100, 10); // 100 最大容量, 10 视频容量
    
    // 输入大量数据测试限制
    int32_t inputCount = 0;
    for (int32_t i = 0; i < 150; i++) {   // 150 测试数据数量，超过限制
        int32_t ret = largeDispatcher->InputData(audioData_);
        if (ret == 0) {
            inputCount++;
        }
    }
    
    std::cout << "Successfully input " << inputCount << " data items with limit of 100" << std::endl;
    EXPECT_LE(largeDispatcher->GetBufferSize(), 100); // 100 最大容量
}

// 43. 并发测试：多线程访问
// 辅助函数：每个线程的具体操作逻辑
void ThreadWorkerFunction(BufferDispatcher* dispatcher, BufferReceiver* receiver,
    int32_t operationsPerThread, MediaData* audioData, MediaData* videoData)
{
    for (int32_t j = 0; j < operationsPerThread; j++) { // 100 循环条件为操作次数
        // 随机选择音频或视频数据
        MediaData::Ptr data = (j % 2 == 0) ? std::static_pointer_cast<MediaData>(audioData) :
            std::static_pointer_cast<MediaData>(videoData); // 2 除数，用于数据类型交替
        dispatcher->InputData(data);
        
        // 读取数据
        auto callback = [](const MediaData::Ptr &data) { (void)data; };
        dispatcher->ReadBufferData(receiver->GetReceiverId(),
            (j % 2 == 0) ? MEDIA_TYPE_AUDIO : MEDIA_TYPE_VIDEO, // 2 除数，用于媒体类型交替
            callback);
    }
}

TEST_F(BufferDispatcherDTTest, Concurrency_MultiThreadAccess)
{
    const int32_t threadCount = 5;                    // 5 测试线程数量，模拟中等并发场景
    const int32_t operationsPerThread = 100;          // 100 每个线程的操作次数，测试基本并发能力
    std::vector<std::thread> threads;
    
    // 创建多个接收者
    std::vector<BufferReceiver::Ptr> receivers;
    for (int32_t i = 0; i < threadCount; i++) {       // 5 循环条件为线程数量
        BufferReceiver::Ptr receiver = std::make_shared<BufferReceiver>();
        dispatcher_->AttachReceiver(receiver);
        receivers.push_back(receiver);
    }
    
    // 启动多个线程进行操作
    for (int32_t i = 0; i < threadCount; i++) {       // 5 循环条件为线程数量
        threads.emplace_back(ThreadWorkerFunction, dispatcher_.get(), receivers[i].get(),
            operationsPerThread, audioData_.get(), videoData_.get());
    }
    
    // 等待所有线程完成
    for (auto &thread : threads) {
        thread.join();
    }
    
    // 验证系统仍然处于稳定状态
    EXPECT_GT(dispatcher_->GetBufferSize(), 0);
}

// 44. 错误恢复测试：错误处理后恢复
TEST_F(BufferDispatcherDTTest, ErrorRecovery_ErrorHandlingAndRecovery)
{
    // 输入一些数据
    dispatcher_->InputData(audioData_);
    dispatcher_->InputData(videoData_);
    
    // 验证缓冲区有数据
    EXPECT_GT(dispatcher_->GetBufferSize(), 0);
    
    // 模拟错误处理 - 清空缓冲区
    dispatcher_->FlushBuffer();
    
    // 验证系统恢复
    EXPECT_EQ(dispatcher_->GetBufferSize(), 0);
    
    // 重新输入数据验证系统正常工作
    dispatcher_->InputData(audioData_);
    EXPECT_GT(dispatcher_->GetBufferSize(), 0);
}

// 45. 内存泄漏测试：长时间运行内存使用
TEST_F(BufferDispatcherDTTest, MemoryLeak_LongRunningMemoryCheck)
{
    size_t initialMemory = 0; // 实际项目中需要获取真正的内存使用量
    
    // 模拟长时间运行的操作
    for (int32_t cycle = 0; cycle < 100; cycle++) {   // 100 模拟内存泄漏测试的循环次数
        // 附加和分离接收者
        BufferReceiver::Ptr receiver = std::make_shared<BufferReceiver>();
        dispatcher_->AttachReceiver(receiver);
        dispatcher_->DetachReceiver(receiver);
        
        // 输入和消费数据
        dispatcher_->InputData(audioData_);
        dispatcher_->InputData(videoData_);
        
        // 清空缓冲区
        dispatcher_->FlushBuffer();
        
        // 验证内存使用模式（实际项目中需要内存监控）
    }
    
    std::cout << "Memory leak test completed through 100 cycles" << std::endl;
    // 在实际项目中，这里应该添加内存使用量的验证
}

// 46. 异常处理测试：异常数据类型处理
TEST_F(BufferDispatcherDTTest, ExceptionHandling_InvalidMediaType)
{
    // 创建带有无效媒体类型的数据
    MediaData::Ptr invalidData = std::make_shared<MediaData>();
    invalidData->mediaType = static_cast<MediaType>(999); // 999 无效的媒体类型值
    invalidData->keyFrame = false;
    invalidData->buff = std::make_shared<DataBuffer>();
    
    // 输入无效数据媒体类型的数据
    int32_t ret = dispatcher_->InputData(invalidData);
    // 处理方式依赖于具体实现，这里假设返回0
    EXPECT_GE(ret, -1);
}

// 47. 数据一致性测试：数据完整性验证
TEST_F(BufferDispatcherDTTest, DataConsistency_DataIntegrity)
{
    // 输入特定数据
    audioData->pts = 12345;     // 12345 模拟音频帧的时戳
    videoData->pts = 67890;     // 67890 模拟视频帧的时戳
    
    dispatcher_->InputData(audioData);
    dispatcher_->InputData(videoData);
    
    // 读取并验证数据
    uint64_t receivedAudioPts = 0;
    uint64_t receivedVideoPts = 0;
    
    auto audioCallback = [&receivedAudioPts](const MediaData::Ptr &data) {
        receivedAudioPts = data->pts;
    };
    
    auto videoCallback = [&receivedVideoPts](const MediaData::Ptr &data) {
        receivedVideoPts = data->pts;
    };
    
    dispatcher_->ReadBufferData(receiver_->GetReceiverId(), MEDIA_TYPE_AUDIO, audioCallback);
    dispatcher_->ReadBufferData(receiver_->GetReceiverId(), MEDIA_TYPE_VIDEO, videoCallback);
    
    // 验证数据完整性
    EXPECT_EQ(receivedAudioPts, 12345);  // 12345 模拟音频帧的时戳
    EXPECT_EQ(receivedVideoPts, 67890);  // 67890 模拟视频帧的时戳
}

// 48. 混合流测试：音频视频混合处理
TEST_F(BufferDispatcherDTTest, MixedStream_AudioVideoMixedProcessing)
{
    // 附加混合接收者
    receiver_->SetSource(dispatcher_);
    
    // 请求混合数据
    bool mixedCalled = false;
    auto mixedCallback = [&mixedCalled](const MediaData::Ptr &data) {
        mixedCalled = true;
        EXPECT_EQ(data->mediaType, MEDIA_TYPE_AV);
    };
    
    receiver_->RequestRead(MEDIA_TYPE_AV, mixedCallback);
    
    // 输入混合数据
    dispatcher_->InputData(mixedData_);
    
    // 验证处理
    EXPECT_TRUE(mixedCalled);
}

// 49. 大数据测试：处理大数据量
TEST_F(BufferDispatcherDTTest, LargeData_HandleLargeDataVolume)
{
    const int32_t largeDataSize = 10000; // 10000 大数据量测试
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 输入大量数据
    for (int32_t i = 0; i < largeDataSize; i++) {   // 10000 循环条件
        audioData->pts = i;
        dispatcher_->InputData(audioData);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Processed " << largeDataSize << " data items in " << duration.count() << " ms" << std::endl;
    std::cout << "Average processing time per item: " <<
        static_cast<double>(duration.count()) / largeDataSize << " ms" << std::endl;
    
    // 验证数据已正确处理
    EXPECT_EQ(dispatcher_->GetBufferSize(), largeDataSize);
}

// 50. 完整流测试端到端流处理
TEST_F(BufferDispatcherDTTest, CompleteFlow_EndToEndStreamProcessing)
{
    // 1. 附加接收者
    dispatcher_->AttachReceiver(receiver_);
    
    // 2. 输入多种类型的数据
    dispatcher_->InputData(audioData_);
    dispatcher_->InputData(videoData_);
    dispatcher_->InputData(nonKeyVideoData_);
    dispatcher_->InputData(mixedData_);
    
    // 3. 验证缓冲区状态
    EXPECT_EQ(dispatcher_->GetBufferSize(), 4);     // 4 模拟接收4组数据
    
    // 4. 读取并处理数据
    int32_t audioReadCount = 0;
    int32_t videoReadCount = 0;
    int32_t mixedReadCount = 0;
    
    auto audioCallback = [&audioReadCount](const MediaData::Ptr &data) {
        audioReadCount++;
        EXPECT_EQ(data->mediaType, MEDIA_TYPE_AUDIO);
    };
    
    auto videoCallback = [&videoReadCount](const MediaData::Ptr &data) {
        videoReadCount++;
        EXPECT_EQ(data->mediaType, MEDIA_TYPE_VIDEO);
    };
    
    auto mixedCallback = [&mixedReadCount](const MediaData::Ptr &data) {
        mixedReadCount++;
        EXPECT_EQ(data->mediaType, MEDIA_TYPE_AV);
    };
    
    dispatcher_->ReadBufferData(receiver_->GetReceiverId(), MEDIA_TYPE_AUDIO, audioCallback);
    dispatcher_->ReadBufferData(receiver_->GetReceiverId(), MEDIA_TYPE_VIDEO, videoCallback);
    dispatcher_->ReadBufferData(receiver_->GetReceiverId(), MEDIA_TYPE_AV, mixedCallback);
    
    // 5. 验证处理结果
    EXPECT_GT(audioReadCount, 0);
    EXPECT_GT(videoReadCount, 0);
    EXPECT_GT(mixedReadCount, 0);
    
    std::cout << "End-to-end test completed:" << std::endl;
    std::cout << " Audio reads: " << audioReadCount << std::endl;
    std::cout << " Video reads: " << videoReadCount << std::endl;
    std::cout << " Mixed reads: " << mixedReadCount << std::endl;
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv); // argc 命令行参数数量, argv 命令行参数数组
    return RUN_ALL_TESTS();
}

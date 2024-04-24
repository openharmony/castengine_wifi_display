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

#ifndef OHOS_SHARING_BUFFER_DISPATCHER_H
#define OHOS_SHARING_BUFFER_DISPATCHER_H
#include <condition_variable>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include "common/identifier.h"
#include "common/media_log.h"
#include "media_channel_def.h"
#include "utils/circular_buffer.h"
#include "utils/data_buffer.h"
#include "utils/timeout_timer.h"

constexpr size_t INITIAL_BUFFER_CAPACITY = 50;
constexpr size_t MAX_RECEIVER_SIZE = 16;
constexpr uint32_t INVALID_INDEX = static_cast<uint32_t>(-1);
constexpr uint32_t RECV_FLAG_BASE = 0x0001;
enum MediaDispacherMode { MEDIA_VIDEO_ONLY, MEDIA_AUDIO_ONLY, MEDIA_VIDEO_AUDIO_MIXED };
namespace OHOS {
namespace Sharing {

class IBufferReader : public std::enable_shared_from_this<IBufferReader> {
public:
    using Ptr = std::shared_ptr<IBufferReader>;

    IBufferReader(){};
    virtual ~IBufferReader(){};

    uint32_t GetLatestAudioIndex()
    {
        MEDIA_LOGD("trace.");
        return lastAudioIndex_;
    }

    uint32_t GetLatestVideoIndex()
    {
        MEDIA_LOGD("trace.");
        return lastVideoIndex_;
    }

    void EnableKeyRedirect(bool enable)
    {
        MEDIA_LOGD("trace.");
        keyRedirect_ = enable;
    }

public:
    virtual bool IsRead(uint32_t receiverId, uint32_t index) = 0;
    virtual void ClearReadBit(uint32_t receiverId, MediaType type) = 0;
    virtual void ClearDataBit(uint32_t receiverId, MediaType type) = 0;
    virtual void NotifyReadReady(uint32_t receiverId, MediaType type) = 0;

    virtual int32_t ReadBufferData(uint32_t receiverId, MediaType type,
                                   std::function<void(const MediaData::Ptr &data)> cb) = 0;

    virtual size_t GetBufferSize() = 0;
    virtual uint32_t GetDispatcherId() = 0;
    virtual const MediaData::Ptr GetSPS() = 0;
    virtual const MediaData::Ptr GetPPS() = 0;

protected:
    uint32_t lastAudioIndex_ = INVALID_INDEX;
    uint32_t lastVideoIndex_ = INVALID_INDEX;
    volatile std::atomic<bool> keyRedirect_ = false;
    volatile std::atomic<uint32_t> recvBitRef_ = 0x0000;
    volatile std::atomic<uint32_t> dataBitRef_ = 0x0000;
};

class IBufferReceiverListener {
public:
    using Ptr = std::shared_ptr<IBufferReceiverListener>;

    IBufferReceiverListener(){};
    virtual ~IBufferReceiverListener(){};

    virtual void OnAccelerationDoneNotify() = 0;
    virtual void OnKeyModeNotify(bool enable) = 0;
};

class BufferReceiver : public IdentifierInfo {
public:
    using Ptr = std::shared_ptr<BufferReceiver>;

    BufferReceiver(){};
    virtual ~BufferReceiver(){};

    virtual bool IsMixedReceiver();
    virtual int32_t OnMediaDataNotify();
    virtual int32_t OnAudioDataNotify();
    virtual int32_t OnVideoDataNotify();
    virtual int32_t RequestRead(MediaType type, std::function<void(const MediaData::Ptr &data)> cb);
    virtual void SetSource(IBufferReader::Ptr dataReader);

    uint32_t GetReceiverId();
    uint32_t GetDispatcherId();

    void NotifyReadStop();
    void NotifyReadStart();
    void EnableKeyMode(bool enable);

    bool IsKeyMode();
    bool IsKeyRedirect();

    const MediaData::Ptr GetSPS();
    const MediaData::Ptr GetPPS();

    bool NeedAcceleration();
    void DisableAcceleration();
    virtual void SendAccelerationDone();
    virtual void EnableKeyRedirect(bool enable);
    void SetBufferReceiverListener(std::weak_ptr<IBufferReceiverListener> listener);

protected:
    std::mutex mutex_;
    std::atomic<bool> dataReady_ = false;
    std::atomic<bool> nonBlockAudio_ = false;
    std::atomic<bool> nonBlockVideo_ = false;

    std::atomic<bool> firstVRead_ = true;
    std::atomic<bool> firstARead_ = true;
    std::atomic<bool> firstMRead_ = true;

    std::condition_variable notifyAudio_;
    std::condition_variable notifyVideo_;
    std::condition_variable notifyData_;
    std::weak_ptr<IBufferReceiverListener> listener_;

    volatile std::atomic<bool> mixed_ = false;
    volatile std::atomic<bool> keyOnly_ = false;
    volatile std::atomic<bool> keyRedirect_ = false;
    volatile std::atomic<bool> accelerationDone_ = false;

    IBufferReader::Ptr bufferReader_ = nullptr;
};

class BufferDispatcherListener {
public:
    using Ptr = std::shared_ptr<BufferDispatcherListener>;
    virtual ~BufferDispatcherListener() = default;

    virtual void OnWriteTimeout() = 0;
};

class BufferDispatcher : public IBufferReader,
                         public IdentifierInfo {
public:
    using Ptr = std::shared_ptr<BufferDispatcher>;

    class DataNotifier {
    public:
        using Ptr = std::shared_ptr<DataNotifier>;

        DataNotifier(){};
        ~DataNotifier(){};

        void SetReadIndex(uint32_t index)
        {
            MEDIA_LOGD("trace.");
            readIndex_ = index;
        }

        uint32_t GetReadIndex()
        {
            MEDIA_LOGD("trace.");
            return readIndex_;
        }

    public:
        void SetBlock();
        void SendAccelerationDone();
        void NotifyDataReceiver(MediaType type);
        void SetNeedUpdate(bool enable, MediaType type);
        void SetNotifyReceiver(BufferReceiver::Ptr receiver);
        void SetListenDispatcher(IBufferReader::Ptr dispatcher);

        bool IsMixedReceiver();
        bool NeedAcceleration();
        bool IsKeyModeReceiver();
        bool IsKeyRedirectReceiver();
        bool DataAvailable(MediaType type);

        uint32_t GetReceiverId();
        uint32_t GetReceiverReadIndex(MediaType type);

        BufferReceiver::Ptr GetBufferReceiver();

    public:
        uint32_t audioIndex = INVALID_INDEX;
        uint32_t videoIndex = INVALID_INDEX;
        std::atomic<bool> needUpdateAIndex = true;
        std::atomic<bool> needUpdateVIndex = true;

    private:
        bool block_ = false;
        uint32_t readIndex_ = INVALID_INDEX;
        std::weak_ptr<BufferReceiver> receiver_;
        std::weak_ptr<IBufferReader> dispatcher_;
    };

    struct DataSpec {
        using Ptr = std::shared_ptr<DataSpec>;

        volatile std::atomic<uint16_t> reserveFlag;
        uint64_t seq;
        MediaData::Ptr mediaData;
    };

public:
    explicit BufferDispatcher(uint32_t maxCapacity = MAX_BUFFER_CAPACITY,
                     uint32_t capacityIncrement = BUFFER_CAPACITY_INCREMENT);
    ~BufferDispatcher() override;

    inline uint32_t GetDispatcherId() override
    {
        MEDIA_LOGD("trace.");
        return GetId();
    }

public:
    void StopDispatch();
    void StartDispatch();
    void CancelReserve();
    void ReleaseAllReceiver();
    int32_t AttachReceiver(BufferReceiver::Ptr receiver);
    int32_t DetachReceiver(BufferReceiver::Ptr receiver);
    int32_t DetachReceiver(uint32_t receiverId, DataNotifier::Ptr notifier);
    void SetBufferDispatcherListener(BufferDispatcherListener::Ptr listener);

    void SetSpsNalu(MediaData::Ptr spsbuf);
    void SetPpsNalu(MediaData::Ptr ppsbuf);
    void SetBufferCapacity(size_t capacity);
    void SetDataMode(MediaDispacherMode dataMode);
    int32_t InputData(const MediaData::Ptr &data);

    void FlushBuffer();
    void ReleaseIdleBuffer();
    void EnableKeyMode(bool enable);
    void EnableRapidMode(bool enable);
    void ClearReadBit(uint32_t receiverId, MediaType type) override;
    void ClearDataBit(uint32_t receiverId, MediaType type) override;
    void SetReceiverReadFlag(uint32_t receiverId, DataSpec::Ptr &dataSpec);
    bool IsRecevierExist(uint32_t receiverId);
    bool IsRead(uint32_t receiverId, uint32_t index) override;

    uint32_t GetCurrentGop();
    size_t GetBufferSize() override;
    void NotifyReadReady(uint32_t receiverId, MediaType type) override;
    int32_t ReadBufferData(uint32_t receiverId, MediaType type,
                           std::function<void(const MediaData::Ptr &data)> cb) override;
    const MediaData::Ptr GetSPS() override;
    const MediaData::Ptr GetPPS() override;
    MediaData::Ptr RequestDataBuffer(MediaType type, uint32_t size);
    DataNotifier::Ptr GetNotifierByReceiverId(uint32_t receiverId);
    DataNotifier::Ptr GetNotifierByReceiverPtr(BufferReceiver::Ptr receiver);

private:
    void UpdateIndex();
    void ResetAllIndex();
    bool IsVideoData(const DataSpec::Ptr &dataSpec);
    bool IsAudioData(const DataSpec::Ptr &dataSpec);
    bool IsKeyVideoFrame(const DataSpec::Ptr &dataSpec);
    bool IsDataReaded(uint32_t receiverId, DataSpec::Ptr &dataSpec);

    uint32_t FindNextDeleteVideoIndex();
    uint32_t FindLastIndex(MediaType type);
    uint32_t FindNextIndex(uint32_t index, MediaType type);
    uint32_t FindNextIndex(uint32_t index, MediaType type, uint32_t receiverId);

    void EraseOldGopDatas();
    void ReCalculateCapacity(bool keyFrame);
    void ReturnIdleBuffer(DataSpec::Ptr &data);
    void DeleteHeadDatas(uint32_t size, bool forceDelete);
    void PreProcessDataSpec(const DataSpec::Ptr &dataSpec);

    bool HeadFrameNeedReserve();
    bool NeedExtendToDBCapacity();
    bool NeedRestoreToNormalCapacity();
    int32_t WriteDataIntoBuffer(const DataSpec::Ptr &data);

    void OnKeyRedirect();
    void SetDataRef(uint32_t bitref);
    void SetReadRef(uint32_t bitref);
    void UnlockWaitingReceiverIndex(MediaType type);
    void ActiveDataRef(MediaType type, bool keyFrame);
    void ActivateReceiverIndex(uint32_t index, MediaType type);
    void SetReceiverDataRef(uint32_t receiverId, MediaType type, bool ready);
    void SetReceiverReadRef(uint32_t receiverId, MediaType type, bool ready);
    void UpdateReceiverReadIndex(uint32_t receiverId, const uint32_t readIndex, MediaType type);

    uint32_t GetDataRef();
    uint32_t GetReadRef();
    uint32_t FindReceiverIndex(uint32_t receiverId);
    uint32_t GetReceiverDataRef(uint32_t receiverId);
    uint32_t GetReceiverReadRef(uint32_t receiverId);
    uint32_t GetReceiverIndexRef(uint32_t receiverId);
    static int32_t NotifyThreadWorker(void *userParam);

private:
    bool running_ = false;
    bool writing_ = false;
    bool videoNeedActivate_ = false;
    bool audioNeedActivate_ = false;
    bool capacityEvaluating_ = false;
    volatile bool keyOnly_ = false;
    volatile bool waitingKey_ = true;
    volatile bool rapidMode_ = false;
    uint16_t readRefFlag_ = 0x0000;
    uint32_t baseCounter_ = 0;
    uint32_t videoFrameCnt_ = 0;
    uint32_t audioFrameCnt_ = 0;
    uint32_t maxBufferCapacity_ = MAX_BUFFER_CAPACITY;
    uint32_t baseBufferCapacity_ = INITIAL_BUFFER_CAPACITY;
    uint32_t doubleBufferCapacity_ = INITIAL_BUFFER_CAPACITY * 2;
    uint32_t bufferCapacityIncrement_ = BUFFER_CAPACITY_INCREMENT;

    mutable std::shared_mutex bufferMutex_;

    std::atomic<bool> continueNotify_ = false;
    std::atomic<uint32_t> gop_ = 0;
    std::mutex idleMutex_;
    std::mutex notifyMutex_;
    std::thread notifyThread_;
    std::condition_variable dataCV_;
    std::list<uint32_t> keyIndexList_;
    std::weak_ptr<BufferDispatcherListener> listener_;
    std::unique_ptr<TimeoutTimer> writingTimer_ = nullptr;
    std::unordered_map<uint32_t, DataNotifier::Ptr> notifiers_;

    circular_buffer<DataSpec::Ptr> circularBuffer_;
    circular_buffer<MediaData::Ptr> idleVideoBuffer_;
    circular_buffer<MediaData::Ptr> idleAudioBuffer_;

    MediaData::Ptr spsBuf_ = nullptr;
    MediaData::Ptr ppsBuf_ = nullptr;
    DataSpec::Ptr refHead_ = nullptr;
    MediaDispacherMode dataMode_ = MEDIA_VIDEO_AUDIO_MIXED;
};

} // namespace Sharing
} // namespace OHOS
#endif
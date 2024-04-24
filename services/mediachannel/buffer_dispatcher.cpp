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
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include "media_channel_def.h"
namespace OHOS {
namespace Sharing {

constexpr int32_t WRITING_TIMTOUT = 30;
void BufferReceiver::SetSource(IBufferReader::Ptr dataReader)
{
    SHARING_LOGD("trace.");
    bufferReader_ = dataReader;
}

int32_t BufferReceiver::OnMediaDataNotify()
{
    SHARING_LOGD("BufferReceiver Media notified.");
    dataReady_ = true;
    notifyData_.notify_one();
    return 0;
}

int32_t BufferReceiver::OnAudioDataNotify()
{
    MEDIA_LOGD("BufferReceiver Audio notified.");
    nonBlockAudio_ = true;
    notifyAudio_.notify_one();
    return 0;
}

int32_t BufferReceiver::OnVideoDataNotify()
{
    MEDIA_LOGD("BufferReceiver Video notified.");
    nonBlockVideo_ = true;
    notifyVideo_.notify_one();
    return 0;
}

bool BufferReceiver::IsMixedReceiver()
{
    MEDIA_LOGD("trace.");
    return mixed_;
}

int32_t BufferReceiver::RequestRead(MediaType type, std::function<void(const MediaData::Ptr &data)> cb)
{
    MEDIA_LOGD("trace.");
    if (bufferReader_ == nullptr) {
        SHARING_LOGE("BufferReceiver read failed null dispatcher.");
        return -1;
    }

    if (firstMRead_ && type == MEDIA_TYPE_AV) {
        bufferReader_->NotifyReadReady(GetReceiverId(), type);
        mixed_ = true;
        firstMRead_ = false;
    } else if (firstARead_ && type == MEDIA_TYPE_AUDIO) {
        bufferReader_->NotifyReadReady(GetReceiverId(), type);
        firstARead_ = false;
    } else if (firstVRead_ && type == MEDIA_TYPE_VIDEO) {
        bufferReader_->NotifyReadReady(GetReceiverId(), type);
        firstVRead_ = false;
    }
    std::unique_lock<std::mutex> locker(mutex_);
    MEDIA_LOGD("BufferDispatcher NotifyThreadWorker before wait, receiverId: %{public}u.", GetReceiverId());
    switch (type) {
        /*  cv will waiting if pred is false;
            so set waiting audio pred (type != MEDIA_TYPE_AUDIO) to NOT block other type.*/
        case MEDIA_TYPE_AUDIO:
            MEDIA_LOGD("wait Audio, receiverId: %{public}u.", GetReceiverId());
            notifyAudio_.wait(locker, [=]() { return nonBlockAudio_ || type != MEDIA_TYPE_AUDIO; });
            nonBlockAudio_ = false;
            break;
        case MEDIA_TYPE_VIDEO:
            MEDIA_LOGD("wait Video, receiverId: %{public}u.", GetReceiverId());
            notifyVideo_.wait(locker, [=]() { return nonBlockVideo_ || type != MEDIA_TYPE_VIDEO; });
            nonBlockVideo_ = false;
            break;
        case MEDIA_TYPE_AV:
            MEDIA_LOGD("wait Mixed, receiverId: %{public}u.", GetReceiverId());
            notifyData_.wait(locker, [=]() { return dataReady_ || type != MEDIA_TYPE_AV; });
            dataReady_ = false;
            break;
        default:
            return 0;
            break;
    }

    bufferReader_->ClearDataBit(GetReceiverId(), type);
    bufferReader_->ClearReadBit(GetReceiverId(), type);
    MEDIA_LOGD("BufferDispatcher NotifyThreadWorker after wait  start read, receiverId: %{public}u.", GetReceiverId());
    int32_t ret = bufferReader_->ReadBufferData(GetReceiverId(), type, cb);
    bufferReader_->NotifyReadReady(GetReceiverId(), type);
    dataReady_ = false;

    return ret;
}

void BufferReceiver::NotifyReadStart()
{
    SHARING_LOGD("receiverId: %{public}u notify start read.", GetReceiverId());
    firstARead_ = true;
    firstVRead_ = true;
    firstMRead_ = true;
}

uint32_t BufferReceiver::GetReceiverId()
{
    MEDIA_LOGD("trace.");
    return GetId();
}

uint32_t BufferReceiver::GetDispatcherId()
{
    SHARING_LOGD("trace.");
    if (bufferReader_) {
        return bufferReader_->GetDispatcherId();
    }

    return 0;
}

void BufferReceiver::NotifyReadStop()
{
    SHARING_LOGD("receiverId: %{public}u notify stop read.", GetReceiverId());
    nonBlockAudio_ = true;
    nonBlockVideo_ = true;
    dataReady_ = true;
    notifyAudio_.notify_all();
    notifyVideo_.notify_all();
    notifyData_.notify_all();
}

void BufferReceiver::EnableKeyMode(bool enable)
{
    SHARING_LOGD("bufferReceiver id %{public}u SetKeyOnlyMode %{public}d.", GetReceiverId(), enable);
    if (keyOnly_ == true && enable == false) {
        SHARING_LOGD("Set KeyOnlyMode false, need report fast read over.");
        accelerationDone_ = true;
    }

    keyOnly_ = enable;
    if (bufferReader_ && enable) {
        bufferReader_->ClearDataBit(GetReceiverId(), MEDIA_TYPE_VIDEO);
    }

    auto listener = listener_.lock();
    if (listener) {
        listener->OnKeyModeNotify(enable);
    }
}

bool BufferReceiver::IsKeyMode()
{
    MEDIA_LOGD("trace.");
    return keyOnly_;
}

bool BufferReceiver::IsKeyRedirect()
{
    SHARING_LOGD("trace.");
    return keyRedirect_;
}

const MediaData::Ptr BufferReceiver::GetSPS()
{
    MEDIA_LOGD("trace.");
    if (bufferReader_) {
        return bufferReader_->GetSPS();
    }

    return nullptr;
}

const MediaData::Ptr BufferReceiver::GetPPS()
{
    MEDIA_LOGD("trace.");
    if (bufferReader_) {
        return bufferReader_->GetPPS();
    }

    return nullptr;
}

bool BufferReceiver::NeedAcceleration()
{
    MEDIA_LOGD("trace.");
    return accelerationDone_;
}

void BufferReceiver::DisableAcceleration()
{
    SHARING_LOGD("trace.");
    accelerationDone_ = false;
}

void BufferReceiver::SendAccelerationDone()
{
    SHARING_LOGD("trace.");
    auto listener = listener_.lock();
    if (listener) {
        listener->OnAccelerationDoneNotify();
    }
}

void BufferReceiver::EnableKeyRedirect(bool enable)
{
    SHARING_LOGD("trace.");
    if (bufferReader_ && enable) {
        bufferReader_->EnableKeyRedirect(enable);
    }
    keyRedirect_ = enable;
}

void BufferReceiver::SetBufferReceiverListener(std::weak_ptr<IBufferReceiverListener> listener)
{
    SHARING_LOGD("trace.");
    listener_ = listener;
}

using DataNotifier = BufferDispatcher::DataNotifier;

void DataNotifier::NotifyDataReceiver(MediaType type)
{
    MEDIA_LOGD("trace.");
    if (receiver_.lock() == nullptr) {
        SHARING_LOGE("target receiver NOT exist.");
        return;
    }

    if (block_) {
        return;
    }

    MEDIA_LOGD("notify target type %{public}d.", type);
    switch (type) {
        case MEDIA_TYPE_AUDIO:
            GetBufferReceiver()->OnAudioDataNotify();
            break;
        case MEDIA_TYPE_VIDEO:
            GetBufferReceiver()->OnVideoDataNotify();
            break;
        case MEDIA_TYPE_AV:
            GetBufferReceiver()->OnMediaDataNotify();
            break;
        default:
            SHARING_LOGI("none process case.");
            break;
    }
}

BufferReceiver::Ptr DataNotifier::GetBufferReceiver()
{
    MEDIA_LOGD("trace.");
    return receiver_.lock();
}

uint32_t DataNotifier::GetReceiverId()
{
    MEDIA_LOGD("trace.");
    auto receiver = receiver_.lock();
    if (receiver == nullptr) {
        SHARING_LOGE("target receiver NOT exist.");
        return INVALID_INDEX;
    }

    return receiver->GetReceiverId();
}

void DataNotifier::SetListenDispatcher(IBufferReader::Ptr dispatcher)
{
    SHARING_LOGD("trace.");
    dispatcher_ = dispatcher;
}

void DataNotifier::SetNotifyReceiver(BufferReceiver::Ptr receiver)
{
    SHARING_LOGD("trace.");
    receiver_ = receiver;
}

void DataNotifier::SetBlock()
{
    SHARING_LOGD("trace.");
    block_ = true;
}

void DataNotifier::SetNeedUpdate(bool enable, MediaType type)
{
    MEDIA_LOGD("trace.");
    if (type == MEDIA_TYPE_AUDIO) {
        needUpdateAIndex = enable;
    } else {
        needUpdateVIndex = enable;
    }
}

bool DataNotifier::DataAvailable(MediaType type)
{
    MEDIA_LOGD("trace.");
    auto dispatcher = dispatcher_.lock();
    if (dispatcher == nullptr) {
        SHARING_LOGE("target dispatcher NOT exist.");
        return false;
    }

    if (type == MEDIA_TYPE_AUDIO) {
        return audioIndex != INVALID_INDEX &&
               (audioIndex < dispatcher->GetLatestAudioIndex() || !dispatcher->IsRead(GetReceiverId(), audioIndex + 1));
    } else if (type == MEDIA_TYPE_VIDEO) {
        return videoIndex != INVALID_INDEX &&
               (videoIndex < dispatcher->GetLatestVideoIndex() || !dispatcher->IsRead(GetReceiverId(), videoIndex + 1));
    } else {
        return videoIndex != INVALID_INDEX &&
               (videoIndex < dispatcher->GetBufferSize() - 1 || !dispatcher->IsRead(GetReceiverId(), videoIndex + 1));
    }

    return false;
}

bool DataNotifier::IsMixedReceiver()
{
    MEDIA_LOGD("trace.");
    auto receiver = receiver_.lock();
    if (receiver == nullptr) {
        SHARING_LOGE("target receiver NOT exist.");
        return false;
    }

    return receiver->IsMixedReceiver();
}

uint32_t DataNotifier::GetReceiverReadIndex(MediaType type)
{
    MEDIA_LOGD("trace.");
    switch (type) {
        case MEDIA_TYPE_VIDEO:
            MEDIA_LOGD("Video Recvid:%{public}d index: %{public}d.", GetReceiverId(), videoIndex);
            return videoIndex;
            break;
        case MEDIA_TYPE_AUDIO:
            MEDIA_LOGD("Audio Recvid:%{public}d index: %{public}d.", GetReceiverId(), audioIndex);
            return audioIndex;
            break;
        case MEDIA_TYPE_AV:
            MEDIA_LOGD("Mixed Recvid:%{public}d vindex: %{public}d  aindex: %{public}d.", GetReceiverId(), videoIndex,
                       audioIndex);
            if (audioIndex != INVALID_INDEX && videoIndex != INVALID_INDEX) {
                return audioIndex <= videoIndex ? audioIndex : videoIndex;
            } else if (audioIndex == INVALID_INDEX && videoIndex == INVALID_INDEX) {
                return INVALID_INDEX;
            } else {
                return audioIndex == INVALID_INDEX ? videoIndex : audioIndex;
            }
            break;
        default:
            return INVALID_INDEX;
            break;
    }
}

bool DataNotifier::IsKeyModeReceiver()
{
    MEDIA_LOGD("trace.");
    auto receiver = receiver_.lock();
    if (receiver) {
        return receiver->IsKeyMode();
    }

    return false;
}

bool DataNotifier::IsKeyRedirectReceiver()
{
    SHARING_LOGD("trace.");
    auto receiver = receiver_.lock();
    if (receiver) {
        return receiver->IsKeyRedirect();
    }

    return false;
}

bool DataNotifier::NeedAcceleration()
{
    MEDIA_LOGD("trace.");
    auto receiver = receiver_.lock();
    if (receiver == nullptr) {
        SHARING_LOGE("target receiver NOT exist.");
        return false;
    }

    return receiver->NeedAcceleration();
}

void DataNotifier::SendAccelerationDone()
{
    SHARING_LOGD("trace.");
    auto receiver = receiver_.lock();
    if (receiver == nullptr) {
        SHARING_LOGE("target receiver NOT exist.");
        return;
    }

    receiver->SendAccelerationDone();
    receiver->DisableAcceleration();
}

BufferDispatcher::BufferDispatcher(uint32_t maxCapacity, uint32_t capacityIncrement)
{
    SHARING_LOGD("BufferDispatcher ctor, set capacity: %{public}u.", maxCapacity);
    maxBufferCapacity_ = maxCapacity;
    bufferCapacityIncrement_ = capacityIncrement;
    {
        std::lock_guard<std::mutex> lock(idleMutex_);
        idleAudioBuffer_.set_capacity(INITIAL_BUFFER_CAPACITY);
        idleVideoBuffer_.set_capacity(INITIAL_BUFFER_CAPACITY);
        for (size_t i = 0; i < INITIAL_BUFFER_CAPACITY; i++) {
            MediaData::Ptr adata = std::make_shared<MediaData>();
            MediaData::Ptr vdata = std::make_shared<MediaData>();
            adata->buff = std::make_shared<DataBuffer>();
            vdata->buff = std::make_shared<DataBuffer>();
            idleAudioBuffer_.push_back(adata);
            idleVideoBuffer_.push_back(vdata);
        }
    }

    writingTimer_ = std::make_unique<TimeoutTimer>("dispatcher-writing-timer");

    std::unique_lock<std::shared_mutex> locker(bufferMutex_);
    circularBuffer_.set_capacity(INITIAL_BUFFER_CAPACITY);
    StartDispatch();
}

BufferDispatcher::~BufferDispatcher()
{
    SHARING_LOGI("BufferDispatcher dtor.");
    running_ = false;
    StopDispatch();
    FlushBuffer();
    ReleaseIdleBuffer();
    ReleaseAllReceiver();
}

void BufferDispatcher::StartDispatch()
{
    SHARING_LOGD("trace.");
    running_ = true;
    notifyThread_ = std::thread(&BufferDispatcher::NotifyThreadWorker, this);
    std::string name = "notifyThread";
    pthread_setname_np(notifyThread_.native_handle(), name.c_str());
}

void BufferDispatcher::StopDispatch()
{
    SHARING_LOGD("trace.");
    running_ = false;
    continueNotify_ = true;

    if (writingTimer_) {
        writingTimer_.reset();
    }

    dataCV_.notify_all();
    if (notifyThread_.joinable()) {
        notifyThread_.join();
    }
}

void BufferDispatcher::SetBufferCapacity(size_t capacity)
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::shared_mutex> locker(bufferMutex_);
    circularBuffer_.set_capacity(capacity);
}

void BufferDispatcher::SetDataMode(MediaDispacherMode dataMode)
{
    SHARING_LOGD("trace.");
    dataMode_ = dataMode;
}

void BufferDispatcher::ReleaseIdleBuffer()
{
    SHARING_LOGD("BufferDispatcher idle Release Start.");
    std::unique_lock<std::mutex> locker(idleMutex_);
    for (auto &data : idleAudioBuffer_) {
        if (data != nullptr || data->buff != nullptr) {
            data->buff.reset();
        }
    }

    idleAudioBuffer_.clear();
    for (auto &data : idleVideoBuffer_) {
        if (data != nullptr || data->buff != nullptr) {
            data->buff.reset();
        }
    }

    idleVideoBuffer_.clear();
    SHARING_LOGD("BufferDispatcher idle Release End.");
}

void BufferDispatcher::FlushBuffer()
{
    SHARING_LOGD("BufferDispatcher Start flushing, dispatcherId: %{public}u.", GetDispatcherId());
    {
        std::lock_guard<std::mutex> lock(idleMutex_);
        idleAudioBuffer_.clear();
        idleVideoBuffer_.clear();
        for (size_t i = 0; i < INITIAL_BUFFER_CAPACITY; i++) {
            MediaData::Ptr adata = std::make_shared<MediaData>();
            MediaData::Ptr vdata = std::make_shared<MediaData>();
            adata->buff = std::make_shared<DataBuffer>();
            vdata->buff = std::make_shared<DataBuffer>();
            idleAudioBuffer_.push_back(adata);
            idleVideoBuffer_.push_back(vdata);
        }
    }

    std::unique_lock<std::shared_mutex> locker(bufferMutex_);
    for (auto &data : circularBuffer_) {
        if (data->mediaData != nullptr && data->mediaData->buff != nullptr) {
            data->mediaData->buff.reset();
        }
    }

    circularBuffer_.clear();
    waitingKey_ = true;
    gop_ = 0;
    audioFrameCnt_ = 0;
    videoFrameCnt_ = 0;
    ResetAllIndex();
    SHARING_LOGD("BufferDispatcher Dispatcher flushing end, dispatcherId: %{public}u.", GetDispatcherId());
}

MediaData::Ptr BufferDispatcher::RequestDataBuffer(MediaType type, uint32_t size)
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> lock(idleMutex_);
    if (size <= 0) {
        SHARING_LOGE("Size invalid.");
        return nullptr;
    }

    MediaData::Ptr retData;
    if (type == MEDIA_TYPE_VIDEO) {
        if (!idleVideoBuffer_.empty()) {
            SHARING_LOGD("video From idle.");
            retData = idleVideoBuffer_.front();
            idleVideoBuffer_.pop_front();
            if (retData == nullptr || retData->buff == nullptr) {
                MEDIA_LOGW("video From alloc when idle nullptr.");
                retData = std::make_shared<MediaData>();
                retData->buff = std::make_shared<DataBuffer>();
                retData->buff->Resize(size);

                return retData;
            }
            if (retData->buff->Size() < static_cast<int>(size)) {
                retData->buff->Resize(size);
            }
            return retData;
        }
    } else {
        if (!idleAudioBuffer_.empty()) {
            SHARING_LOGD("Audio From idle.");
            retData = idleAudioBuffer_.front();
            idleAudioBuffer_.pop_front();
            if (retData == nullptr || retData->buff == nullptr) {
                MEDIA_LOGW("Audio From alloc when idle nullptr.");
                retData = std::make_shared<MediaData>();
                retData->buff = std::make_shared<DataBuffer>();
                retData->buff->Resize(size);

                return retData;
            }
            if (retData->buff->Size() < static_cast<int>(size)) {
                retData->buff->Resize(size);
            }
            return retData;
        }
    }

    SHARING_LOGD("Audio From alloc.");
    retData = std::make_shared<MediaData>();
    retData->buff = std::make_shared<DataBuffer>();
    retData->buff->Resize(size);
    return retData;
}

void BufferDispatcher::ReturnIdleBuffer(DataSpec::Ptr &data)
{
    MEDIA_LOGD("trace.");
    std::lock_guard<std::mutex> lock(idleMutex_);
    if (data == nullptr || data->mediaData == nullptr) {
        return;
    }
    if (data->mediaData->mediaType == MEDIA_TYPE_VIDEO) {
        if (idleVideoBuffer_.size() < INITIAL_BUFFER_CAPACITY) {
            idleVideoBuffer_.push_back(data->mediaData);
            MEDIA_LOGD("data: push_back in idleVideoBuffer_, size: %{public}zu.", idleVideoBuffer_.size());
        }
    } else {
        if (idleAudioBuffer_.size() < INITIAL_BUFFER_CAPACITY) {
            idleAudioBuffer_.push_back(data->mediaData);
            MEDIA_LOGD("data: push_back in idleAudioBuffer_, size: %{public}zu.", idleAudioBuffer_.size());
        }
    }

    data.reset();
}

size_t BufferDispatcher::GetBufferSize()
{
    SHARING_LOGD("trace.");
    return circularBuffer_.size();
}

uint32_t BufferDispatcher::FindReceiverIndex(uint32_t receiverId)
{
    MEDIA_LOGD("trace.");
    if (notifiers_.find(receiverId) != notifiers_.end()) {
        return notifiers_[receiverId]->GetReadIndex();
    }

    return INVALID_INDEX;
}

bool BufferDispatcher::IsRecevierExist(uint32_t receiverId)
{
    SHARING_LOGD("trace.");
    auto notifier = GetNotifierByReceiverId(receiverId);
    if (notifier == nullptr) {
        return false;
    }

    return true;
}

void BufferDispatcher::EnableKeyMode(bool enable)
{
    SHARING_LOGD("trace.");
    keyOnly_ = enable;
}

int32_t BufferDispatcher::AttachReceiver(BufferReceiver::Ptr receiver)
{
    SHARING_LOGD("trace.");
    if (receiver == nullptr) {
        return -1;
    }

    if (IsRecevierExist(receiver->GetReceiverId())) {
        SHARING_LOGE("Exist.");
        return 0;
    }

    receiver->NotifyReadStart();
    std::lock_guard<std::mutex> locker(notifyMutex_);
    if (readRefFlag_ == 0xFFFF) {
        SHARING_LOGE("readRefFlag limited.");
        return -1;
    }

    DataNotifier::Ptr notifier = std::make_shared<DataNotifier>();
    notifier->SetListenDispatcher(shared_from_this());
    notifier->SetNotifyReceiver(receiver);

    auto usableRef = ~readRefFlag_ & (-(~readRefFlag_));

    if ((usableRef & (usableRef - 1)) != 0) {
        SHARING_LOGE("usableRef: %{public}d invalid.", usableRef);
        return -1;
    }

    readRefFlag_ |= usableRef;
    notifier->SetReadIndex(static_cast<uint32_t>(log2(usableRef)));
    SHARING_LOGI("receiverId: %{public}d, readIndex: %{public}d, usableRef: %{public}d, readRefFlag_: %{public}d.",
                 receiver->GetReceiverId(), notifier->GetReadIndex(), usableRef, readRefFlag_);
    receiver->SetSource(shared_from_this());
    notifiers_.emplace(receiver->GetReceiverId(), notifier);

    if (circularBuffer_.empty()) {
        notifier->audioIndex = INVALID_INDEX;
        notifier->videoIndex = INVALID_INDEX;
        SetReceiverDataRef(receiver->GetReceiverId(), MEDIA_TYPE_AUDIO, false);
        SetReceiverDataRef(receiver->GetReceiverId(), MEDIA_TYPE_VIDEO, false);
        SHARING_LOGD("BufferDispatcher Attach when buffer empty  RecvId: %{public}d.", receiver->GetReceiverId());
        videoNeedActivate_ = true;
        audioNeedActivate_ = true;
        return 0;
    }

    if (dataMode_ == MEDIA_AUDIO_ONLY) {
        notifier->audioIndex = circularBuffer_.size() - 1;
        SetReceiverDataRef(receiver->GetReceiverId(), MEDIA_TYPE_AUDIO, true);
        notifier->videoIndex = INVALID_INDEX;
        SetReceiverDataRef(receiver->GetReceiverId(), MEDIA_TYPE_VIDEO, false);
        SHARING_LOGD("BufferDispatcher Attach when Keyindex List empty  RecvId: %{public}d.",
                     receiver->GetReceiverId());
    } else {
        if (!keyIndexList_.empty()) {
            SHARING_LOGD("BufferDispatcher Attach with Keyindex  RecvId: %{public}d  KeyIndex:%{public}d.",
                         receiver->GetReceiverId(), keyIndexList_.back());
            uint32_t tempIndex = FindNextIndex(keyIndexList_.back(), MEDIA_TYPE_AUDIO);
            notifier->audioIndex = tempIndex == keyIndexList_.back() ? INVALID_INDEX : tempIndex;
            notifier->videoIndex = keyIndexList_.back();
            bool isAudioReady = tempIndex != INVALID_INDEX ? true : false;
            SetReceiverDataRef(receiver->GetReceiverId(), MEDIA_TYPE_AUDIO, isAudioReady);
            SetReceiverDataRef(receiver->GetReceiverId(), MEDIA_TYPE_VIDEO, true);
            if (lastAudioIndex_ == INVALID_INDEX) {
                audioNeedActivate_ = true;
            }
        } else {
            SHARING_LOGD("BufferDispatcher Attach with Non Keyindex Exist RecvId: %{public}d.",
                         receiver->GetReceiverId());
            uint32_t tempIndex = FindLastIndex(MEDIA_TYPE_AUDIO);
            notifier->audioIndex = tempIndex;
            notifier->videoIndex = INVALID_INDEX;
            SetReceiverDataRef(receiver->GetReceiverId(), MEDIA_TYPE_AUDIO, true);
            SetReceiverDataRef(receiver->GetReceiverId(), MEDIA_TYPE_VIDEO, false);
            if (lastAudioIndex_ == INVALID_INDEX) {
                audioNeedActivate_ = true;
            }
        }
    }

    return 0;
}

int32_t BufferDispatcher::DetachReceiver(BufferReceiver::Ptr receiver)
{
    SHARING_LOGI("buffer dispatcher: Detach receiver in.");
    if (receiver == nullptr) {
        SHARING_LOGE("buffer dispatcher: Detach receiver failed - null receiver.");
        return -1;
    }

    if (!IsRecevierExist(receiver->GetReceiverId())) {
        SHARING_LOGE("BufferDispatcher AttachReceiver No Vaild Recevier Exist.");
        return 0;
    }

    auto notifier = GetNotifierByReceiverPtr(receiver);
    if (notifier == nullptr) {
        SHARING_LOGE("buffer dispatcher: Detach receiver failed - no find receiver in notifiers.");
        return -1;
    }

    std::lock_guard<std::mutex> locker(notifyMutex_);
    notifier->SetBlock();
    SetReceiverReadRef(receiver->GetReceiverId(), MEDIA_TYPE_AUDIO, false);
    SetReceiverReadRef(receiver->GetReceiverId(), MEDIA_TYPE_VIDEO, false);

    readRefFlag_ &= ~(RECV_FLAG_BASE << notifier->GetReadIndex());
    notifiers_.erase(receiver->GetReceiverId());
    SHARING_LOGI("now refFlag: %{public}d.", readRefFlag_);
    return 0;
}

int32_t BufferDispatcher::DetachReceiver(uint32_t receiverId, DataNotifier::Ptr notifier)
{
    SHARING_LOGI("buffer dispatcher: Detach notifier in.");
    if (notifier == nullptr) {
        SHARING_LOGE("buffer dispatcher: Detach receiver failed - null notifier.");
        return -1;
    }
    notifier->SetBlock();
    SetReceiverReadRef(receiverId, MEDIA_TYPE_AUDIO, false);
    SetReceiverReadRef(receiverId, MEDIA_TYPE_VIDEO, false);

    readRefFlag_ &= ~(RECV_FLAG_BASE << notifier->GetReadIndex());
    notifiers_.erase(receiverId);
    SHARING_LOGI("now refFlag: %{public}d.", readRefFlag_);
    return 0;
}

void BufferDispatcher::ReleaseAllReceiver()
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> locker(notifyMutex_);
    for (auto it = notifiers_.begin(); it != notifiers_.end();) {
        auto notifier = it->second;
        if (notifier == nullptr) {
            ++it;
            continue;
        }

        auto receiver = notifier->GetBufferReceiver();
        if (receiver == nullptr) {
            ++it;
            continue;
        }

        auto receiverId = receiver->GetReceiverId();
        if (notifiers_.find(receiverId) != notifiers_.end()) {
            auto notifierFind = notifiers_[receiverId];
            ++it;
            DetachReceiver(receiverId, notifierFind);
        } else {
            ++it;
            SHARING_LOGE("buffer dispatcher: Detach receiver failed - no find receiver in notifiers.");
        }
    }

    notifiers_.clear();
    SHARING_LOGD("release all receiver out.");
}

void BufferDispatcher::SetBufferDispatcherListener(BufferDispatcherListener::Ptr listener)
{
    SHARING_LOGD("trace.");
    listener_ = listener;
    writingTimer_->StartTimer(WRITING_TIMTOUT, "waiting for continuous data inputs", [this]() {
        if (!writing_) {
            SHARING_LOGI("writing timeout");
            auto listener = listener_.lock();
            if (listener) {
                listener->OnWriteTimeout();
            }
        } else {
            SHARING_LOGI("restart timer");
            writing_ = false;
        }
    }, true);
}

DataNotifier::Ptr BufferDispatcher::GetNotifierByReceiverPtr(BufferReceiver::Ptr receiver)
{
    SHARING_LOGD("trace.");
    if (receiver == nullptr) {
        return nullptr;
    }
    return GetNotifierByReceiverId(receiver->GetReceiverId());
}

DataNotifier::Ptr BufferDispatcher::GetNotifierByReceiverId(uint32_t receiverId)
{
    MEDIA_LOGD("trace.");
    std::lock_guard<std::mutex> locker(notifyMutex_);
    if (notifiers_.find(receiverId) != notifiers_.end()) {
        return notifiers_[receiverId];
    }

    return nullptr;
}

int32_t BufferDispatcher::ReadBufferData(uint32_t receiverId, MediaType type,
                                         std::function<void(const MediaData::Ptr &data)> cb)
{
    MEDIA_LOGD("in, receiverId: %{public}u.", receiverId);
    auto notifier = GetNotifierByReceiverId(receiverId);
    if (notifier == nullptr) {
        SHARING_LOGE("notifier is nullptr.");
        return -1;
    }

    std::shared_lock<std::shared_mutex> locker(bufferMutex_);
    uint32_t readIndex = notifier->GetReceiverReadIndex(type);
    if (readIndex >= circularBuffer_.size()) {
        SHARING_LOGE("Read wrong index exceed size.");
        return -1;
    }

    if ((keyOnly_ || notifier->IsKeyModeReceiver()) && type == MEDIA_TYPE_VIDEO &&
        !IsKeyVideoFrame(circularBuffer_.at(readIndex))) {
        UpdateReceiverReadIndex(receiverId, readIndex, type);
        SHARING_LOGE("Read Non Key Video in KeyOnly Mode index: %{public}u.", readIndex);
        return -1;
    }

    if (IsDataReaded(receiverId, circularBuffer_.at(readIndex))) {
        UpdateReceiverReadIndex(receiverId, readIndex, type);
        return -1;
    }

    readIndex = notifier->GetReceiverReadIndex(type);
    if (readIndex >= circularBuffer_.size()) {
        return -1;
    }

    auto data = circularBuffer_.at(readIndex);
    if (data == nullptr) {
        SHARING_LOGE("BufferDispatcher Read data nullptr.");
        return -1;
    }

    if (IsKeyVideoFrame(data)) {
        int32_t bufferVideoCacheCnt = 0;
        for (size_t i = readIndex + 1; i < circularBuffer_.size(); i++) {
            if (circularBuffer_[i]->mediaData->mediaType == MEDIA_TYPE_VIDEO)
                bufferVideoCacheCnt++;
        }
        MEDIA_LOGD("TEST STATISTIC:interval: buffer cache %{public}d frames.", bufferVideoCacheCnt);
    }

    SetReceiverReadFlag(receiverId, data);
    if (cb != nullptr) {
        cb(data->mediaData);
    }

    MEDIA_LOGD(
        "Current data readed, Recvid:%{public}d, remain %{public}zu data, readIndex: %{public}u, "
        "readtype: %{public}d, diff: %{public}zu.",
        receiverId, circularBuffer_.size(), readIndex, int32_t(type), circularBuffer_.size() - readIndex);
    UpdateReceiverReadIndex(receiverId, readIndex, type);
    return 0;
}

int32_t BufferDispatcher::InputData(const MediaData::Ptr &data)
{
    MEDIA_LOGD("inputmediatype: %{public}d, keyFrame: %{public}d, pts: %{public}" PRIu64 ".", data->mediaType,
               data->keyFrame, data->pts);
    if (data == nullptr || data->buff == nullptr) {
        SHARING_LOGE("data nullptr.");
        return -1;
    }

    if (!writing_) {
        writing_ = true;
    }

    DataSpec::Ptr dataSpec = std::make_shared<DataSpec>();
    dataSpec->mediaData = data;
    if (dataMode_ == MEDIA_AUDIO_ONLY) {
        WriteDataIntoBuffer(dataSpec);
    } else {
        PreProcessDataSpec(dataSpec);
    }

    if (circularBuffer_.size() > 0) {
        MEDIA_LOGD("inputmediatype: %{public}d, keyFrame: %{public}d, pts: %{public}" PRIu64 ".",
                   circularBuffer_[circularBuffer_.size() - 1]->mediaData->mediaType,
                   circularBuffer_[circularBuffer_.size() - 1]->mediaData->keyFrame,
                   circularBuffer_[circularBuffer_.size() - 1]->mediaData->pts);
    }

    MEDIA_LOGD(
        "dispatcherId: %{public}u, after InputData, current circularBuffer_ size: %{public}zu, "
        "idleVideoBuffer_ size: %{public}zu, idle_audioBuffer_ size: %{public}zu, "
        "keyFrame: %{public}s, data size: %{public}d, adataCount:%{public}d.",
        GetDispatcherId(), circularBuffer_.size(), idleVideoBuffer_.size(), idleAudioBuffer_.size(),
        data->keyFrame ? "true" : "false", data->buff->Size(), audioFrameCnt_);
    return 0;
}

void BufferDispatcher::PreProcessDataSpec(const DataSpec::Ptr &dataSpec)
{
    MEDIA_LOGD("trace.");
    if (waitingKey_) {
        if (IsAudioData(dataSpec)) {
        } else if (!IsKeyVideoFrame(dataSpec)) {
            SHARING_LOGD("BufferDispatcher Waiting First Key Video Frame.");
            return;
        } else {
            SHARING_LOGD("BufferDispatcher received first key video frame and restore from uncontinuous...Flushing.");
            FlushBuffer();
            baseCounter_++;
            capacityEvaluating_ = true;
            waitingKey_ = false;
        }
    } else {
        if (capacityEvaluating_) {
            ReCalculateCapacity(IsKeyVideoFrame(dataSpec));
        }
    }

    WriteDataIntoBuffer(dataSpec);
}

int32_t BufferDispatcher::WriteDataIntoBuffer(const DataSpec::Ptr &data)
{
    MEDIA_LOGD("trace.");
    if (data->mediaData == nullptr || data->mediaData->buff == nullptr) {
        SHARING_LOGE("null data.");
        return -1;
    }

    if (NeedExtendToDBCapacity()) {
        SHARING_LOGD("BufferDispatcher buffer Extended to %{public}d  CRTL_SIZE.", doubleBufferCapacity_);
        SetBufferCapacity(doubleBufferCapacity_);
    }

    if (NeedRestoreToNormalCapacity()) {
        std::unique_lock<std::shared_mutex> locker(bufferMutex_);
        int32_t popSize = circularBuffer_.size() - INITIAL_BUFFER_CAPACITY;
        for (int32_t i = 0; i < popSize; i++) {
            if (HeadFrameNeedReserve()) {
                MEDIA_LOGW(
                    "dispatcherId: %{public}u, need reserve but pop, mediaType: "
                    "%{public}d, keyFrame: %{public}s, pts: %{public}" PRIu64".",
                    GetDispatcherId(), int32_t(circularBuffer_.front()->mediaData->mediaType),
                    circularBuffer_.front()->mediaData->keyFrame ? "true" : "false",
                    circularBuffer_.front()->mediaData->pts);
            }

            MEDIA_LOGW(
                "dispatcherId: %{public}u, delete data, mediaType: %{public}d, keyFrame: "
                "%{public}s, pts: %{public}" PRIu64", reserveFlag: %{public}x.",
                GetDispatcherId(), int32_t(circularBuffer_.front()->mediaData->mediaType),
                circularBuffer_.front()->mediaData->keyFrame ? "true" : "false",
                circularBuffer_.front()->mediaData->pts, circularBuffer_.front()->reserveFlag.load());
            circularBuffer_.pop_front();
            audioFrameCnt_--;
            UpdateIndex();
        }

        baseBufferCapacity_ = INITIAL_BUFFER_CAPACITY;
        doubleBufferCapacity_ = INITIAL_BUFFER_CAPACITY * 2; //2 : increasement
        SHARING_LOGD("BufferDispatcher buffer Extended to %{public}d  NORMALSIZE.", baseBufferCapacity_);
        circularBuffer_.set_capacity(baseBufferCapacity_);
    }

    if (IsKeyVideoFrame(data) && !keyOnly_) {
        EraseOldGopDatas();
    }

    bool updateIndexFlag = false;
    std::unique_lock<std::shared_mutex> locker(bufferMutex_);
    if (circularBuffer_.size() >= circularBuffer_.capacity()) {
        updateIndexFlag = true;
    }

    if (updateIndexFlag) {
        uint32_t nextDeleteIndex = 1;
        if (IsVideoData(data)) {
            nextDeleteIndex = FindNextDeleteVideoIndex();
        }

        for (size_t i = 0; i <= nextDeleteIndex; i++) {
            MediaType headType = circularBuffer_.front()->mediaData->mediaType;
            DataSpec::Ptr retBuff = circularBuffer_.front();
            if (HeadFrameNeedReserve()) {
                MEDIA_LOGW(
                    "dispatcherId: %{public}u, need reserve but pop, mediaType: "
                    "%{public}d, keyFrame: %{public}s, pts: %{public}" PRIu64".",
                    GetDispatcherId(), int32_t(retBuff->mediaData->mediaType),
                    retBuff->mediaData->keyFrame ? "true" : "false", retBuff->mediaData->pts);
            }

            MEDIA_LOGW(
                "dispatcherId: %{public}u, delete data, mediaType: %{public}d, "
                "keyFrame: %{public}s, pts: %{public}" PRIu64", reserveFlag: %{public}x.",
                GetDispatcherId(), int32_t(circularBuffer_.front()->mediaData->mediaType),
                circularBuffer_.front()->mediaData->keyFrame ? "true" : "false",
                circularBuffer_.front()->mediaData->pts, circularBuffer_.front()->reserveFlag.load());

            circularBuffer_.pop_front();
            ReturnIdleBuffer(retBuff);
            headType == MEDIA_TYPE_AUDIO ? audioFrameCnt_-- : videoFrameCnt_--;
            UpdateIndex();
        }
    }

    data->reserveFlag = 0;
    MEDIA_LOGD("WriteDataIntoBuffer data type: %{public}d, keyFrame: %{public}s, pts: %{public}" PRIu64
               ", cur_size: %{public}zu, capacity: %{public}zu dispatcher[%{public}u].",
               int32_t(data->mediaData->mediaType), data->mediaData->keyFrame ? "true" : "false",
               data->mediaData->pts, circularBuffer_.size(), circularBuffer_.capacity(), GetDispatcherId());
    circularBuffer_.push_back(data);
    if (IsAudioData(data)) {
        lastAudioIndex_ = circularBuffer_.size() - 1;
        ActiveDataRef(MEDIA_TYPE_AUDIO, false);
        audioFrameCnt_++;
    } else {
        lastVideoIndex_ = circularBuffer_.size() - 1;
        if (!keyOnly_ || (keyOnly_ && IsKeyVideoFrame(data))) {
            ActiveDataRef(MEDIA_TYPE_VIDEO, IsKeyVideoFrame(data));
        }
        videoFrameCnt_++;
    }

    if (audioNeedActivate_ && IsAudioData(data)) {
        MEDIA_LOGD("BufferDispatcher ActivateReceiverIndex By AudioData.");
        ActivateReceiverIndex(circularBuffer_.size() - 1, MEDIA_TYPE_AUDIO);
    }

    if (IsKeyVideoFrame(data)) {
        uint32_t keyIndex = circularBuffer_.size() - 1;
        {
            std::lock_guard<std::mutex> indexLocker(notifyMutex_);
            keyIndexList_.push_back(keyIndex);
        }
        if (videoNeedActivate_) {
            MEDIA_LOGD("BufferDispatcher ActivateReceiverIndex By KeyVideo Frame index: %{public}d.", keyIndex);
            ActivateReceiverIndex(keyIndex, MEDIA_TYPE_VIDEO);
        }
        if (keyRedirect_) {
            OnKeyRedirect();
            EnableKeyRedirect(false);
        }
    }

    continueNotify_ = true;
    dataCV_.notify_one();
    return 0;
}

void BufferDispatcher::EraseOldGopDatas()
{
    MEDIA_LOGD("BufferDispatcher Delete old datas In.");
    if (dataMode_ == MEDIA_AUDIO_ONLY) {
        FlushBuffer();
        return;
    }

    std::unique_lock<std::shared_mutex> locker(bufferMutex_);
    uint32_t nextKey = 0;
    {
        std::lock_guard<std::mutex> lock(notifyMutex_);
        if (!keyIndexList_.empty() && keyIndexList_.back() > 0) {
            MEDIA_LOGD("find next key listsize %{public}zu, back:%{public}d.", keyIndexList_.size(),
                       keyIndexList_.back());
            nextKey = keyIndexList_.back();
            keyIndexList_.clear();
            keyIndexList_.push_back(nextKey);
        }
    }

    MEDIA_LOGD("erase between 0 to next Video Frame %{public}d.", nextKey);
    DeleteHeadDatas(nextKey, false);
    nextKey = FindNextDeleteVideoIndex();
    DeleteHeadDatas(nextKey, true);
    std::string indexs;

    MEDIA_LOGD("circularBuffer_ size: %{public}zu.", circularBuffer_.size());
    for (auto &keyIndex : keyIndexList_) {
        indexs += std::to_string(keyIndex) + ", ";
        MEDIA_LOGD("keyIndex update to %{public}d.", keyIndex);
    }

    MEDIA_LOGD("current keyIndex: %{public}s.", indexs.c_str());
}

void BufferDispatcher::DeleteHeadDatas(uint32_t size, bool forceDelete)
{
    MEDIA_LOGD("trace.");
    if (size <= 0) {
        MEDIA_LOGW("invalid Size, dispatcherId: %{public}u!", GetDispatcherId());
        return;
    }

    for (size_t i = 0; i < size; i++) {
        if (HeadFrameNeedReserve() && !forceDelete) {
            MEDIA_LOGD("index %{public}zu need reserve.", i);
            break;
        }
        if (circularBuffer_.empty()) {
            return;
        }
        DataSpec::Ptr retBuff = circularBuffer_.front();
        MEDIA_LOGD("BufferDispatcher pop out headtype  %{public}d.", retBuff->mediaData->mediaType);
        retBuff->mediaData->mediaType == MEDIA_TYPE_AUDIO ? audioFrameCnt_-- : videoFrameCnt_--;
        if (HeadFrameNeedReserve()) {
            MEDIA_LOGW("dispatcherId: %{public}u, need reserve but pop, mediaType: "
                 "%{public}d, keyFrame: %{public}s, pts: %{public}" PRIu64 ".",
                 GetDispatcherId(), int32_t(retBuff->mediaData->mediaType),
                 retBuff->mediaData->keyFrame ? "true" : "false",
                 retBuff->mediaData->pts);
        }

        MEDIA_LOGW(
            "dispatcherId: %{public}u, delete data, mediaType: %{public}d, keyFrame: %{public}s, pts: %{public}" PRIu64
            ", reserveFlag: %{public}x.",
            GetDispatcherId(), int32_t(circularBuffer_.front()->mediaData->mediaType),
            circularBuffer_.front()->mediaData->keyFrame ? "true" : "false", circularBuffer_.front()->mediaData->pts,
            circularBuffer_.front()->reserveFlag.load());
        circularBuffer_.pop_front();
        ReturnIdleBuffer(retBuff);
        UpdateIndex();
    }

    if (circularBuffer_.size() < baseBufferCapacity_ && circularBuffer_.capacity() > baseBufferCapacity_) {
        MEDIA_LOGD("capacity return to base %{public}d.", baseBufferCapacity_);
        circularBuffer_.set_capacity(baseBufferCapacity_);
    }
}

void BufferDispatcher::UpdateIndex()
{
    MEDIA_LOGD("trace.");
    std::lock_guard<std::mutex> locker(notifyMutex_);
    if (!keyIndexList_.empty() && keyIndexList_.front() == 0) {
        keyIndexList_.pop_front();
        MEDIA_LOGD("BufferDispatcher pop out first  0 keyIndex after listsize %{public}zu.", keyIndexList_.size());
    }

    for (auto &keyIndex : keyIndexList_) {
        if (keyIndex > 0) {
            keyIndex--;
        }
    }

    for (auto &[recvId, notifier] : notifiers_) {
        if (notifier->videoIndex > 0 && notifier->videoIndex != INVALID_INDEX) {
            notifier->videoIndex--;
        }
        if (notifier->audioIndex > 0 && notifier->audioIndex != INVALID_INDEX) {
            notifier->audioIndex--;
        }
    }

    if (lastVideoIndex_ > 0 && lastVideoIndex_ != INVALID_INDEX) {
        lastVideoIndex_--;
    }

    if (lastAudioIndex_ > 0 && lastAudioIndex_ != INVALID_INDEX) {
        lastAudioIndex_--;
    }
}

uint32_t BufferDispatcher::FindNextDeleteVideoIndex()
{
    MEDIA_LOGD("trace.");
    for (size_t i = 0; i < circularBuffer_.size(); i++) {
        if (circularBuffer_[i]->mediaData != nullptr && circularBuffer_[i]->mediaData->mediaType == MEDIA_TYPE_VIDEO) {
            return i;
        }
    }

    return 0;
}

uint32_t BufferDispatcher::FindLastIndex(MediaType type)
{
    SHARING_LOGD("trace.");
    if (circularBuffer_.empty()) {
        return INVALID_INDEX;
    }

    return type == MEDIA_TYPE_AUDIO ? lastAudioIndex_ : lastVideoIndex_;
}

void BufferDispatcher::UpdateReceiverReadIndex(uint32_t receiverId, const uint32_t readIndex, MediaType type)
{
    MEDIA_LOGD("trace.");
    uint32_t nextIndex = FindNextIndex(readIndex, type, receiverId);
    bool noAvaliableData = false;
    if (nextIndex == readIndex) {
        noAvaliableData = true;
    }

    auto notifier = GetNotifierByReceiverId(receiverId);
    if (notifier == nullptr) {
        SHARING_LOGE("notifier is nullptr.");
        return;
    }

    bool readOver = circularBuffer_.size() - readIndex < 3;
    if (readOver && notifier->NeedAcceleration() && type == MEDIA_TYPE_VIDEO) {
        SHARING_LOGD("BufferDispatcher SendAccelerationDone.");
        notifier->SendAccelerationDone();
    }

    notifier->SetNeedUpdate(noAvaliableData, type);

    if (type == MEDIA_TYPE_VIDEO) {
        notifier->videoIndex = nextIndex;
    } else if (type == MEDIA_TYPE_AUDIO) {
        notifier->audioIndex = nextIndex;
    } else {
        notifier->videoIndex = nextIndex;
        notifier->audioIndex = nextIndex;
    }

    MEDIA_LOGD("After UpdateReceiverReadIndex  type %{public}d, aindex %{public}d, vindex %{public}d.", type,
               notifier->audioIndex, notifier->videoIndex);
}

uint32_t BufferDispatcher::FindNextIndex(uint32_t index, MediaType type)
{
    MEDIA_LOGD("trace.");
    if (index + 1 >= circularBuffer_.size() || index == INVALID_INDEX) {
        return index;
    }

    if (type == MEDIA_TYPE_AV) {
        return index + 1;
    }

    for (size_t i = index + 1; i < circularBuffer_.size(); i++) {
        if (circularBuffer_[i] && circularBuffer_[i]->mediaData && circularBuffer_[i]->mediaData->mediaType == type) {
            if (keyOnly_ && type == MEDIA_TYPE_VIDEO && !IsKeyVideoFrame(circularBuffer_[i])) {
                continue;
            } else {
                return i;
            }
        }
    }

    return index;
}

uint32_t BufferDispatcher::FindNextIndex(uint32_t index, MediaType type, uint32_t receiverId)
{
    MEDIA_LOGD("trace.");
    if (index + 1 >= circularBuffer_.size() || index == INVALID_INDEX) {
        return index;
    }

    if (type == MEDIA_TYPE_AV) {
        return index + 1;
    }

    auto notifier = GetNotifierByReceiverId(receiverId);
    if (notifier == nullptr) {
        SHARING_LOGE("FindNextIndex GetNotifier nullptr.");
        return INVALID_INDEX;
    }

    bool keyModeReceiver = notifier->IsKeyModeReceiver();
    for (size_t i = index + 1; i < circularBuffer_.size(); i++) {
        if (circularBuffer_[i] && circularBuffer_[i]->mediaData && circularBuffer_[i]->mediaData->mediaType == type) {
            if ((keyOnly_ || keyModeReceiver) && type == MEDIA_TYPE_VIDEO) {
                if (!IsKeyVideoFrame(circularBuffer_[i])) {
                    continue;
                } else {
                    for (size_t bIndex = index + 1; bIndex < i; bIndex++) {
                        SetReceiverReadFlag(receiverId, circularBuffer_[bIndex]);
                    }
                    return i;
                }
            } else {
                return i;
            }
        }
    }

    return index;
}

void BufferDispatcher::ResetAllIndex()
{
    SHARING_LOGD("trace.");
    std::lock_guard<std::mutex> locker(notifyMutex_);
    keyIndexList_.clear();
    for (auto &[recvId, notifier] : notifiers_) {
        notifier->videoIndex = INVALID_INDEX;
        notifier->audioIndex = INVALID_INDEX;
    }

    videoNeedActivate_ = true;
    audioNeedActivate_ = true;
    lastAudioIndex_ = INVALID_INDEX;
    lastVideoIndex_ = INVALID_INDEX;
}

bool BufferDispatcher::IsDataReaded(uint32_t receiverId, DataSpec::Ptr &dataSpec)
{
    MEDIA_LOGD("trace.");
    uint32_t index = FindReceiverIndex(receiverId);
    if (index == INVALID_INDEX) {
        return false;
    }

    return dataSpec->reserveFlag & RECV_FLAG_BASE << index;
}

bool BufferDispatcher::IsVideoData(const DataSpec::Ptr &dataSpec)
{
    MEDIA_LOGD("trace.");
    if (dataSpec == nullptr || dataSpec->mediaData == nullptr) {
        SHARING_LOGE("BufferDispatcher EMPTY dataSpec OR mediadata.");
        return false;
    }

    return dataSpec->mediaData->mediaType == MEDIA_TYPE_VIDEO;
}

bool BufferDispatcher::IsAudioData(const DataSpec::Ptr &dataSpec)
{
    MEDIA_LOGD("trace.");
    if (dataSpec == nullptr || dataSpec->mediaData == nullptr) {
        SHARING_LOGE("BufferDispatcher EMPTY dataSpec OR mediadata.");
        return false;
    }

    return dataSpec->mediaData->mediaType == MEDIA_TYPE_AUDIO;
}

bool BufferDispatcher::IsKeyVideoFrame(const DataSpec::Ptr &dataSpec)
{
    MEDIA_LOGD("trace.");
    if (dataSpec == nullptr || dataSpec->mediaData == nullptr) {
        SHARING_LOGE("BufferDispatcher EMPTY dataSpec OR mediadata.");
        return false;
    }

    return IsVideoData(dataSpec) && dataSpec->mediaData->keyFrame;
}

bool BufferDispatcher::HeadFrameNeedReserve()
{
    MEDIA_LOGD("trace.");
    if (!circularBuffer_.empty()) {
        uint8_t temp = readRefFlag_;
        MEDIA_LOGD("IsHeadFrameNeedReserve Head reserveFlag %{public}d readRefFlag_ %{public}d.",
                   circularBuffer_.front()->reserveFlag.load(), readRefFlag_);
        return temp ^ circularBuffer_.front()->reserveFlag;
    }

    return false;
}

bool BufferDispatcher::NeedExtendToDBCapacity()
{
    MEDIA_LOGD("trace.");
    std::shared_lock<std::shared_mutex> locker(bufferMutex_);
    return (circularBuffer_.size() >= circularBuffer_.capacity() &&
            circularBuffer_.capacity() < doubleBufferCapacity_ && HeadFrameNeedReserve());
}

bool BufferDispatcher::NeedRestoreToNormalCapacity()
{
    MEDIA_LOGD("trace.");
    std::shared_lock<std::shared_mutex> locker(bufferMutex_);
    return audioFrameCnt_ >= circularBuffer_.capacity() && circularBuffer_.capacity() != INITIAL_BUFFER_CAPACITY;
}

void BufferDispatcher::ReCalculateCapacity(bool keyFrame)
{
    MEDIA_LOGD("trace.");
    baseCounter_++;
    if (baseCounter_ >= maxBufferCapacity_) {
        SHARING_LOGE("BufferDispatcher too many Audiodata need Set Capacity to default.");
    }

    if (baseCounter_ >= circularBuffer_.capacity() && !keyFrame) {
        uint32_t tmpSize = circularBuffer_.capacity() + bufferCapacityIncrement_ < maxBufferCapacity_
                               ? circularBuffer_.capacity() + bufferCapacityIncrement_
                               : maxBufferCapacity_;
        doubleBufferCapacity_ = tmpSize * 2 < maxBufferCapacity_ ? tmpSize * 2 : maxBufferCapacity_; // 2: increasement
        SHARING_LOGD("BufferDispatcher buffer Extended to %{public}d in adaptive capacity calculating.", tmpSize);
        SetBufferCapacity(tmpSize);
        return;
    }

    if (keyFrame) {
        baseBufferCapacity_ = baseCounter_ + bufferCapacityIncrement_ < maxBufferCapacity_
                                  ? baseCounter_ + bufferCapacityIncrement_
                                  : maxBufferCapacity_;
        if (baseBufferCapacity_ < INITIAL_BUFFER_CAPACITY) {
            baseBufferCapacity_ = INITIAL_BUFFER_CAPACITY;
        }
        doubleBufferCapacity_ = baseBufferCapacity_ * 2 < maxBufferCapacity_ // 2: increasement
                                ? baseBufferCapacity_ * 2 // 2: increasement
                                : maxBufferCapacity_;
        gop_ = baseCounter_;
        capacityEvaluating_ = gop_ > 0 ? false : true;
        SetBufferCapacity(baseBufferCapacity_);
        baseCounter_ = 0;
        SHARING_LOGD(
            "The gop is %{public}d and BufferDispatcher buffer Extended to %{public}d on base capacity confirmed.",
            GetCurrentGop(), baseBufferCapacity_);
    }
}

int32_t BufferDispatcher::NotifyThreadWorker(void *userParam)
{
    SHARING_LOGI("BufferDispatcher DataNotifier thread in.");
    BufferDispatcher *dispatcher = (BufferDispatcher *)userParam;
    while (dispatcher->running_) {
        std::unique_lock<std::mutex> locker(dispatcher->notifyMutex_);
        uint32_t notifyRef = dispatcher->dataBitRef_ & dispatcher->recvBitRef_;
        MEDIA_LOGD("DataBitRef %{public}u   recvBitRef_ %{public}d   notifyRef_ %{public}d.",
                   dispatcher->dataBitRef_.load(), dispatcher->recvBitRef_.load(), notifyRef);

        for (auto &[recvId, notifier] : dispatcher->notifiers_) {
            auto index = notifier->GetReadIndex();
            if (((RECV_FLAG_BASE << (index * 2)) & notifyRef) || // 2: fix offset, get data type
                ((RECV_FLAG_BASE << (index * 2 + 1)) & notifyRef)) { // 2: fix offset, get data type
                MediaType notifyType;
                if (notifier->IsMixedReceiver()) {
                    notifyType = MEDIA_TYPE_AV;
                } else {
                    notifyType = (((RECV_FLAG_BASE << (index * 2)) & notifyRef) ? // 2: fix offset, get data type
                                    MEDIA_TYPE_AUDIO : MEDIA_TYPE_VIDEO); // 2: fix offset, get data type
                }
                MEDIA_LOGD("notify the receiveId: %{public}d, notifyType: %{public}d, notifyRef: %{public}x.", recvId,
                           notifyType, notifyRef);
                notifier->NotifyDataReceiver(notifyType);
            }
        }

        dispatcher->dataCV_.wait(locker, [&dispatcher]() { return dispatcher->continueNotify_.load(); });
        dispatcher->continueNotify_ = false;
    }

    return 0;
}

void BufferDispatcher::NotifyReadReady(uint32_t receiverId, MediaType type)
{
    MEDIA_LOGD("trace.");
    auto notifier = GetNotifierByReceiverId(receiverId);
    if (notifier == nullptr) {
        SHARING_LOGE("notifier is nullptr.");
        return;
    }

    std::shared_lock<std::shared_mutex> lock(bufferMutex_);
    std::unique_lock<std::mutex> locker(notifyMutex_);
    if (type == MEDIA_TYPE_AV) {
        SetReceiverReadRef(receiverId, MEDIA_TYPE_VIDEO, true);
        SetReceiverReadRef(receiverId, MEDIA_TYPE_AUDIO, true);
    } else {
        SetReceiverReadRef(receiverId, type, true);
    }

    bool dataAvaliable = notifier->DataAvailable(type);
    MEDIA_LOGW("receiverId %{public}d  MediaType %{public}d  dataAvaliable %{public}d.", receiverId, type,
               dataAvaliable);
    if (type == MEDIA_TYPE_AV) {
        SetReceiverDataRef(receiverId, MEDIA_TYPE_VIDEO, dataAvaliable);
        SetReceiverDataRef(receiverId, MEDIA_TYPE_AUDIO, dataAvaliable);
    } else {
        SetReceiverDataRef(receiverId, type, dataAvaliable);
    }

    if (!dataAvaliable) {
        return;
    }

    continueNotify_ = true;
    dataCV_.notify_one();
}

void BufferDispatcher::SetDataRef(uint32_t bitref)
{
    SHARING_LOGD("trace.");
    dataBitRef_ &= bitref;
}

uint32_t BufferDispatcher::GetDataRef()
{
    SHARING_LOGD("trace.");
    return dataBitRef_;
}

void BufferDispatcher::SetReadRef(uint32_t bitref)
{
    SHARING_LOGD("trace.");
    recvBitRef_ &= bitref;
}

uint32_t BufferDispatcher::GetReadRef()
{
    SHARING_LOGD("trace.");
    return recvBitRef_;
}

void BufferDispatcher::ActiveDataRef(MediaType type, bool keyFrame)
{
    MEDIA_LOGD("trace.");
    std::unique_lock<std::mutex> locker(notifyMutex_);
    uint32_t bitRef = 0x0000;
    for (auto &[recvId, notifier] : notifiers_) {
        auto index = notifier->GetReadIndex();
        if (type == MEDIA_TYPE_AUDIO) {
            bitRef |= RECV_FLAG_BASE << (index * 2); // 2: fix offset, get audio notifyer id
            continue;
        }
        bool keyModeReceiver = false;
        keyModeReceiver = notifier->IsKeyModeReceiver();
        if (keyFrame && keyModeReceiver && keyIndexList_.empty()) {
            notifier->videoIndex = circularBuffer_.size() - 1;
        }
        if ((!keyModeReceiver || (keyModeReceiver && keyFrame))) {
            if (index != INVALID_INDEX) {
                bitRef |= RECV_FLAG_BASE << (index * 2 + 1); // 2: fix offset, get video notifyer id
            }
        }
    }

    dataBitRef_ |= bitRef;
}

void BufferDispatcher::SetReceiverDataRef(uint32_t receiverId, MediaType type, bool ready)
{
    MEDIA_LOGD("trace.");
    uint32_t index = FindReceiverIndex(receiverId);
    if (index == INVALID_INDEX) {
        return;
    }

    if (type == MEDIA_TYPE_AUDIO) {
        uint32_t audioBit = index * 2;
        uint32_t bitRef = RECV_FLAG_BASE << audioBit;
        MEDIA_LOGD("Audio recvid %{public}d ,bitRef %{public}d ready %{public}d.", receiverId, bitRef, ready);
        if (!ready) {
            bitRef = ~bitRef;
            dataBitRef_ &= bitRef;
        } else {
            dataBitRef_ |= bitRef;
        }
    } else if (type == MEDIA_TYPE_VIDEO) {
        uint32_t videoBit = index * 2 + 1;
        uint32_t bitRef = RECV_FLAG_BASE << videoBit;
        MEDIA_LOGD("Video recvid %{public}d ,bitRef %{public}d ready %{public}d.", receiverId, bitRef, ready);
        if (!ready) {
            bitRef = ~bitRef;
            dataBitRef_ &= bitRef;
        } else {
            dataBitRef_ |= bitRef;
        }
    }
}

void BufferDispatcher::SetReceiverReadRef(uint32_t receiverId, MediaType type, bool ready)
{
    MEDIA_LOGD("trace.");
    uint32_t index = FindReceiverIndex(receiverId);
    if (index == INVALID_INDEX) {
        return;
    }

    if (type == MEDIA_TYPE_AUDIO) {
        uint32_t audioBit = index * 2;
        uint32_t bitRef = RECV_FLAG_BASE << audioBit;
        MEDIA_LOGD("Audio recvid %{public}d ,bitRef %{public}d ready %{public}d.", receiverId, bitRef, ready);

        if (!ready) {
            bitRef = ~bitRef;
            recvBitRef_ &= bitRef;
        } else {
            recvBitRef_ |= bitRef;
        }
    } else if (type == MEDIA_TYPE_VIDEO) {
        uint32_t videoBit = index * 2 + 1;
        uint32_t bitRef = RECV_FLAG_BASE << videoBit;
        MEDIA_LOGD("Video recvid %{public}d ,bitRef %{public}d ready %{public}d.", receiverId, bitRef, ready);

        if (!ready) {
            bitRef = ~bitRef;
            recvBitRef_ &= bitRef;
        } else {
            recvBitRef_ |= bitRef;
        }
    }
}

uint32_t BufferDispatcher::GetReceiverDataRef(uint32_t receiverId)
{
    SHARING_LOGD("trace.");
    return 0;
}

uint32_t BufferDispatcher::GetReceiverReadRef(uint32_t receiverId)
{
    SHARING_LOGD("trace.");
    uint32_t retBitRef = GetReceiverIndexRef(receiverId);
    retBitRef &= recvBitRef_;
    return retBitRef;
}

uint32_t BufferDispatcher::GetReceiverIndexRef(uint32_t receiverId)
{
    SHARING_LOGD("trace.");
    uint32_t index = FindReceiverIndex(receiverId);
    uint32_t audioBit = index * 2;
    uint32_t videoBit = index * 2 + 1;
    uint32_t retBitRef = 0x0000;
    retBitRef |= RECV_FLAG_BASE << audioBit;
    retBitRef |= RECV_FLAG_BASE << videoBit;
    return retBitRef;
}

void BufferDispatcher::ClearReadBit(uint32_t receiverId, MediaType type)
{
    MEDIA_LOGD("trace.");
    std::lock_guard<std::mutex> indexLocker(notifyMutex_);
    if (type != MEDIA_TYPE_AV) {
        SetReceiverReadRef(receiverId, type, false);
    } else {
        SetReceiverReadRef(receiverId, MEDIA_TYPE_VIDEO, false);
        SetReceiverReadRef(receiverId, MEDIA_TYPE_AUDIO, false);
    }
}

void BufferDispatcher::ClearDataBit(uint32_t receiverId, MediaType type)
{
    MEDIA_LOGD("trace.");
    std::lock_guard<std::mutex> indexLocker(notifyMutex_);
    if (type != MEDIA_TYPE_AV) {
        SetReceiverDataRef(receiverId, type, false);
    } else {
        SetReceiverDataRef(receiverId, MEDIA_TYPE_VIDEO, false);
        SetReceiverDataRef(receiverId, MEDIA_TYPE_AUDIO, false);
    }
}

bool BufferDispatcher::IsRead(uint32_t receiverId, uint32_t index)
{
    MEDIA_LOGD("trace.");
    if (index >= circularBuffer_.size()) {
        return true;
    } else {
        return IsDataReaded(receiverId, circularBuffer_.at(index));
    }
}

void BufferDispatcher::ActivateReceiverIndex(uint32_t index, MediaType type)
{
    MEDIA_LOGD("trace.");
    std::unique_lock<std::mutex> lock(notifyMutex_);
    for (auto &[recvId, notifier] : notifiers_) {
        if (type == MEDIA_TYPE_VIDEO) {
            if (notifier->videoIndex == INVALID_INDEX) {
                notifier->videoIndex = index;
                SHARING_LOGD("RecvId %{public}d Activate %{public}d.", notifier->GetReceiverId(), notifier->videoIndex);
                videoNeedActivate_ = false;
            }
        } else {
            if (notifier->audioIndex == INVALID_INDEX) {
                notifier->audioIndex = index;
                SHARING_LOGD("RecvId %{public}d Activate %{public}d.", notifier->GetReceiverId(), notifier->audioIndex);
                audioNeedActivate_ = false;
            }
        }
    }
}
void BufferDispatcher::UnlockWaitingReceiverIndex(MediaType type)
{
    SHARING_LOGD("trace.");
}

void BufferDispatcher::SetReceiverReadFlag(uint32_t receiverId, DataSpec::Ptr &dataSpec)
{
    MEDIA_LOGD("trace.");
    uint32_t index = FindReceiverIndex(receiverId);
    if (index != INVALID_INDEX) {
        dataSpec->reserveFlag |= RECV_FLAG_BASE << index;
        MEDIA_LOGW("mediaType: %{public}d, pts: %{public}" PRIu64
                   ", reserveFlag: %{public}x, receiverId: %{public}d, index: %{public}d.",
                   dataSpec->mediaData->mediaType, dataSpec->mediaData->pts, dataSpec->reserveFlag.load(), receiverId,
                   index);
    }
}

void BufferDispatcher::CancelReserve()
{
    SHARING_LOGD("trace.");
    for (auto &data : circularBuffer_) {
        data->reserveFlag = 0xff;
    }
}

void BufferDispatcher::SetSpsNalu(MediaData::Ptr spsbuf)
{
    SHARING_LOGD("trace.");
    spsBuf_ = spsbuf;
}

const MediaData::Ptr BufferDispatcher::GetSPS()
{
    MEDIA_LOGD("trace.");
    return spsBuf_;
}

void BufferDispatcher::SetPpsNalu(MediaData::Ptr ppsbuf)
{
    SHARING_LOGD("trace.");
    ppsBuf_ = ppsbuf;
}

const MediaData::Ptr BufferDispatcher::GetPPS()
{
    MEDIA_LOGD("trace.");
    return ppsBuf_;
}

uint32_t BufferDispatcher::GetCurrentGop()
{
    SHARING_LOGD("trace.");
    return gop_;
}

void BufferDispatcher::OnKeyRedirect()
{
    SHARING_LOGD("trace.");
    std::unique_lock<std::mutex> lock(notifyMutex_);
    if (keyIndexList_.empty()) {
        return;
    }

    auto nextIndex = keyIndexList_.back();

    for (auto &[recvId, notifier] : notifiers_) {
        if (notifier->IsKeyRedirectReceiver()) {
            SHARING_LOGD("receiverId: %{public}u, videoIndex: %{public}d, nextIndex: %{public}d.",
                         notifier->GetReceiverId(), notifier->videoIndex, nextIndex);
            auto curIndex = notifier->videoIndex;
            notifier->videoIndex = nextIndex;
            for (auto i = curIndex; i < nextIndex; i++) {
                SetReceiverReadFlag(notifier->GetReceiverId(), circularBuffer_[i]);
            }
            if (!rapidMode_) {
                auto receiver = notifier->GetBufferReceiver();
                if (receiver != nullptr) {
                    receiver->EnableKeyRedirect(false);
                }
            }
        }
    }
}

void BufferDispatcher::EnableRapidMode(bool enable)
{
    SHARING_LOGD("trace.");
    rapidMode_ = enable;
}

} // namespace Sharing
} // namespace OHOS

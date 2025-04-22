/*
 * Copyright (c) 2025-2025 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#include "audio_avcodec_decoder.h"
#include "avcodec_codec_name.h"
#include "avcodec_errors.h"
#include "common/common_macro.h"
#include "common/media_log.h"
#include "const_def.h"

namespace OHOS {
namespace Sharing {
AudioAvCodecDecoder::AudioAvCodecDecoder()
{
    SHARING_LOGD("tracs");
}

AudioAvCodecDecoder::~AudioAvCodecDecoder()
{
    Release();
}

int32_t AudioAvCodecDecoder::Init(const AudioTrack &audioTrack)
{
    SHARING_LOGD("trace.");
    if (!InitDecoder() || !SetAudioCallback() || !SetDecoderFormat(audioTrack)) {
        return ERROR_DECODER_INIT;
    }
    return 0;
}

bool AudioAvCodecDecoder::InitDecoder()
{
    SHARING_LOGD("trace.");
    if (audioDecoder_ != nullptr) {
        SHARING_LOGE("audio decoder already init.");
        return true;
    }
    audioDecoder_ = MediaAVCodec::AudioDecoderFactory::CreateByName(
        (MediaAVCodec::AVCodecCodecName::AUDIO_DECODER_AAC_NAME).data());
    if (audioDecoder_ == nullptr) {
        SHARING_LOGE("create audio decoder failed.");
        return false;
    }
    SHARING_LOGI("init audio decoder success.");
    return true;
}

bool AudioAvCodecDecoder::SetAudioCallback()
{
    SHARING_LOGD("trace.");
    RETURN_FALSE_IF_NULL(audioDecoder_);
    auto ret = audioDecoder_->SetCallback(std::static_pointer_cast<AudioAvCodecDecoder>(shared_from_this()));
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        SHARING_LOGE("set audio decoder callback failed!");
        return false;
    }
    SHARING_LOGD("set audio decoder callback success");
    return true;
}

bool AudioAvCodecDecoder::Start()
{
    SHARING_LOGD("trace.");
    if (isRunning_) {
        SHARING_LOGW(" decoder is running.");
        return true;
    }

    auto audioDecoder = GetDecoder();
    RETURN_FALSE_IF_NULL(audioDecoder);
    auto ret = audioDecoder->Start();
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        SHARING_LOGE("start decoder failed");
        return false;
    }
    isRunning_ = true;
    return true;
}

void AudioAvCodecDecoder::Stop()
{
    SHARING_LOGD("trace.");
    if (!isRunning_) {
        SHARING_LOGE("decoder is not running");
        return;
    }
    if (StopDecoder()) {
        isRunning_ = false;
        isFirstFrame_ = false;
    }
}

void AudioAvCodecDecoder::Release()
{
    SHARING_LOGD("trace.");
    if (audioDecoder_ != nullptr) {
        audioDecoder_->Release();
        audioDecoder_->Reset();
    }
}

bool AudioAvCodecDecoder::StopDecoder()
{
    SHARING_LOGD("trace.");
    auto audioDecoder = GetDecoder();
    RETURN_FALSE_IF_NULL(audioDecoder);

    auto ret = audioDecoder->Flush();
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        SHARING_LOGE("Flush audioDecoder failed");
        return false;
    }
    ret = audioDecoder->Stop();
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        SHARING_LOGE("Stop audioDecoder failed");
        return false;
    }
    ret = audioDecoder->Reset();
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        SHARING_LOGE("Reset audioDecoder failed");
        return false;
    }
    SHARING_LOGD("StopDecoder Success.");
    return true;
}

bool AudioAvCodecDecoder::SetDecoderFormat(const AudioTrack &audioTrack)
{
    SHARING_LOGI("trace.");
    auto audioDecoder = GetDecoder();
    RETURN_FALSE_IF_NULL(audioDecoder);

    int32_t sampleRate =
        audioTrack.sampleRate == 0 ? AUDIO_DECODE_DEFAULT_SAMPLERRATE : static_cast<int32_t>(audioTrack.sampleRate);
    int32_t channelCount =
        audioTrack.channels == 0 ? AUDIO_DECODE_DEFAULT_CHANNEL_COUNT : static_cast<int32_t>(audioTrack.channels);

    MediaAVCodec::Format format;
    format.PutIntValue("sample_rate", sampleRate);
    format.PutIntValue("channel_count", channelCount);
    format.PutIntValue("audio_sample_format", static_cast<int32_t>(MediaAVCodec::AudioSampleFormat::SAMPLE_S16LE));
    int32_t ret = audioDecoder->Configure(format);
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        SHARING_LOGE("Configure decoder format failed, Error code %{public}d.", ret);
        return false;
    }
    ret = audioDecoder->Prepare();
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        SHARING_LOGE("Configure decoder prepare failed, Error code %{public}d.", ret);
        return false;
    }
    return true;
}

void AudioAvCodecDecoder::OnFrame(const Frame::Ptr &frame)
{
    SHARING_LOGD("trace.");
    auto audioDecoder = GetDecoder();
    RETURN_IF_NULL(audioDecoder);
    int32_t bufferIndex = 0;
    std::shared_ptr<MediaAVCodec::AVSharedMemory> inputBuffer;
    {
        std::unique_lock<std::mutex> lock(inputBufferMutex_);
        if (inBufferQueue_.empty()) {
            inCond_.wait_for(lock, std::chrono::milliseconds(AUDIO_DECODE_WAIT_MILLISECONDS),
                             [this]() { return (!inBufferQueue_.empty()); });
        }
        if (inBufferQueue_.empty()) {
            SHARING_LOGW("Index queue is empty.");
            return;
        }
        auto pair_data = inBufferQueue_.front();
        bufferIndex = pair_data.first;
        inputBuffer = pair_data.second;
        inBufferQueue_.pop();
    }

    if ((inputBuffer == nullptr) || frame == nullptr || (frame->Data() == nullptr)) {
        SHARING_LOGW("GetInputBuffer or GetFrameBuffer failed.");
        return;
    }
    if (memcpy_s(inputBuffer->GetBase(), inputBuffer->GetSize(), frame->Data(), frame->Size()) != EOK) {
        SHARING_LOGW("Copy data failed.");
        return;
    }
    MediaAVCodec::AVCodecBufferInfo bufferInfo = {
        .presentationTimeUs = frame->Pts(),
        .size = static_cast<int32_t>(frame->Size()),
        .offset = 0,
    };
    auto bufferFlag = MediaAVCodec::AVCODEC_BUFFER_FLAG_CODEC_DATA;
    if (isFirstFrame_) {
        bufferFlag = MediaAVCodec::AVCODEC_BUFFER_FLAG_CODEC_DATA;
        isFirstFrame_ = false;
    } else {
        bufferFlag = MediaAVCodec::AVCODEC_BUFFER_FLAG_CODEC_DATA;
    }
    int32_t ret = audioDecoder->QueueInputBuffer(bufferIndex, bufferInfo, bufferFlag);
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        SHARING_LOGE("Queue input buffer fail. Error code %{public}d.", ret);
    }
}

void AudioAvCodecDecoder::OnError(MediaAVCodec::AVCodecErrorType errorType, int32_t errorCode)
{
    SHARING_LOGE("onError, errorCode = %{public}d", errorCode);
}

void AudioAvCodecDecoder::OnInputBufferAvailable(uint32_t index, std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer)
{
    SHARING_LOGD("trace.");
    {
        std::lock_guard<std::mutex> lock(inputBufferMutex_);
        if (buffer == nullptr) {
            return;
        }
        inBufferQueue_.push({index, buffer});
    }
    inCond_.notify_all();
}

void AudioAvCodecDecoder::OnOutputBufferAvailable(uint32_t index, MediaAVCodec::AVCodecBufferInfo info,
                                                  MediaAVCodec::AVCodecBufferFlag flag,
                                                  std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer)
{
    SHARING_LOGD("trace.");
    auto audioDecoder = GetDecoder();
    RETURN_IF_NULL(audioDecoder);
    if (flag & MediaAVCodec::AVCODEC_BUFFER_FLAG_EOS) {
        SHARING_LOGE("flag is eos.");
        return;
    }
    if (!buffer) {
        SHARING_LOGE("buffer is null");
        return;
    }
    if (info.size <= 0 || info.size > buffer->GetSize()) {
        SHARING_LOGE("Codec output info error, AVCodec info: size %{public}d, memory size %{public}d.", info.size,
                     buffer->GetSize());
        return;
    }
    auto frameBuffer = FrameImpl::Create();
    frameBuffer->SetSize(static_cast<uint32_t>(info.size));
    frameBuffer->codecId_ = CODEC_AAC;
    frameBuffer->pts_ = static_cast<uint32_t>(info.presentationTimeUs);
    frameBuffer->Assign((char *)buffer->GetBase(), info.size);
    DeliverFrame(frameBuffer);
    int32_t ret = audioDecoder->ReleaseOutputBuffer(index);
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        SHARING_LOGE("ReleaseOutputBuffer fail. Error code %{public}d.", ret);
    }
}

void AudioAvCodecDecoder::OnOutputFormatChanged(const MediaAVCodec::Format &format)
{
    SHARING_LOGD("trace.");
}

std::shared_ptr<MediaAVCodec::AVCodecAudioDecoder> AudioAvCodecDecoder::GetDecoder()
{
    std::lock_guard<std::mutex> lock(decoderMutex_);
    return audioDecoder_;
}
} // namespace Sharing
} // namespace OHOS
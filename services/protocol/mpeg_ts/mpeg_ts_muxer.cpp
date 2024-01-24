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

#include "mpeg_ts_muxer.h"
#include "common/media_log.h"
#include "mpeg-muxer.h"
#include "mpeg-ts-proto.h"
namespace OHOS {
namespace Sharing {
static constexpr size_t MAX_BUFFER_LEN = 1500;
MpegTsMuxer::MpegTsMuxer()
{
    static mpeg_muxer_func_t func;
    func.alloc = [](void *param, size_t bytes) {
        MpegTsMuxer *tsMuxer = (MpegTsMuxer *)param;
        if (tsMuxer) {
            // flush
            if (tsMuxer->curPos_ + bytes > MAX_BUFFER_LEN) {
                tsMuxer->OnTSPacket();
            }
            return (void*)(tsMuxer->buffer_->Data() + tsMuxer->curPos_);
        }
        return (void*)nullptr;
    };
    func.free = [](void *param, void *packet) {
        // Nothing to do, use the only buffer.
    };
    func.write = [](void *param, int stream, void *packet, size_t bytes) {
        MpegTsMuxer *tsMuxer = (MpegTsMuxer *)param;
        if (!tsMuxer) {
            return 0;
        }
        tsMuxer->curPos_ += bytes;
        return 0;
    };

    // ts muxer
    muxerCtx_ = mpeg_muxer_create(false, &func, this);
    buffer_ = std::make_shared<DataBuffer>(MAX_BUFFER_LEN);
}

MpegTsMuxer::~MpegTsMuxer()
{
    if (muxerCtx_) {
        mpeg_muxer_destroy(muxerCtx_);
        muxerCtx_ = nullptr;
    }
}

void MpegTsMuxer::InputFrame(const Frame::Ptr &frame)
{
    SHARING_LOGD("trace.");
    auto iter = codecTracks_.find(frame->GetCodecId());
    bool addStream = false;
    if (iter == codecTracks_.end()) {
        addStream = true;
    }
    EPSI_STREAM_TYPE streamType = PSI_STREAM_RESERVED;
    switch (frame->GetCodecId()) {
        case CODEC_H264:
        case CODEC_H265: {
            if (frame->GetCodecId() == CODEC_H264) {
                streamType = PSI_STREAM_H264;
            } else {
                streamType = PSI_STREAM_H265;
            }

            _timestamp = frame->Dts();
            keyFrame_ = frame->ConfigFrame();
            MEDIA_LOGD(
                "_timestamp = %{public}d, keyFrame_  %{public}d, frame->Dts() %{public}d, frame->pts() %{public}d",
                _timestamp, keyFrame_, frame->Dts(), frame->Pts());
            break;
        }
        case CODEC_AAC:
            streamType = PSI_STREAM_AAC;
            break;
        default:
            MEDIA_LOGW("Unknown codec: %d", frame->GetCodecId());
            break;
    }
    if (streamType == PSI_STREAM_RESERVED) {
        MEDIA_LOGW("streamType: %d", streamType);
        return;
    }

    if (addStream) {
        codecTracks_[frame->GetCodecId()] =
            mpeg_muxer_add_stream(muxerCtx_, streamType, 0, 0); // extradata is null for H264,AAC
    }
    mpeg_muxer_input(muxerCtx_, codecTracks_[frame->GetCodecId()], keyFrame_ ? MPEG_FLAG_IDR_FRAME : 0,
                     frame->Pts() * 90, frame->Dts() * 90, frame->Peek(), frame->Size()); // 90 : timestamp in 90KHz.
    OnTSPacket();
}

void MpegTsMuxer::SetOnMux(const OnMux &cb)
{
    onMux_ = cb;
}

void MpegTsMuxer::OnTSPacket()
{
    if (onMux_ && curPos_ != 0) {
        buffer_->UpdateSize(curPos_);
        MEDIA_LOGD("_timestamp = %{public}d, bufferSize %{public}d", _timestamp, buffer_->Size());
        onMux_(buffer_, _timestamp, keyFrame_);
        buffer_->UpdateSize(0);
        curPos_ = 0;
    }
    keyFrame_ = false;
}
} // namespace Sharing
} // namespace OHOS

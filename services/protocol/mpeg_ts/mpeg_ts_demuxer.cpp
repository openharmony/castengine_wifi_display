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

#include "mpeg_ts_demuxer.h"
#include "common/media_log.h"

namespace OHOS {
namespace Sharing {

MpegTsDemuxer::MpegTsDemuxer()
{
    splitter_.setOnSplit([this](const char *data, size_t len) { ts_demuxer_input(demuxerCtx_, (uint8_t *)data, len); });
    demuxerCtx_ = ts_demuxer_create(
        [](void *param, int32_t program, int32_t stream, int32_t codecId, int32_t flags, int64_t pts, int64_t dts,
           const void *data, size_t bytes) {
            MpegTsDemuxer *self = (MpegTsDemuxer *)param;
            if (self && self->onDecode_) {
                if (flags & MPEG_FLAG_PACKET_CORRUPT) {
                    MEDIA_LOGD("g_FLAG_PACKET_CORRUPT.");
                } else {
                    self->onDecode_(stream, codecId, flags, pts, dts, data, bytes);
                }
            }

            return 0;
        },
        this);

    ts_demuxer_notify_t notify = {
        [](void *param, int32_t stream, int32_t codecId, const void *extra, int32_t bytes, int32_t finish) {
            MpegTsDemuxer *self = (MpegTsDemuxer *)param;
            if (self && self->onStream_) {
                self->onStream_(stream, codecId, extra, bytes, finish);
            }
        }};
    ts_demuxer_set_notify((struct ts_demuxer_t *)demuxerCtx_, &notify, this);
}

MpegTsDemuxer::~MpegTsDemuxer()
{
    ts_demuxer_destroy(demuxerCtx_);
}

size_t MpegTsDemuxer::Input(const uint8_t *data, size_t len)
{
    if (MpegTsSplitter::IsTSPacket((char *)data, len)) {
        return ts_demuxer_input(demuxerCtx_, data, len);
    }
    splitter_.Input((char *)data, len);
    return 0;
}

void MpegTsDemuxer::SetOnDecode(const OnDecode &cb)
{
    onDecode_ = std::move(cb);
}

void MpegTsDemuxer::SetOnStream(const OnStream &cb)
{
    onStream_ = std::move(cb);
}
} // namespace Sharing
} // namespace OHOS
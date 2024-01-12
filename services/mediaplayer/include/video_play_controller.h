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

#ifndef OHOS_SHARING_VIDEO_PLAY_CONTROLLER_H
#define OHOS_SHARING_VIDEO_PLAY_CONTROLLER_H

#include <thread>
#include "buffer_dispatcher.h"
#include "codec/include/video_sink_decoder.h"
#include "common/event_comm.h"
#include "common/sharing_log.h"

namespace OHOS {
namespace Sharing {

class MediaController;

class VideoPlayController : public VideoSinkDecoderListener,
                            public IBufferReceiverListener,
                            public std::enable_shared_from_this<VideoPlayController> {
public:
    explicit VideoPlayController(uint32_t mediaChannelId);
    ~VideoPlayController();

    void SetKeyMode(bool mode)
    {
        SHARING_LOGD("trace.");
        isKeyMode_ = mode;
        if (bufferReceiver_ && isVideoRunning_) {
            bufferReceiver_->EnableKeyMode(mode);
        }
    }

    void SetKeyRedirect(bool keyRedirect)
    {
        SHARING_LOGD("trace.");
        if (bufferReceiver_) {
            bufferReceiver_->EnableKeyRedirect(keyRedirect);
        }
    }

    void SetMediaController(std::shared_ptr<MediaController> mediaController)
    {
        SHARING_LOGD("trace.");
        mediaController_ = mediaController;
    }

public:
    void Release();
    void Stop(BufferDispatcher::Ptr &dispatcher);

    bool Init(VideoTrack &videoTrack);
    bool Start(BufferDispatcher::Ptr &dispatcher);
    bool SetSurface(sptr<Surface> surface, bool keyFrame = false);

    // impl IBufferReceiverListener
    void OnAccelerationDoneNotify() override;
    void OnKeyModeNotify(bool enable) override;

protected:
    void OnError(int32_t errorCode) final;
    void OnVideoDataDecoded(DataBuffer::Ptr decodedData) final;

private:
    void StopVideoThread();
    void VideoPlayThread();
    void StartVideoThread();
    void ProcessVideoData(const char *data, int32_t size);
    int32_t RenderInCopyMode(const DataBuffer::Ptr decodedData);

private:
    bool firstFrame_ = true;
    bool enableSurface_ = false;
    bool forceSWDecoder_ = false;
    bool isSurfaceNoCopy_ = false;
    uint32_t mediachannelId_ = 0;
    sptr<Surface> surface_ = nullptr;

    std::atomic_bool isKeyMode_ = false;
    std::atomic_bool isVideoRunning_ = false;
    std::weak_ptr<MediaController> mediaController_;
    std::shared_ptr<std::thread> videoPlayThread_ = nullptr;
    std::shared_ptr<BufferReceiver> bufferReceiver_ = nullptr;
    std::shared_ptr<VideoSinkDecoder> videoSinkDecoder_ = nullptr;

    VideoTrack videoTrack_;
};

} // namespace Sharing
} // namespace OHOS
#endif
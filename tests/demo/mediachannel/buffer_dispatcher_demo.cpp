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

#include <chrono>
#include <fstream>
#include <thread>
#include <unordered_map>
#include "common/sharing_log.h"
#include "const_def.h"
#include "event_comm.h"
#include "windowmgr/include/window_manager.h"
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswresample/swresample.h"
}
#include "audio_sink.h"
#include "mediachannel/buffer_dispatcher.h"
#include "stdlib.h"
#include "video_sink_decoder.h"
#include "surface_utils.h"
namespace OHOS {
namespace Sharing {
int gType = 0;
int gPort = 30;
uint32_t receiverSize = 3;
static const char testblock[200]{0};
std::vector<std::pair<int32_t, int32_t>> position{
    {0, 0},   {480, 0},   {960, 0},   {1440, 0},   {0, 270}, {480, 270}, {960, 270}, {1440, 270},
    {0, 540}, {480, 540}, {960, 540}, {1440, 540}, {0, 810}, {480, 810}, {960, 810}, {1440, 810}};
#define AUDIO_INBUF_SIZE 192000
static int alloc_and_copy(AVPacket *outPacket, const uint8_t *spspps, uint32_t spsppsSize, const uint8_t *in,
                          uint32_t size)
{
    int err;
    int startCodeLen = 3;

    err = av_grow_packet(outPacket, spsppsSize + size + startCodeLen);
    if (err < 0) {
        return err;
    }
    if (spspps) {
        memcpy(outPacket->data, spspps, spsppsSize);
    }

    (outPacket->data + spsppsSize)[0] = 0;
    (outPacket->data + spsppsSize)[1] = 0;
    (outPacket->data + spsppsSize)[2] = 1;
    memcpy(outPacket->data + spsppsSize + startCodeLen, in, size);
    return 0;
}

static int h264_extradata_to_annexb(const unsigned char *extraData, const int extraDataSize, AVPacket *outPacket,
                                    int padding)
{
    const unsigned char *pExtraData = nullptr;
    int len = 0;
    int spsUnitNum, ppsUnitNum;
    int unitSize, totalSize = 0;
    unsigned char startCode[] = {0, 0, 0, 1};
    unsigned char *pOut = nullptr;
    int err;

    pExtraData = extraData + 4;
    len = (*pExtraData++ & 0x3) + 1;

    // sps
    spsUnitNum = (*pExtraData++ & 0x1f);
    while (spsUnitNum--) {
        unitSize = (pExtraData[0] << 8 | pExtraData[1]);
        pExtraData += 2;
        totalSize += unitSize + sizeof(startCode);

        if (totalSize > INT_MAX - padding) {
            av_free(pOut);
            return AVERROR(EINVAL);
        }
        if (pExtraData + unitSize > extraData + extraDataSize) {
            av_free(pOut);
            return AVERROR(EINVAL);
        }
        if ((err = av_reallocp(&pOut, totalSize + padding)) < 0) {
            return err;
        }
        memcpy(pOut + totalSize - unitSize - sizeof(startCode), startCode, sizeof(startCode));
        memcpy(pOut + totalSize - unitSize, pExtraData, unitSize);

        pExtraData += unitSize;
    }

    // pps
    ppsUnitNum = (*pExtraData++ & 0x1f);
    while (ppsUnitNum--) {
        unitSize = (pExtraData[0] << 8 | pExtraData[1]);
        pExtraData += 2;
        totalSize += unitSize + sizeof(startCode);

        if (totalSize > INT_MAX - padding) {
            av_free(pOut);
            return AVERROR(EINVAL);
        }
        if (pExtraData + unitSize > extraData + extraDataSize) {
            av_free(pOut);
            return AVERROR(EINVAL);
        }
        if ((err = av_reallocp(&pOut, totalSize + padding)) < 0) {
            return err;
        }
        memcpy(pOut + totalSize - unitSize - sizeof(startCode), startCode, sizeof(startCode));
        memcpy(pOut + totalSize - unitSize, pExtraData, unitSize);

        pExtraData += unitSize;
    }
    outPacket->data = pOut;
    outPacket->size = totalSize;

    return len;
}

class BufferController {
public:
    using Ptr = std::shared_ptr<BufferController>;
    BufferController(/* args */)
    {
        SHARING_LOGI("BufferController ctor");
        dispatcher_ = std::make_shared<BufferDispatcher>();
    }
    ~BufferController()
    {
        SHARING_LOGI("BufferController dtor");
        if (dispatcher_ != nullptr) {
            dispatcher_->FlushBuffer();
            dispatcher_->ReleaseAllReceiver();
            dispatcher_.reset();
        }
    }
    inline void StartWriteSeqData(bool mode = true)
    {
        isRunning_ = true;
        if (mode) {
            dataWriteTH_ = std::thread(&BufferController::WriteData, this);
        } else {
            dataWriteTH_ = std::thread(&BufferController::WriteDummyData, this);
        }
    }
    inline void StopWriteSeqData()
    {
        isRunning_ = false;
        if (dataWriteTH_.joinable()) {
            dataWriteTH_.join();
        }
    }
    inline void SetGopSize(uint32_t size)
    {
        gopSize_ = size;
    }
    inline void SetDummyDataSize(uint32_t size)
    {
        dummyCount_ = size;
    }
    inline bool IsRunning()
    {
        return isRunning_;
    }

    inline void AppendDemoData(int mode = 0)
    {
        auto mediaData = std::make_shared<MediaData>();
        mediaData->buff = std::make_shared<DataBuffer>();
        std::string fakeData;
        if (mode == 0) {
            mediaData->mediaType = MEDIA_TYPE_AV;
            mediaData->pts = ts_;
            mediaData->keyFrame = true;
            fakeData = "av-append-key" + std::to_string(ts_);
        } else if (mode == 1) {
            mediaData->mediaType = MEDIA_TYPE_AV;
            mediaData->pts = ts_;
            mediaData->keyFrame = false;
            fakeData = "av-append" + std::to_string(ts_);
        } else if (mode == 2) {
            mediaData->mediaType = MEDIA_TYPE_VIDEO;
            mediaData->pts = ts_;
            mediaData->keyFrame = true;
            fakeData = "v-append-key" + std::to_string(ts_);
        } else if (mode == 3) {
            mediaData->mediaType = MEDIA_TYPE_VIDEO;
            mediaData->pts = ts_;
            mediaData->keyFrame = false;
            fakeData = "v-append" + std::to_string(ts_);
        } else if (mode == 4) {
            mediaData->mediaType = MEDIA_TYPE_AUDIO;
            mediaData->pts = ts_;
            fakeData = "a-append" + std::to_string(ts_);
        }
        SHARING_LOGD("generate append data: %{public}s", fakeData.c_str());
        fakeData += realSimuData;
        mediaData->buff->ReplaceData(fakeData.c_str(), fakeData.size());
        dummyDatas_.push_back(mediaData);
        ++dummyCount_;
        ++ts_;
    }

    inline void GenerateAVDemoData(bool firstKey = true)
    {
        // av
        dummyDatas_.clear();
        dummyDatas_.resize(0);
        uint32_t count = dummyCount_;
        uint32_t videoCount = 0;
        for (size_t i = 0; i < count; i++) {
            auto mediaData = std::make_shared<MediaData>();
            mediaData->buff = std::make_shared<DataBuffer>();
            std::string fakeData;
            if (i % 3 == 0) {
                mediaData->mediaType = MEDIA_TYPE_VIDEO;
                mediaData->pts = ts_;
                fakeData = "v-" + std::to_string(ts_) + realSimuData;
                videoCount++;
                if (videoCount >= gopSize_ && videoCount % gopSize_ == 0) {
                    mediaData->keyFrame = true;
                } else {
                    mediaData->keyFrame = false;
                }
            } else {
                mediaData->mediaType = MEDIA_TYPE_AUDIO;
                mediaData->pts = ts_;
                fakeData = "a-" + std::to_string(ts_) + realSimuData;
            }
            mediaData->buff->ReplaceData(fakeData.c_str(), fakeData.size());
            dummyDatas_.push_back(mediaData);
            ++ts_;
        }
        if (firstKey) {
            dummyDatas_[0]->keyFrame = firstKey;
        }
    }

    inline void GenerateAudioDemoData()
    {
        uint32_t count = dummyCount_;
        // audio only
        for (size_t i = 0; i < count; ++i) {
            auto mediaData = std::make_shared<MediaData>();
            mediaData->buff = std::make_shared<DataBuffer>();
            mediaData->pts = ts_;
            std::string fakeData;
            mediaData->mediaType = MEDIA_TYPE_AUDIO;
            fakeData = "a-" + std::to_string(ts_) + realSimuData;
            mediaData->buff->ReplaceData(fakeData.c_str(), fakeData.size());
            dummyDatas_.push_back(mediaData);
            ++ts_;
        }
    }

    inline void GenerateVideoDemoData(bool firstKey = true)
    {
        // video only
        uint32_t count = dummyCount_;
        uint32_t videoCount = 0;
        for (size_t i = 0; i < count; ++i) {
            auto mediaData = std::make_shared<MediaData>();
            mediaData->buff = std::make_shared<DataBuffer>();
            std::string fakeData;
            mediaData->mediaType = MEDIA_TYPE_VIDEO;
            mediaData->pts = ts_;
            ++videoCount;
            if (videoCount >= gopSize_ && videoCount % gopSize_ == 0) {
                mediaData->keyFrame = true;
            }
            fakeData = "v-" + std::to_string(ts_) + realSimuData;
            mediaData->buff->ReplaceData(fakeData.c_str(), fakeData.size());
            dummyDatas_.push_back(mediaData);
            ++ts_;
        }
        if (firstKey) {
            dummyDatas_[0]->keyFrame = firstKey;
        }
    }
    uint32_t ReturnTS()
    {
        return ts_;
    }
    inline std::shared_ptr<BufferDispatcher> GetDispatcher()
    {
        return dispatcher_;
    }
    inline void WriteGop()
    {
        bool waitKeyFlag = true;
        for (; writeIndex_ < dummyDatas_.size(); writeIndex_++) {
            if (waitKeyFlag) {
                if (!dummyDatas_[writeIndex_]->keyFrame) {
                    continue;
                } else {
                    waitKeyFlag = false;
                }
            } else {
                if (dummyDatas_[writeIndex_]->keyFrame) {
                    break;
                }
            }
            dispatcher_->InputData(dummyDatas_[writeIndex_]);
        }
        SHARING_LOGI("After WriteGop BufferQueue size : %{public}d", dispatcher_->GetBufferSize());
    }
    inline void SetWriteIndex(uint32_t index)
    {
        writeIndex_ = index;
    }

    inline void SetDummyCount(int count)
    {
        dummyCount_ = count;
    }

    inline void SetGopSize(int size)
    {
        gopSize_ = size;
    }

private:
    void WriteData()
    {
        while (isRunning_) {
            AVFormatContext *inputContext = avformat_alloc_context();
            std::string inputFile = "/data/360p.mp4";
            AVPacket packet;
            AVCodec *codec = nullptr;
            AVCodecContext *codecContext = nullptr;
            struct SwrContext *convertContext = nullptr;
            AVSampleFormat in_sample_fmt;
            AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
            int in_sample_rate;
            int out_sample_rate = 48000;
            uint64_t in_channel_layout;
            uint64_t out_channel_layout = AV_CH_LAYOUT_MONO;
            int out_channel_nb = 0;
            uint8_t *buffer = nullptr;
            int video_stream = -1;
            int audio_stream = -1;
            SHARING_LOGD("write file in");
            int ret = 0;
            if ((ret = avformat_open_input(&inputContext, inputFile.c_str(), nullptr, nullptr)) != 0) {
                SHARING_LOGE("ffmpeg open file failed, %{public}s", av_err2str(ret));
                break;
            }
            ret = avformat_find_stream_info(inputContext, nullptr);

            video_stream = av_find_best_stream(inputContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
            audio_stream = av_find_best_stream(inputContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
            if (video_stream < 0 && audio_stream < 0) {
                SHARING_LOGE("find video && audio stream error");
                avformat_close_input(&inputContext);
                break;
            }
            if (audio_stream >= 0) {
                av_dump_format(inputContext, audio_stream, inputFile.c_str(), 0);
                codec = avcodec_find_decoder(inputContext->streams[audio_stream]->codecpar->codec_id);
                codecContext = avcodec_alloc_context3(codec);
                avcodec_parameters_to_context(codecContext, inputContext->streams[audio_stream]->codecpar);
                if (avcodec_open2(codecContext, codec, nullptr) < 0) {
                    SHARING_LOGE("open codec error");
                    avformat_close_input(&inputContext);
                    break;
                }
                convertContext = swr_alloc();
                in_sample_fmt = codecContext->sample_fmt;
                in_sample_rate = codecContext->sample_rate;
                in_channel_layout = codecContext->channel_layout;
                convertContext = swr_alloc_set_opts(convertContext, out_channel_layout, out_sample_fmt, out_sample_rate,
                                                    in_channel_layout, in_sample_fmt, in_sample_rate, 0, nullptr);
                swr_init(convertContext);
                out_channel_nb = av_get_channel_layout_nb_channels(out_channel_layout);
                buffer = (uint8_t *)av_malloc(2 * 48000);
            }
            if (video_stream >= 0) {
                av_dump_format(inputContext, video_stream, inputFile.c_str(), 0);
            }
            double vfps = 0.0;
            if (inputContext->streams[video_stream]->avg_frame_rate.den == 0 ||
                (inputContext->streams[video_stream]->avg_frame_rate.num == 0 &&
                 inputContext->streams[video_stream]->avg_frame_rate.den == 1)) {
                vfps = inputContext->streams[video_stream]->r_frame_rate.num /
                       inputContext->streams[video_stream]->r_frame_rate.den;
            } else {
                vfps = inputContext->streams[video_stream]->avg_frame_rate.num /
                       inputContext->streams[video_stream]->avg_frame_rate.den;
            }
            double afps = (48000 * 2 * 16 / 8) / (1024 * 2 * 16 / 8.0); // aac
            SHARING_LOGD("vfpr: %{public}lf, afps: %{public}lf", vfps, afps);
            av_init_packet(&packet);
            packet.data = nullptr;
            packet.size = 0;
            bool reading = true;
            int32_t keyCount = 0;
            SHARING_LOGD("write file start");
            while (reading) {
                static std::chrono::time_point timeStart = std::chrono::system_clock::now();
                int error = av_read_frame(inputContext, &packet);
                if (error == AVERROR_EOF) {
                    reading = false;
                    break;
                } else if (error < 0) {
                    break;
                }
                if (audio_stream == packet.stream_index) {
                } else if (video_stream == packet.stream_index) {
                    unsigned char *pData = packet.data; // frame data
                    unsigned char *pEnd = nullptr;
                    int dataSize = packet.size;
                    int curSize = 0;
                    unsigned char nalHeader, nalType;
                    AVPacket *outPacket = av_packet_alloc();
                    pEnd = pData + dataSize;
                    SHARING_LOGD("packet size: %{public}d, flag: %{public}d, pts: %{public}" PRIu64 "", packet.size,
                                 packet.flags, packet.pts);
                    while (curSize < dataSize) {
                        AVPacket spsppsPacket;
                        spsppsPacket.data = nullptr;
                        spsppsPacket.size = 0;
                        int naluSize = 0;
                        if (pEnd - pData < 4) {
                            break;
                        }
                        for (int i = 0; i < 4; ++i) {
                            naluSize <<= 8;
                            naluSize |= pData[i];
                        }
                        pData += 4;
                        curSize += 4;
                        if (naluSize > (pEnd - pData + 1) || naluSize <= 0) {
                            SHARING_LOGD("nalusize error, real size: %{public}d, naluSize: %{public}d",
                                         pEnd - pData + 1, naluSize);
                            break;
                        }
                        nalHeader = *pData;
                        nalType = nalHeader & 0x1F;
                        SHARING_LOGD("nalType: %{public}u, flag: %{public}d", nalType, packet.flags);
                        if (nalType == 6) {
                            pData += naluSize;
                            curSize += naluSize;
                            continue;
                        } else if (nalType == 5) {
                            // key frame
                            h264_extradata_to_annexb(
                                inputContext->streams[packet.stream_index]->codecpar->extradata,
                                inputContext->streams[packet.stream_index]->codecpar->extradata_size, &spsppsPacket,
                                AV_INPUT_BUFFER_PADDING_SIZE);
                            if (alloc_and_copy(outPacket, spsppsPacket.data, spsppsPacket.size, pData, naluSize) < 0) {
                                av_packet_free(&outPacket);
                                if (spsppsPacket.data) {
                                    free(spsppsPacket.data);
                                    spsppsPacket.data = nullptr;
                                }
                                break;
                            }
                        } else {
                            if (alloc_and_copy(outPacket, nullptr, 0, pData, naluSize) < 0) {
                                av_packet_free(&outPacket);
                                if (spsppsPacket.data) {
                                    free(spsppsPacket.data);
                                    spsppsPacket.data = nullptr;
                                }
                                break;
                            }
                        }
                        if (spsppsPacket.data) {
                            free(spsppsPacket.data);
                            spsppsPacket.data = nullptr;
                        }
                        curSize += naluSize;
                        SHARING_LOGD("goto next, diffsize: %{public}d", packet.size - outPacket->size);
                    }
                    auto inputData = dispatcher_->RequestDataBuffer(MEDIA_TYPE_VIDEO, outPacket->size);
                    inputData->buff->ReplaceData((const char *)(outPacket->data), outPacket->size);
                    inputData->keyFrame = ((nalType == 5) ? true : false);
                    inputData->mediaType = MEDIA_TYPE_VIDEO;
                    inputData->pts = packet.pts;
                    if (inputData->keyFrame) {
                        ++keyCount;
                        SHARING_LOGD("write key frame: %{public}d, pts: %{public}" PRIu64 "", keyCount, inputData->pts);
                    }
                    dispatcher_->InputData(inputData);
                    ++writeVcount_;
                    av_packet_unref(outPacket);
                    auto timeStamp =
                        timeStart + std::chrono::microseconds(1000000 * 1000 / int(vfps * 1000) * writeVcount_);
                    std::this_thread::sleep_until(timeStamp);
                }

                av_packet_unref(&packet);
            }
            SHARING_LOGD("write file done, write audio: %{public}u, write video: %{public}u, keyCount: %{public}d",
                         writeAcount_, writeVcount_, keyCount);
            if (buffer != nullptr) {
                free(buffer);
            }
            if (codecContext != nullptr) {
                avcodec_close(codecContext);
            }
            avformat_close_input(&inputContext);
            break;
        }
    }

    void WriteDummyData()
    {
        while (isRunning_) {
            for (; writeIndex_ < dummyCount_;) {
                if (dummyDatas_[writeIndex_] != nullptr && dummyDatas_[writeIndex_]->buff != nullptr) {
                    auto inputData = dispatcher_->RequestDataBuffer(dummyDatas_[writeIndex_]->mediaType,
                                                                    dummyDatas_[writeIndex_]->buff->Size());
                    inputData->buff->ReplaceData(dummyDatas_[writeIndex_]->buff->Peek(),
                                                 dummyDatas_[writeIndex_]->buff->Size());
                    inputData->keyFrame = dummyDatas_[writeIndex_]->keyFrame;
                    inputData->isRaw = dummyDatas_[writeIndex_]->isRaw;
                    inputData->mediaType = dummyDatas_[writeIndex_]->mediaType;
                    inputData->pts = dummyDatas_[writeIndex_]->pts;
                    inputData->ssrc = dummyDatas_[writeIndex_]->ssrc;
                    dispatcher_->InputData(inputData);
                } else {
                    SHARING_LOGE("WriteDummyData nullptr ");
                }

                writeIndex_++;
                usleep(25 * 1000);
            }
            isRunning_ = false;
        }
    }
    /* data */
    std::shared_ptr<BufferDispatcher> dispatcher_ = nullptr;
    std::unique_ptr<std::ifstream> file_ = nullptr;
    std::vector<MediaData::Ptr> dummyDatas_;
    uint32_t gopSize_ = 30;
    static uint32_t ts_;
    static std::string realSimuData;
    uint32_t dummyCount_ = 0;
    uint32_t writeIndex_ = 0;
    uint32_t writeAcount_ = 0;
    uint32_t writeVcount_ = 0;
    std::thread dataWriteTH_;
    bool isRunning_ = false;
};
uint32_t BufferController::ts_ = 0;
std::string BufferController::realSimuData = std::string(370, 'c');
enum ReadType { DUMMY, READ_TYPE_AUDIO, READ_TYPE_VIDEO, READ_TYPE_AV };
class BufferRecevierImpl : public VideoSinkDecoderListener,
                           public BufferReceiver,
                           public std::enable_shared_from_this<BufferRecevierImpl> {
public:
    using Ptr = std::shared_ptr<BufferRecevierImpl>;
    BufferRecevierImpl()
    {
        SHARING_LOGD("BufferRecevierImpl");
    }
    virtual ~BufferRecevierImpl()
    {
        if (videoSinkDecoder_) {
            videoSinkDecoder_->Release();
            videoSinkDecoder_ = nullptr;
        }
        if (audioSink_) {
            audioSink_->Release();
            audioSink_ = nullptr;
        }
        SHARING_LOGD("~BufferRecevierImpl");
    }
    void Release()
    {
        if (videoSinkDecoder_) {
            videoSinkDecoder_->Release();
            videoSinkDecoder_ = nullptr;
        }
        if (audioSink_) {
            audioSink_->Release();
            audioSink_ = nullptr;
        }
    }
    void Init(ReadType type = DUMMY)
    {
        readType_ = type;
        SHARING_LOGD("init in");
        if (type == READ_TYPE_AUDIO || type == READ_TYPE_AV) {
            audioSink_ = std::make_shared<AudioSink>(0);
            if (audioSink_->Prepare(1, 48000) != 0) {
                SHARING_LOGE("audio player prepare failed.");
            }
            audioSink_->SetVolume(0.3f);
        }

        if (type == READ_TYPE_VIDEO || type == READ_TYPE_AV) {
            videoSinkDecoder_ = std::make_shared<VideoSinkDecoder>(GetReceiverId());
            if (!videoSinkDecoder_) {
                return;
            }
            SHARING_LOGD("init in2");
            VideoTrack videoTrack;
            videoTrack.codecId = CodecId::CODEC_H264;
            videoTrack.height = 270;
            videoTrack.width = 480;
            videoTrack.frameRate = 30;
            if (!videoSinkDecoder_->Init(CodecId::CODEC_H264) && !videoSinkDecoder_->SetDecoderFormat(videoTrack)) {
                SHARING_LOGE("video sink init error");
            }
            SHARING_LOGD("init in3");
            WindowProperty::Ptr windowProperty = std::make_shared<WindowProperty>();
            windowProperty->height = 270;
            windowProperty->width = 480;
            windowProperty->startX = position[winNum_].first;
            windowProperty->startY = position[winNum_].second;
            ++winNum_;
            windowProperty->isFull = false;
            windowProperty->isShow = true;
            windowId_ = WindowManager::GetInstance().CreateWindow(windowProperty);
            sptr<Surface> surface = WindowManager::GetInstance().GetSurface(windowId_);
            // surfaceId = surface->GetUniqueId();
            // int ret = SurfaceUtils::GetInstance()->Add(surfaceId, surface);
            if (videoSinkDecoder_) {
                videoSinkDecoder_->SetSurface(surface);
            }
        }
        SHARING_LOGD("init out");
    }

    void OnVideoDataDecoded(DataBuffer::Ptr decodedData)
    {
        SHARING_LOGD("OnVideoDataDecoded");
        if (windowId_ == -1) {
            SHARING_LOGE("windowId is invalid");
            return;
        }
        Resolution rect;
        rect.width = DEFAULT_VIDEO_WIDTH;
        rect.height = DEFAULT_VIDEO_HEIGHT;
        if (decodedData->Peek() && decodedData->Size() > 0) {
            WindowManager::GetInstance().Flush(windowId_, (uint8_t *)decodedData->Peek(), decodedData->Size(), rect);
        }
    }

    inline void StartReadSeqData(ReadType readType = READ_TYPE_AV, MediaType type = MEDIA_TYPE_AV, bool mode = false)
    {
        SHARING_LOGD("StartReadSeqData");
        if (videoSinkDecoder_) {
            videoSinkDecoder_->SetVideoDecoderListener(shared_from_this());
        }
        isRunning_ = true;
        if (readType == READ_TYPE_AV) {
            if (videoSinkDecoder_) {
                if (!videoSinkDecoder_->Start()) {
                    SHARING_LOGE("video sink decoder start error");
                    return;
                }
            } else {
                SHARING_LOGE("video sink decoder is nullptr");
            }
            if (audioSink_) {
                if (audioSink_->Start() != 0) {
                    SHARING_LOGE("audio sink start error");
                    return;
                }
            } else {
                SHARING_LOGE("audio sink is nullptr");
            }

            if (mode) {
                dataReadTH_ = std::thread(&BufferRecevierImpl::ReadData, this, MEDIA_TYPE_VIDEO);
                dataReadTH2_ = std::thread(&BufferRecevierImpl::ReadData, this, MEDIA_TYPE_AUDIO);
            } else {
                dataReadTH_ = std::thread(&BufferRecevierImpl::ReadData, this, type);
            }
        } else if (readType == READ_TYPE_AUDIO) {
            if (audioSink_) {
                if (audioSink_->Start() != 0) {
                    SHARING_LOGE("audio sink start error");
                    return;
                }
            } else {
                SHARING_LOGE("audio sink is nullptr");
            }
            dataReadTH_ = std::thread(&BufferRecevierImpl::ReadData, this, type);
        } else if (readType == READ_TYPE_VIDEO) {
            if (videoSinkDecoder_) {
                if (!videoSinkDecoder_->Start()) {
                    SHARING_LOGE("video sink decoder start error");
                    return;
                }
            } else {
                SHARING_LOGE("video sink decoder is nullptr");
            }
            dataReadTH_ = std::thread(&BufferRecevierImpl::ReadData, this, type);
        } else {
            // read dummy
            if (mode) {
                dataReadTH_ = std::thread(&BufferRecevierImpl::ReadDummyData, this, MEDIA_TYPE_VIDEO);
                dataReadTH2_ = std::thread(&BufferRecevierImpl::ReadDummyData, this, MEDIA_TYPE_AUDIO);
            } else {
                dataReadTH_ = std::thread(&BufferRecevierImpl::ReadDummyData, this, type);
            }
        }
    }
    inline void StopReadSeqData(bool mode = false)
    {
        isRunning_ = false;
        NotifyReadStop();
        SHARING_LOGD("stop th1");
        if (dataReadTH_.joinable()) {
            dataReadTH_.join();
        }
        SHARING_LOGD("stop th2");
        if (mode && dataReadTH2_.joinable()) {
            dataReadTH2_.join();
        }
        SHARING_LOGD("stop avcodec");
        if (videoSinkDecoder_) {
            videoSinkDecoder_->Stop();
        }
        SHARING_LOGD("stop audio sink");
        if (audioSink_) {
            audioSink_->Stop();
        }
        SHARING_LOGD("stop read success, read count: %{public}u, receiveId: %{public}u", count_.load(),
                     GetReceiverId());
    }
    inline void SetReadInterval(uint32_t interval)
    {
        readInterval_ = interval;
    }

private:
    void ProcessVideoData(const char *data, int size)
    {
        if (data == nullptr || size <= 0 || videoSinkDecoder_ == nullptr) {
            SHARING_LOGD("ProcessVideoData null, ReceiveId: %{public}u", GetReceiverId());
            return;
        }
        int32_t bufferIndex = -1;
        {
            std::unique_lock<std::mutex> lock(videoSinkDecoder_->inMutex_);
            if (videoSinkDecoder_->inQueue_.empty()) {
                while (isRunning_) {
                    SHARING_LOGD("try wait, ReceiveId: %{public}u", GetReceiverId());
                    videoSinkDecoder_->inCond_.wait_for(lock, std::chrono::milliseconds(DECODE_WAIT_MILLISECONDS),
                                                        [this]() { return (!videoSinkDecoder_->inQueue_.empty()); });

                    if (videoSinkDecoder_->inQueue_.empty()) {
                        SHARING_LOGD("index queue empty, ReceiveId: %{public}u", GetReceiverId());
                        continue;
                    }
                    break;
                }
            }
            if (!videoSinkDecoder_->inQueue_.empty()) {
                bufferIndex = videoSinkDecoder_->inQueue_.front();
                videoSinkDecoder_->inQueue_.pop();
            }
        }

        if (!isRunning_) {
            SHARING_LOGD("stop return, ReceiveId: %{public}u", GetReceiverId());
            return;
        }
        if (bufferIndex == -1) {
            SHARING_LOGD("stop no process video data, ReceiveId: %{public}u", GetReceiverId());
        } else {
            SHARING_LOGD("ProcessVideoData DecodeVideoData in, ReceiveId: %{public}u", GetReceiverId());
            videoSinkDecoder_->DecodeVideoData(data, size, bufferIndex);
            SHARING_LOGD("ProcessVideoData DecodeVideoData out, ReceiveId: %{public}u", GetReceiverId());
        }
    }

    void ReadData(MediaType type = MEDIA_TYPE_AV)
    {
        while (isRunning_) {
            SHARING_LOGD("readtype: %{public}d, ReceiveId: %{public}u, read data in, read count: %{public}d",
                         int32_t(type), GetReceiverId(), count_.load());
            MediaData::Ptr outData = std::make_shared<MediaData>();
            outData->buff = std::make_shared<DataBuffer>();
            auto start = std::chrono::steady_clock::now();
            int32_t ret = RequestRead(type, [&outData](const MediaData::Ptr &data) {
                SHARING_LOGD("request start");
                if (data == nullptr || data->buff == nullptr) {
                    return;
                }
                outData->buff->ReplaceData(data->buff->Peek(), data->buff->Size());
                outData->keyFrame = data->keyFrame;
                outData->mediaType = data->mediaType;
                outData->pts = data->pts;
                outData->isRaw = data->isRaw;
                outData->ssrc = data->ssrc;
                SHARING_LOGD("request end");
            });
            if (ret == -1 || outData == nullptr || outData->buff == nullptr) {
                SHARING_LOGD("ReceiveId: %{public}u, request read return -1", GetReceiverId());
                ++count_;
                outData.reset();
                continue;
            }
            ++count_;
            ++readCount_;
            auto end1 = std::chrono::steady_clock::now();
            std::chrono::duration<double, std::milli> diff = end1 - start;
            SHARING_LOGD(
                "readtype: %{public}d, ReceiveId: %{public}u, read data out, readtime(before decode): %{public}lf",
                int32_t(type), GetReceiverId(), diff.count());
            if (outData->mediaType == MEDIA_TYPE_VIDEO) {
                ++readVCount_;
                if (videoSinkDecoder_ && outData && outData->buff) {
                    SHARING_LOGD("decode video receiverId: %{public}u", GetReceiverId());
                    if (outData->buff->Peek() && outData->buff->Size() > 0) {
                        if (!memcmp(testblock, outData->buff->Peek(), 200)) {
                            SHARING_LOGE("buff is all zero");
                            exit(-1);
                        }
                        ProcessVideoData(outData->buff->Peek(), outData->buff->Size());
                    }
                }
            } else if (outData->mediaType == MEDIA_TYPE_AUDIO) {
                ++readACount_;
                if (audioSink_ && outData && outData->buff) {
                    SHARING_LOGD("audio sink write");
                    size_t size = outData->buff->Size();
                    uint8_t *buf = (uint8_t *)(outData->buff->Peek());
                    if (buf && size > 0) {
                        audioSink_->Write(buf, size);
                    }
                }
            }
            auto end2 = std::chrono::steady_clock::now();
            diff = end2 - end1;
            SHARING_LOGD(
                "readtype: %{public}d, ReceiveId: %{public}u, read data out, keyFrame: %{public}s, pts: %{public}" PRIu64 ", decoded time: %{public}lf",
                int32_t(type), GetReceiverId(), outData->keyFrame ? "true" : "false", outData->pts, diff.count());
        }
        SHARING_LOGD("readtype: %{public}d, ReceiveId: %{public}u, read done, read count: %{public}u", int32_t(type),
                     GetReceiverId(),
                     type == MEDIA_TYPE_AUDIO ? readACount_.load()
                                              : (type == MEDIA_TYPE_VIDEO ? readVCount_.load() : readCount_.load()));
    }

    void ReadDummyData(MediaType type = MEDIA_TYPE_AV)
    {
        while (isRunning_) {
            SHARING_LOGD("readtype: %{public}d, ReceiveId: %{public}u, read dummy data in, read count: %{public}u",
                         int32_t(type), GetReceiverId(), count_.load());
            MediaData::Ptr outData = std::make_shared<MediaData>();
            outData->buff = std::make_shared<DataBuffer>();
            auto start = std::chrono::steady_clock::now();
            int32_t ret = RequestRead(type, [&outData](const MediaData::Ptr &data) {
                SHARING_LOGD("request start");
                if (data == nullptr || data->buff == nullptr) {
                    return;
                }
                outData->buff->ReplaceData(data->buff->Peek(), data->buff->Size());
                outData->keyFrame = data->keyFrame;
                outData->mediaType = data->mediaType;
                outData->pts = data->pts;
                outData->isRaw = data->isRaw;
                outData->ssrc = data->ssrc;
                SHARING_LOGD("request end");
            });
            if (ret == -1 || outData == nullptr || outData->buff == nullptr) {
                SHARING_LOGD("ReceiveId: %{public}u, request read return -1", GetReceiverId());
                ++count_;
                outData.reset();
                continue;
            }
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double, std::milli> diff = end - start;
            SHARING_LOGD(
                "readtype: %{public}d, ReceiveId: %{public}u, read dummy data out, keyFrame: %{public}s, pts: %{public}" PRIu64 ", readtime(no copy): %{public}lf",
                int32_t(type), GetReceiverId(), outData->keyFrame ? "true" : "false", outData->pts, diff.count());
            ++count_;
            if (type == MEDIA_TYPE_AUDIO) {
                ++readACount_;
            } else if (type == MEDIA_TYPE_VIDEO) {
                ++readVCount_;
            } else {
                ++readCount_;
            }
        }
        SHARING_LOGD("readtype: %{public}d, ReceiveId: %{public}u, read done, read count: %{public}u", int32_t(type),
                     GetReceiverId(),
                     type == MEDIA_TYPE_AUDIO ? readACount_.load()
                                              : (type == MEDIA_TYPE_VIDEO ? readVCount_.load() : readCount_.load()));
    }
    std::atomic_bool isRunning_;
    ReadType readType_ = READ_TYPE_AV;
    std::thread dataReadTH_;
    std::thread dataReadTH2_;
    uint32_t readInterval_ = 30;
    std::atomic_int32_t count_ = 0;
    std::atomic_int32_t readCount_ = 0;
    std::atomic_int32_t readACount_ = 0;
    std::atomic_int32_t readVCount_ = 0;
    int32_t windowId_ = -1;
    std::shared_ptr<VideoSinkDecoder> videoSinkDecoder_ = nullptr;
    std::shared_ptr<AudioSink> audioSink_ = nullptr;
    static int32_t winNum_;
};
int32_t BufferRecevierImpl::winNum_ = 0;
} // namespace Sharing
} // namespace OHOS
using namespace OHOS::Sharing;

using handleFunc = void (*)(BufferController::Ptr &);
using FuncMap = std::unordered_map<int, handleFunc>;

void Test1(BufferController::Ptr &controller);
void Test2(BufferController::Ptr &controller);
void Test3(BufferController::Ptr &controller);
void Test4(BufferController::Ptr &controller);
void Test5(BufferController::Ptr &controller);
void Test6(BufferController::Ptr &controller);
void Test7(BufferController::Ptr &controller);
void Test8(BufferController::Ptr &controller);
void Test9(BufferController::Ptr &controller);
void Test10(BufferController::Ptr &controller);
void Test11(BufferController::Ptr &controller);
void Test12(BufferController::Ptr &controller);
void Test13(BufferController::Ptr &controller);
void Test14(BufferController::Ptr &controller);
void Test15(BufferController::Ptr &controller);
void Test16(BufferController::Ptr &controller);
void Test17(BufferController::Ptr &controller);
void Test18(BufferController::Ptr &controller);
void Test19(BufferController::Ptr &controller);
void Test20(BufferController::Ptr &controller);
void Test21(BufferController::Ptr &controller);
void Test22(BufferController::Ptr &controller);
void Test23(BufferController::Ptr &controller);
void Test24(BufferController::Ptr &controller);
void Test25(BufferController::Ptr &controller);
void Test26(BufferController::Ptr &controller);
void Test27(BufferController::Ptr &controller);
void Test28(BufferController::Ptr &controller);
void Test29(BufferController::Ptr &controller);
void Test30(BufferController::Ptr &controller);
void Test31(BufferController::Ptr &controller);
void Test32(BufferController::Ptr &controller);
void Test33(BufferController::Ptr &controller);
void Test34(BufferController::Ptr &controller);
void Test35(BufferController::Ptr &controller);

int main(int argc, char *argv[])
{
    if (argc != 2) {
        SHARING_LOGE("please select test mode");
        return -1;
    }

    BufferController::Ptr controller = std::make_shared<BufferController>();
    int mode = atoi(argv[1]);
    FuncMap testFunc{{1, &Test1},   {2, &Test2},   {3, &Test3},   {4, &Test4},   {5, &Test5},   {6, &Test6},
                     {7, &Test7},   {8, &Test8},   {9, &Test9},   {10, &Test10}, {11, &Test11}, {12, &Test12},
                     {13, &Test13}, {14, &Test14}, {15, &Test15}, {16, &Test16}, {17, &Test17}, {18, &Test18},
                     {19, &Test19}, {20, &Test20}, {21, &Test21}, {22, &Test22}, {23, &Test23}, {24, &Test24},
                     {25, &Test25}, {26, &Test26}, {27, &Test27}, {28, &Test28}, {29, &Test29}, {30, &Test30},
                     {31, &Test31}, {32, &Test32}, {33, &Test33}, {34, &Test34}, {35, &Test35}};

    auto iter = testFunc.find(mode);
    if (iter == testFunc.end()) {
        SHARING_LOGE("mode: %{public}d not found", mode);
        return -1;
    } else {
        (*(iter->second))(controller);
    }

    while (1) {
        SHARING_LOGD("you can press CTRL+C to exit");
        sleep(1);
    }
    return 0;
}

void Test1(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test write audio only no read:");
    SHARING_LOGD("length can grow up to the default N, and the old node will be deleted after exceeding. ");
    controller->StartWriteSeqData();
    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test2(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test write video only no read:");
    SHARING_LOGD("configurate length according to the GopSize success, 2 * gopsize * 1.2 < default max N:");
    controller->SetDummyCount(300);
    controller->SetGopSize(60);
    controller->GenerateVideoDemoData();
    controller->SetWriteIndex(0);
    controller->StartWriteSeqData(false);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test3(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test write video only no read:");
    SHARING_LOGD("configurate length according to the GopSize error, 2 * gopsize * 1.2 > default max N:");
    controller->SetDummyCount(300);
    controller->SetGopSize(300);
    controller->GenerateVideoDemoData();
    controller->SetWriteIndex(0);
    controller->StartWriteSeqData(false);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test4(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test write av only no read:");
    SHARING_LOGD("configurate length according to the GopSize success, 2 * gopsize * 1.2 < default max N:");

    controller->SetDummyCount(600);
    controller->SetGopSize(30);
    controller->GenerateAVDemoData(0);
    controller->SetWriteIndex(0);
    controller->StartWriteSeqData(false);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test5(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test write av only no read:");
    SHARING_LOGD("configurate length according to the GopSize error, 2 * gopsize * 1.2 > default max N:");

    controller->SetDummyCount(600);
    controller->SetGopSize(300);
    controller->GenerateAVDemoData(0);
    controller->SetWriteIndex(0);
    controller->StartWriteSeqData(false);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test6(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test write av only no read:");
    SHARING_LOGD("after configurate length, insert key frame, current length < N:");

    controller->SetDummyCount(100);
    controller->SetGopSize(30);
    controller->GenerateAVDemoData();
    controller->SetWriteIndex(0);

    // input one keyframe
    controller->AppendDemoData(2);
    controller->AppendDemoData(2);
    controller->StartWriteSeqData(false);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test7(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test write av only no read:");
    SHARING_LOGD("after configurate length, insert key frame, current length > N:");

    controller->SetDummyCount(150);
    controller->SetGopSize(30);
    controller->GenerateAVDemoData();
    // input one keyframe
    controller->AppendDemoData(2);
    controller->AppendDemoData(2);
    controller->SetWriteIndex(0);
    controller->StartWriteSeqData(false);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test8(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test write av only no read:");
    SHARING_LOGD("after configurate length, insert non key frame, current length up to 2N:");

    controller->SetDummyCount(220);
    controller->SetGopSize(30);
    controller->GenerateAVDemoData();
    controller->AppendDemoData(3);
    controller->AppendDemoData(4);
    controller->SetWriteIndex(0);
    controller->StartWriteSeqData(false);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test9(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test write av only no read:");
    SHARING_LOGD("after configurate length, insert non key frame, current length up to 2N:");

    controller->SetDummyCount(220);
    controller->SetGopSize(30);
    controller->GenerateAVDemoData();
    controller->AppendDemoData(2);
    controller->SetWriteIndex(0);
    controller->StartWriteSeqData(false);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test10(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test write av one sequential read:");
    SHARING_LOGD("after configurate length, current length < N:");
    SHARING_LOGD("now ts: %{public}u", controller->ReturnTS());
    // for dummy test
    controller->SetDummyCount(100);
    controller->SetGopSize(30);
    controller->GenerateAVDemoData();
    controller->SetWriteIndex(0);

    SHARING_LOGD("now ts: %{public}u", controller->ReturnTS());
    controller->AppendDemoData(2);
    controller->AppendDemoData(2);
    SHARING_LOGI("Start Write Data");
    controller->StartWriteSeqData(false);

    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receiver1->Init(DUMMY);
    receivers.push_back(receiver1);
    receivers[0]->SetReadInterval(30);
    receivers[0]->StartReadSeqData(DUMMY);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    receivers[0]->StopReadSeqData();
    controller->GetDispatcher()->DetachReceiver(receivers[0]);
    receivers[0]->Release();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test11(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test write av one sequential read:");
    SHARING_LOGD("after configurate length, current length > N & < 2N:");

    controller->SetDummyCount(150);
    controller->SetGopSize(30);
    controller->GenerateAVDemoData();
    for (int i = 0; i < 40; ++i) {
        controller->AppendDemoData(3);
    }
    controller->SetWriteIndex(0);

    SHARING_LOGD("now ts: %{public}u", controller->ReturnTS());
    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);

    SHARING_LOGD("now ts: %{public}u", controller->ReturnTS());
    controller->AppendDemoData(2);
    controller->AppendDemoData(2);
    controller->AppendDemoData(2);
    controller->AppendDemoData(2);
    controller->AppendDemoData(2);
    controller->AppendDemoData(2);
    SHARING_LOGI("Start Write Data");
    controller->StartWriteSeqData(false);

    usleep(1000 * 25);
    receivers[0]->SetReadInterval(30);
    receivers[0]->StartReadSeqData(DUMMY);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    receivers[0]->StopReadSeqData();
    controller->GetDispatcher()->DetachReceiver(receivers[0]);
    receivers[0]->Release();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test12(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test write av one sequential read:");
    SHARING_LOGD("after configurate length, current length > 2N:");

    controller->SetDummyCount(220);
    controller->SetGopSize(30);
    controller->GenerateAVDemoData();
    controller->SetWriteIndex(0);

    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);

    controller->AppendDemoData(2);
    controller->AppendDemoData(2);
    controller->AppendDemoData(2);
    controller->AppendDemoData(2);
    SHARING_LOGI("Start Write Data");
    controller->StartWriteSeqData(false);

    receivers[0]->SetReadInterval(30);
    receivers[0]->StartReadSeqData(DUMMY);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    receivers[0]->StopReadSeqData();
    controller->GetDispatcher()->DetachReceiver(receivers[0]);
    receivers[0]->Release();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test13(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test write av one sequential read:");
    SHARING_LOGD("after configurate length, current length > 2N:");

    controller->SetDummyCount(220);
    controller->SetGopSize(30);
    controller->GenerateAVDemoData();
    controller->SetWriteIndex(0);

    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);

    controller->AppendDemoData(3);
    controller->AppendDemoData(3);
    controller->AppendDemoData(3);
    controller->AppendDemoData(3);
    controller->AppendDemoData(3);
    controller->AppendDemoData(3);
    controller->AppendDemoData(3);
    controller->AppendDemoData(3);
    SHARING_LOGI("Start Write Data");
    controller->StartWriteSeqData(false);

    receivers[0]->SetReadInterval(30);
    receivers[0]->StartReadSeqData(DUMMY);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    receivers[0]->StopReadSeqData();
    controller->GetDispatcher()->DetachReceiver(receivers[0]);
    receivers[0]->Release();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test14(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test write av one read audio only:");

    controller->SetDummyCount(200);
    controller->SetGopSize(30);
    controller->GenerateAVDemoData();
    controller->SetWriteIndex(0);

    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(DUMMY);

    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);

    SHARING_LOGI("Start Write Data");
    controller->StartWriteSeqData(false);

    receivers[0]->SetReadInterval(30);
    receivers[0]->StartReadSeqData(DUMMY, MEDIA_TYPE_AUDIO);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    receivers[0]->StopReadSeqData(true);
    controller->GetDispatcher()->DetachReceiver(receiver1);
    receivers[0]->Release();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test15(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test write av one read video only:");

    std::vector<BufferRecevierImpl::Ptr> receivers;
    const int NUM = 8;
    for (int i = 0; i < NUM; ++i) {
        BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
        receiver1->Init(READ_TYPE_VIDEO);
        controller->GetDispatcher()->AttachReceiver(receiver1);
        if (i % 2) {
            receiver1->EnableKeyMode(true);
        }
        receivers.push_back(receiver1);
    }

    SHARING_LOGI("Start Write Data");

    controller->StartWriteSeqData();
    for (int i = 0; i < NUM; ++i) {
        receivers[i]->StartReadSeqData(READ_TYPE_VIDEO, MEDIA_TYPE_VIDEO);
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
    }

    std::this_thread::sleep_for(std::chrono::seconds(45));
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test16(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test write av one multi read:");
    // for dummy test
    controller->SetDummyCount(100);
    controller->SetGopSize(30);
    controller->GenerateAVDemoData();
    controller->SetWriteIndex(0);

    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);
    BufferRecevierImpl::Ptr receiver2 = std::make_shared<BufferRecevierImpl>();
    receiver2->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver2);
    receivers.push_back(receiver2);
    BufferRecevierImpl::Ptr receiver3 = std::make_shared<BufferRecevierImpl>();
    receiver3->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver3);
    receivers.push_back(receiver3);
    controller->AppendDemoData(2);

    SHARING_LOGI("Start Write Data");
    controller->StartWriteSeqData(false);

    receivers[0]->SetReadInterval(30);
    receivers[0]->StartReadSeqData(DUMMY, MEDIA_TYPE_AUDIO);
    receivers[1]->SetReadInterval(30);
    receivers[1]->StartReadSeqData(DUMMY, MEDIA_TYPE_VIDEO);
    receivers[2]->SetReadInterval(30);
    receivers[2]->StartReadSeqData(DUMMY, MEDIA_TYPE_AV);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    receivers[0]->StopReadSeqData();
    receivers[1]->StopReadSeqData();
    receivers[2]->StopReadSeqData();
    controller->GetDispatcher()->DetachReceiver(receivers[0]);
    controller->GetDispatcher()->DetachReceiver(receivers[1]);
    controller->GetDispatcher()->DetachReceiver(receivers[2]);
    receivers[0]->Release();
    receivers[1]->Release();
    receivers[2]->Release();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test17(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test write av one multi read:");
    controller->SetDummyCount(150);
    controller->SetGopSize(30);
    controller->GenerateAVDemoData();
    controller->SetWriteIndex(0);

    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);
    BufferRecevierImpl::Ptr receiver2 = std::make_shared<BufferRecevierImpl>();
    receiver2->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver2);
    receivers.push_back(receiver2);
    BufferRecevierImpl::Ptr receiver3 = std::make_shared<BufferRecevierImpl>();
    receiver3->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver3);
    receivers.push_back(receiver3);
    controller->AppendDemoData(2);

    SHARING_LOGI("Start Write Data");
    controller->StartWriteSeqData(false);

    receivers[0]->SetReadInterval(30);
    receivers[0]->StartReadSeqData(DUMMY, MEDIA_TYPE_AUDIO);
    receivers[1]->SetReadInterval(30);
    receivers[1]->StartReadSeqData(DUMMY, MEDIA_TYPE_VIDEO);
    receivers[2]->SetReadInterval(30);
    receivers[2]->StartReadSeqData(DUMMY, MEDIA_TYPE_AV);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    receivers[0]->StopReadSeqData();
    receivers[1]->StopReadSeqData();
    receivers[2]->StopReadSeqData();
    controller->GetDispatcher()->DetachReceiver(receivers[0]);
    controller->GetDispatcher()->DetachReceiver(receivers[1]);
    controller->GetDispatcher()->DetachReceiver(receivers[2]);
    receivers[0]->Release();
    receivers[1]->Release();
    receivers[2]->Release();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test18(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test write av one multi read:");

    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(READ_TYPE_AUDIO);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);
    BufferRecevierImpl::Ptr receiver2 = std::make_shared<BufferRecevierImpl>();
    receiver2->Init(READ_TYPE_VIDEO);
    controller->GetDispatcher()->AttachReceiver(receiver2);
    receivers.push_back(receiver2);
    BufferRecevierImpl::Ptr receiver3 = std::make_shared<BufferRecevierImpl>();
    receiver3->Init(READ_TYPE_AV);
    controller->GetDispatcher()->AttachReceiver(receiver3);
    receivers.push_back(receiver3);
    BufferRecevierImpl::Ptr receiver4 = std::make_shared<BufferRecevierImpl>();
    receiver4->Init(READ_TYPE_AV);
    receiver4->EnableKeyMode(true);
    controller->GetDispatcher()->AttachReceiver(receiver4);
    receivers.push_back(receiver4);
    BufferRecevierImpl::Ptr receiver5 = std::make_shared<BufferRecevierImpl>();
    receiver5->Init(READ_TYPE_AV);
    controller->GetDispatcher()->AttachReceiver(receiver5);
    receivers.push_back(receiver5);

    SHARING_LOGI("Start Write Data");
    // for real test
    controller->StartWriteSeqData();
    receivers[0]->SetReadInterval(30);
    receivers[0]->StartReadSeqData(READ_TYPE_AUDIO, MEDIA_TYPE_AUDIO);
    receivers[1]->SetReadInterval(30);
    receivers[1]->StartReadSeqData(READ_TYPE_VIDEO, MEDIA_TYPE_VIDEO);
    receivers[2]->SetReadInterval(30);
    receivers[2]->StartReadSeqData(READ_TYPE_AV, MEDIA_TYPE_AV, true);
    receivers[3]->SetReadInterval(30);
    receivers[3]->StartReadSeqData(READ_TYPE_AV, MEDIA_TYPE_AV, true);
    receivers[4]->SetReadInterval(30);
    receivers[4]->StartReadSeqData(READ_TYPE_AV, MEDIA_TYPE_AV, false);

    std::this_thread::sleep_for(std::chrono::seconds(10));
    receiver4->EnableKeyMode(false);

    std::this_thread::sleep_for(std::chrono::seconds(10));
    receiver4->EnableKeyMode(true);

    std::this_thread::sleep_for(std::chrono::seconds(60));
    controller->StopWriteSeqData();
    receivers[0]->StopReadSeqData();
    receivers[1]->StopReadSeqData();
    receivers[2]->StopReadSeqData(true);
    receivers[3]->StopReadSeqData(true);
    receivers[4]->StopReadSeqData(false);
    controller->GetDispatcher()->DetachReceiver(receiver1);
    controller->GetDispatcher()->DetachReceiver(receiver2);
    controller->GetDispatcher()->DetachReceiver(receiver3);
    controller->GetDispatcher()->DetachReceiver(receiver4);
    controller->GetDispatcher()->DetachReceiver(receiver5);
    receiver1->Release();
    receiver2->Release();
    receiver3->Release();
    receiver4->Release();
    receiver5->Release();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test19(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test write av one read delay in:");

    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(READ_TYPE_AV);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);
    BufferRecevierImpl::Ptr receiver2 = std::make_shared<BufferRecevierImpl>();
    receiver2->Init(READ_TYPE_AV);
    controller->GetDispatcher()->AttachReceiver(receiver2);
    receivers.push_back(receiver2);

    SHARING_LOGI("Start Write Data");
    // for real test
    controller->StartWriteSeqData();
    receivers[0]->SetReadInterval(30);
    receivers[0]->StartReadSeqData(READ_TYPE_AV);
    // receiver1 delay 2s in
    std::this_thread::sleep_for(std::chrono::seconds(2));
    receivers[1]->SetReadInterval(30);
    receivers[1]->StartReadSeqData(READ_TYPE_AV);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    receivers[0]->StopReadSeqData();
    receivers[1]->StopReadSeqData();
    controller->StopWriteSeqData();
    controller->GetDispatcher()->DetachReceiver(receiver1);
    controller->GetDispatcher()->DetachReceiver(receiver2);
    receiver1->Release();
    receiver2->Release();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test20(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test one read rate change:");
    // for dummy test
    controller->SetDummyCount(1000);
    controller->SetGopSize(30);
    controller->GenerateAVDemoData();
    controller->SetWriteIndex(0);

    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);

    SHARING_LOGI("Start Write Data");
    controller->StartWriteSeqData(false);

    receivers[0]->SetReadInterval(30);
    receivers[0]->StartReadSeqData(DUMMY);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    receivers[0]->SetReadInterval(5);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    receivers[0]->SetReadInterval(100);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    receivers[0]->SetReadInterval(15);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    receivers[0]->StopReadSeqData();
    controller->GetDispatcher()->DetachReceiver(receivers[0]);
    receivers[0]->Release();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test21(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test single mode change to mixed mode:");
    // for dummy test
    controller->SetDummyCount(110);
    controller->GenerateAudioDemoData();
    controller->SetWriteIndex(0);
    for (int i = 0; i < 3; ++i) {
        controller->AppendDemoData(2);
        for (int j = 0; j < 39; ++j) {
            controller->AppendDemoData(3);
            if (j % 5 == 0) {
                controller->AppendDemoData(4);
            }
        }
    }
    for (int i = 0; i < 100; ++i) {
        controller->AppendDemoData(3);
    }
    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);
    SHARING_LOGI("Start Write Data");
    controller->StartWriteSeqData(false);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test22(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test mixed mode change to single mode:");
    // for dummy test
    controller->SetWriteIndex(0);
    controller->AppendDemoData(2);
    for (int i = 0; i < 39; ++i) {
        controller->AppendDemoData(3);
    }
    controller->AppendDemoData(2);
    for (int i = 0; i < 200; ++i) {
        controller->AppendDemoData(4);
    }
    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);

    SHARING_LOGI("Start Write Data");
    controller->StartWriteSeqData(false);

    receivers[0]->SetReadInterval(30);
    receivers[0]->StartReadSeqData(DUMMY);
    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    receivers[0]->StopReadSeqData();
    controller->GetDispatcher()->DetachReceiver(receivers[0]);
    receivers[0]->Release();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test23(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test sequential read:");
    // for dummy test
    controller->SetDummyCount(1000);
    controller->SetGopSize(30);
    controller->GenerateAVDemoData();

    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);
    controller->SetWriteIndex(0);
    SHARING_LOGI("Start Write Data");
    controller->StartWriteSeqData(false);

    receivers[0]->SetReadInterval(30);
    receivers[0]->StartReadSeqData(DUMMY);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    SHARING_LOGD("STOP!!");
    receivers[0]->StopReadSeqData();
    controller->GetDispatcher()->DetachReceiver(receivers[0]);
    receivers[0]->Release();
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test24(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test mixed read, change read rate:");
    // for dummy test
    controller->SetDummyCount(1000);
    controller->SetGopSize(30);
    controller->GenerateAVDemoData();

    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);

    BufferRecevierImpl::Ptr receiver2 = std::make_shared<BufferRecevierImpl>();
    receiver2->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver2);
    receivers.push_back(receiver2);

    BufferRecevierImpl::Ptr receiver3 = std::make_shared<BufferRecevierImpl>();
    receiver3->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver3);
    receivers.push_back(receiver3);

    controller->SetWriteIndex(0);
    SHARING_LOGI("Start Write Data");
    controller->StartWriteSeqData(false);

    receivers[0]->SetReadInterval(20);
    receivers[0]->StartReadSeqData(DUMMY, MEDIA_TYPE_AUDIO);
    receivers[1]->SetReadInterval(30);
    receivers[1]->StartReadSeqData(DUMMY, MEDIA_TYPE_VIDEO);
    receivers[2]->SetReadInterval(25);
    receivers[2]->StartReadSeqData(DUMMY);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    receivers[0]->StopReadSeqData();
    receivers[1]->StopReadSeqData();
    receivers[2]->StopReadSeqData();
    controller->GetDispatcher()->DetachReceiver(receivers[0]);
    controller->GetDispatcher()->DetachReceiver(receivers[1]);
    controller->GetDispatcher()->DetachReceiver(receivers[2]);
    receivers[0]->Release();
    receivers[1]->Release();
    receivers[2]->Release();
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test25(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test one read, another no read, gop size normal:");
    // for dummy test
    controller->SetDummyCount(0);
    controller->SetGopSize(40);
    controller->AppendDemoData(2);
    for (int i = 1; i <= 38; ++i) {
        controller->AppendDemoData(4);
    }
    controller->AppendDemoData(2);
    for (int i = 1; i <= 80; ++i) {
        controller->AppendDemoData(3);
    }
    controller->AppendDemoData(2);
    controller->AppendDemoData(2);
    controller->AppendDemoData(2);
    controller->AppendDemoData(3);
    controller->AppendDemoData(3);
    controller->AppendDemoData(4);
    controller->AppendDemoData(4);
    controller->SetWriteIndex(0);
    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);

    BufferRecevierImpl::Ptr receiver2 = std::make_shared<BufferRecevierImpl>();
    receiver2->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver2);
    receivers.push_back(receiver2);

    receivers[0]->SetReadInterval(25);
    receivers[1]->SetReadInterval(30);

    controller->StartWriteSeqData(false);

    receivers[0]->StartReadSeqData(DUMMY);
    std::this_thread::sleep_for(std::chrono::seconds(45));

    receivers[0]->StopReadSeqData();
    controller->GetDispatcher()->DetachReceiver(receivers[0]);
    receivers[0]->Release();
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test26(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test one read, another no read, gop size large:");
    // for dummy test
    controller->SetDummyCount(0);
    controller->SetGopSize(100);
    controller->AppendDemoData(4);
    controller->AppendDemoData(4);
    controller->AppendDemoData(2);
    for (int i = 1; i <= 99; ++i) {
        controller->AppendDemoData(4);
    }
    controller->AppendDemoData(2);
    for (int i = 1; i <= 100; ++i) {
        controller->AppendDemoData(4);
    }
    controller->AppendDemoData(2);

    controller->SetWriteIndex(0);
    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);

    BufferRecevierImpl::Ptr receiver2 = std::make_shared<BufferRecevierImpl>();
    receiver2->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver2);
    receivers.push_back(receiver2);

    receivers[0]->SetReadInterval(25);

    controller->StartWriteSeqData(false);

    receivers[0]->StartReadSeqData(DUMMY);
    std::this_thread::sleep_for(std::chrono::seconds(45));
    receivers[0]->StopReadSeqData();
    controller->GetDispatcher()->DetachReceiver(receivers[0]);
    receivers[0]->Release();
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test27(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test only read no write, block read:");
    // only read no write, block read;
    // for dummy test
    controller->SetDummyCount(0);
    // set data at first
    controller->SetGopSize(40);
    controller->AppendDemoData(2);
    for (int i = 1; i <= 38; ++i) {
        controller->AppendDemoData(3);
        controller->AppendDemoData(4);
    }
    controller->AppendDemoData(2);
    controller->SetWriteIndex(0);

    // no write data for 2s
    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);
    receivers[0]->SetReadInterval(25);
    receivers[0]->StartReadSeqData(DUMMY);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // start write, notify to read
    controller->StartWriteSeqData(false);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    controller->StopWriteSeqData();
    for (int i = 0; i < 100; ++i) {
        controller->AppendDemoData(4);
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));
    controller->StartWriteSeqData();
    // read done, block again
    std::this_thread::sleep_for(std::chrono::seconds(45));
    receivers[0]->StopReadSeqData();
    controller->GetDispatcher()->DetachReceiver(receivers[0]);
    receivers[0]->Release();
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test28(BufferController::Ptr &controller)
{
    SHARING_LOGD("start test read busy, not notify:");
    // read busy, queue not notify
    // once read done, requestRead notify queue send a signal to read
    // for dummy test
    controller->SetDummyCount(0);
    controller->SetGopSize(40);
    for (int k = 0; k < 10; ++k) {
        controller->AppendDemoData(2);
        for (int i = 1; i <= 38; ++i) {
            controller->AppendDemoData(3);
            controller->AppendDemoData(4);
        }
        controller->AppendDemoData(2);
    }
    controller->SetWriteIndex(0);
    // one reads normally, another reads very slowly
    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);
    receivers[0]->SetReadInterval(30);
    BufferRecevierImpl::Ptr receiver2 = std::make_shared<BufferRecevierImpl>();
    receiver2->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver2);
    receivers.push_back(receiver2);
    receivers[1]->SetReadInterval(500);

    controller->StartWriteSeqData(false);

    receivers[0]->StartReadSeqData(DUMMY);
    receivers[1]->StartReadSeqData(DUMMY);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    receivers[0]->StopReadSeqData();
    receivers[1]->StopReadSeqData();
    controller->GetDispatcher()->DetachReceiver(receivers[0]);
    controller->GetDispatcher()->DetachReceiver(receivers[1]);
    receivers[0]->Release();
    receivers[1]->Release();
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test29(BufferController::Ptr &controller)
{
    // for dummy test
    controller->SetDummyCount(100);
    controller->GenerateAudioDemoData();
    controller->SetWriteIndex(0);

    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);

    SHARING_LOGI("Start Write Data");
    controller->StartWriteSeqData(false);

    receivers[0]->SetReadInterval(30);
    receivers[0]->StartReadSeqData(DUMMY);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    controller->StopWriteSeqData();
    controller->GetDispatcher()->DetachReceiver(receivers[0]);
    receivers[0]->Release();
    receivers[0]->StopReadSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test30(BufferController::Ptr &controller)
{
    // for dummy test
    controller->SetDummyCount(1000);
    controller->SetGopSize(30);
    controller->GenerateAVDemoData();

    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);
    controller->SetWriteIndex(0);
    receivers[0]->SetReadInterval(30);
    SHARING_LOGI("Start Write Data");
    controller->StartWriteSeqData(false);

    receivers[0]->StartReadSeqData(DUMMY, MEDIA_TYPE_AV, true);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    receivers[0]->StopReadSeqData(true);
    controller->GetDispatcher()->DetachReceiver(receivers[0]);
    receivers[0]->Release();
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test31(BufferController::Ptr &controller)
{
    // for dummy test
    controller->SetDummyCount(1000);
    controller->SetGopSize(30);
    controller->GenerateAVDemoData();

    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);
    BufferRecevierImpl::Ptr receiver2 = std::make_shared<BufferRecevierImpl>();
    receiver2->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver2);
    receivers.push_back(receiver2);

    controller->SetWriteIndex(0);
    receivers[0]->SetReadInterval(30);
    receivers[0]->EnableKeyMode(true);
    SHARING_LOGI("Start Write Data");
    controller->StartWriteSeqData(false);

    receivers[0]->StartReadSeqData(DUMMY, MEDIA_TYPE_AV, true);
    receivers[1]->StartReadSeqData(DUMMY, MEDIA_TYPE_AV, true);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    SHARING_LOGD("one receiver exits");
    receivers[1]->StopReadSeqData(true);
    controller->GetDispatcher()->DetachReceiver(receiver2);
    receivers.pop_back();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    SHARING_LOGD("one receiver reads in again");
    BufferRecevierImpl::Ptr receiver3 = std::make_shared<BufferRecevierImpl>();
    receiver3->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver2);
    receivers.push_back(receiver2);
    controller->GetDispatcher()->AttachReceiver(receiver3);
    receivers.push_back(receiver3);
    receivers[1]->StartReadSeqData(DUMMY, MEDIA_TYPE_AV, true);
    receivers[2]->StartReadSeqData(DUMMY, MEDIA_TYPE_AV, true);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    receivers[0]->StopReadSeqData(true);
    receivers[1]->StopReadSeqData(true);
    receivers[2]->StopReadSeqData(true);
    controller->GetDispatcher()->DetachReceiver(receivers[0]);
    controller->GetDispatcher()->DetachReceiver(receivers[1]);
    controller->GetDispatcher()->DetachReceiver(receivers[2]);
    receivers[0]->Release();
    receivers[1]->Release();
    receivers[2]->Release();
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test32(BufferController::Ptr &controller)
{
    const int NUM = 16;
    std::vector<BufferRecevierImpl::Ptr> receivers;
    for (int i = 0; i < NUM; ++i) {
        BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
        receiver1->Init(READ_TYPE_VIDEO);

        controller->GetDispatcher()->AttachReceiver(receiver1);
        receivers.push_back(receiver1);
    }

    controller->SetWriteIndex(0);
    receivers[0]->SetReadInterval(30);
    SHARING_LOGI("Start Write Data");
    controller->StartWriteSeqData();
    for (int i = 0; i < NUM; ++i) {
        receivers[i]->StartReadSeqData(READ_TYPE_VIDEO, MEDIA_TYPE_VIDEO);
    }
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test33(BufferController::Ptr &controller)
{
    // for dummy test
    controller->SetDummyCount(1000);
    controller->SetGopSize(30);
    controller->GenerateAVDemoData();
    const int NUM = 8;
    std::vector<BufferRecevierImpl::Ptr> receivers;
    for (int i = 0; i < NUM; ++i) {
        BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
        receiver1->Init(DUMMY);
        controller->GetDispatcher()->AttachReceiver(receiver1);
        receivers.push_back(receiver1);
    }

    controller->SetWriteIndex(0);
    receivers[0]->SetReadInterval(30);
    SHARING_LOGI("Start Write Data");
    for (int i = 0; i < NUM; ++i) {
        receivers[i]->StartReadSeqData(DUMMY, MEDIA_TYPE_AV, true);
    }
    controller->StartWriteSeqData(false);

    std::this_thread::sleep_for(std::chrono::seconds(60));
    for (int i = 0; i < NUM; ++i) {
        receivers[i]->StopReadSeqData(true);
        controller->GetDispatcher()->DetachReceiver(receivers[i]);
        receivers[i]->Release();
    }
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test34(BufferController::Ptr &controller)
{
    // for dummy test
    controller->SetDummyCount(1000);
    controller->SetGopSize(30);
    controller->GenerateAVDemoData();

    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receivers.push_back(receiver1);
    BufferRecevierImpl::Ptr receiver2 = std::make_shared<BufferRecevierImpl>();
    receiver2->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver2);
    receivers.push_back(receiver2);

    controller->SetWriteIndex(0);
    receivers[0]->SetReadInterval(30);
    receivers[0]->EnableKeyMode(true);
    SHARING_LOGI("Start Write Data");
    controller->StartWriteSeqData(false);
    receivers[0]->StartReadSeqData(DUMMY, MEDIA_TYPE_AV, true);
    receivers[1]->StartReadSeqData(DUMMY, MEDIA_TYPE_AV, true);

    std::this_thread::sleep_for(std::chrono::seconds(5));
    SHARING_LOGD("one receiver reads in again");
    BufferRecevierImpl::Ptr receiver3 = std::make_shared<BufferRecevierImpl>();
    receiver3->Init(DUMMY);
    controller->GetDispatcher()->AttachReceiver(receiver3);
    receivers.push_back(receiver3);
    receivers[2]->StartReadSeqData(DUMMY, MEDIA_TYPE_AV, true);

    std::this_thread::sleep_for(std::chrono::seconds(45));
    receivers[0]->StopReadSeqData(true);
    receivers[1]->StopReadSeqData(true);
    receivers[2]->StopReadSeqData(true);
    controller->GetDispatcher()->DetachReceiver(receivers[0]);
    controller->GetDispatcher()->DetachReceiver(receivers[1]);
    controller->GetDispatcher()->DetachReceiver(receivers[2]);
    receivers[0]->Release();
    receivers[1]->Release();
    receivers[2]->Release();
    controller->StopWriteSeqData();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}

void Test35(BufferController::Ptr &controller)
{
    std::vector<BufferRecevierImpl::Ptr> receivers;
    BufferRecevierImpl::Ptr receiver1 = std::make_shared<BufferRecevierImpl>();
    receiver1->Init(READ_TYPE_AV);
    controller->GetDispatcher()->AttachReceiver(receiver1);
    receiver1->EnableKeyMode(true);
    receivers.push_back(receiver1);

    SHARING_LOGI("Start Write Data");
    controller->StartWriteSeqData();
    receivers[0]->SetReadInterval(30);
    receivers[0]->StartReadSeqData(READ_TYPE_AV, MEDIA_TYPE_AV, true);
    std::this_thread::sleep_for(std::chrono::seconds(10));
    SHARING_LOGD("disable key only mode");
    receiver1->EnableKeyMode(false);

    std::this_thread::sleep_for(std::chrono::seconds(10));
    SHARING_LOGD("enable key only mode");
    receiver1->EnableKeyMode(true);

    std::this_thread::sleep_for(std::chrono::seconds(36));
    controller->StopWriteSeqData();
    receivers[0]->StopReadSeqData(true);
    controller->GetDispatcher()->DetachReceiver(receiver1);
    receiver1->Release();
    SHARING_LOGD("test done enter for exit:");
    getchar();
}
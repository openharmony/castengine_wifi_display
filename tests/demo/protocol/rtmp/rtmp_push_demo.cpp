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

#include <condition_variable>
#include <memory>
#include <mutex>
#include <unistd.h>
#include "common/sharing_log.h"
#include "h264_parser.h"
#include "rtmp_publisher.h"

using namespace OHOS::Sharing;

#define RTMP_URL  "rtmp://192.168.62.38:1935/live/0"
#define PUSH_FILE "/system/bin/test.h264"
#define HTTP_URL  "http://192.168.62.38:8080/live/01.flv"

int TestRtmpPublisher(EventLoop *event_loop);

class H264File {
public:
    H264File(int bufSize = 5000000);
    ~H264File();

    bool open(const char *path);
    void Close();

    bool isOpened() const
    {
        return (m_file != NULL);
    }

    int readFrame(char *inBuf, int inBufSize, bool *bEndOfFrame);

private:
    FILE *m_file = NULL;
    char *m_buf = NULL;
    int m_bufSize = 0;
    int m_bytesUsed = 0;
    int m_count = 0;
};

H264File::H264File(int bufSize) : m_bufSize(bufSize)
{
    m_buf = new char[m_bufSize];
}

H264File::~H264File()
{
    delete m_buf;
}

bool H264File::open(const char *path)
{
    m_file = fopen(path, "rb");
    if (m_file == NULL) {
        return false;
    }

    return true;
}

void H264File::Close()
{
    if (m_file) {
        fclose(m_file);
        m_file = NULL;
        m_count = 0;
        m_bytesUsed = 0;
    }
}

int H264File::readFrame(char *inBuf, int inBufSize, bool *bEndOfFrame)
{
    if (m_file == NULL) {
        return -1;
    }

    int bytesRead = (int)fread(m_buf, 1, m_bufSize, m_file);
    if (bytesRead == 0) {
        fseek(m_file, 0, SEEK_SET);
        m_count = 0;
        m_bytesUsed = 0;
        bytesRead = (int)fread(m_buf, 1, m_bufSize, m_file);
        if (bytesRead == 0) {
            this->Close();
            return -1;
        }
    }

    bool bFindStart = false, bFindEnd = false;

    int i = 0, startCode = 3;
    *bEndOfFrame = false;
    for (i = 0; i < bytesRead - 5; i++) {
        if (m_buf[i] == 0 && m_buf[i + 1] == 0 && m_buf[i + 2] == 1) {
            startCode = 3;
        } else if (m_buf[i] == 0 && m_buf[i + 1] == 0 && m_buf[i + 2] == 0 && m_buf[i + 3] == 1) {
            startCode = 4;
        } else {
            continue;
        }

        if (((m_buf[i + startCode] & 0x1F) == 0x5 || (m_buf[i + startCode] & 0x1F) == 0x1) &&
            ((m_buf[i + startCode + 1] & 0x80) == 0x80)) {
            bFindStart = true;
            i += 4;
            break;
        }
    }

    for (; i < bytesRead - 5; i++) {
        if (m_buf[i] == 0 && m_buf[i + 1] == 0 && m_buf[i + 2] == 1) {
            startCode = 3;
        } else if (m_buf[i] == 0 && m_buf[i + 1] == 0 && m_buf[i + 2] == 0 && m_buf[i + 3] == 1) {
            startCode = 4;
        } else {
            continue;
        }

        if (((m_buf[i + startCode] & 0x1F) == 0x7) || ((m_buf[i + startCode] & 0x1F) == 0x8) ||
            ((m_buf[i + startCode] & 0x1F) == 0x6) ||
            (((m_buf[i + startCode] & 0x1F) == 0x5 || (m_buf[i + startCode] & 0x1F) == 0x1) &&
             ((m_buf[i + startCode + 1] & 0x80) == 0x80))) {
            bFindEnd = true;
            break;
        }
    }

    bool flag = false;
    if (bFindStart && !bFindEnd && m_count > 0) {
        flag = bFindEnd = true;
        i = bytesRead;
        *bEndOfFrame = true;
    }

    if (!bFindStart || !bFindEnd) {
        this->Close();
        return -1;
    }

    int size = (i <= inBufSize ? i : inBufSize);
    memcpy(inBuf, m_buf, size);

    if (!flag) {
        m_count += 1;
        m_bytesUsed += i;
    } else {
        m_count = 0;
        m_bytesUsed = 0;
    }

    fseek(m_file, m_bytesUsed, SEEK_SET);
    return size;
}

int TestRtmpPublisher(EventLoop *event_loop)
{
    H264File h264_file;
    if (!h264_file.open(PUSH_FILE)) {
        printf("Open %s failed.\n", PUSH_FILE);
        return -1;
    }

    /* push stream to local rtmp server */
    MediaInfo media_info;
    auto publisher = RtmpPublisher::Create(event_loop);

    std::string status;
    if (publisher->OpenUrl(RTMP_URL, 3000, status) < 0) {
        printf("Open url %s failed, status: %s\n", RTMP_URL, status.c_str());
        return -1;
    }
    sleep(6);
    int buf_size = 500000;
    bool end_of_frame = false;
    bool has_sps_pps = false;
    uint8_t *frame_buf = new uint8_t[buf_size];

    while (publisher->IsConnected()) {
        int frameSize = h264_file.readFrame((char *)frame_buf, buf_size, &end_of_frame);
        if (frameSize > 0) {
            if (!has_sps_pps) {
                if (frame_buf[3] == 0x67 || frame_buf[4] == 0x67) {
                    Nal sps = H264Parser::findNal(frame_buf, frameSize);
                    if (sps.first != nullptr && sps.second != nullptr && *sps.first == 0x67) {
                        media_info.sps_size = (uint32_t)(sps.second - sps.first + 1);
                        media_info.sps.reset(new uint8_t[media_info.sps_size], std::default_delete<uint8_t[]>());
                        memcpy(media_info.sps.get(), sps.first, media_info.sps_size);

                        Nal pps = H264Parser::findNal(sps.second, frameSize - (int)(sps.second - frame_buf));
                        if (pps.first != nullptr && pps.second != nullptr && *pps.first == 0x68) {
                            media_info.pps_size = (uint32_t)(pps.second - pps.first + 1);
                            media_info.pps.reset(new uint8_t[media_info.pps_size], std::default_delete<uint8_t[]>());
                            memcpy(media_info.pps.get(), pps.first, media_info.pps_size);

                            has_sps_pps = true;
                            publisher->SetMediaInfo(media_info); /* set sps pps */
                            printf("Start rtmp pusher, rtmp url: %s , http-flv url: %s \n\n", RTMP_URL, HTTP_URL);
                        }
                    }
                }
            }

            if (has_sps_pps) {
                publisher->PushVideoFrame(frame_buf, frameSize); /* send h.264 frame */
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }

    delete[] frame_buf;
    return 0;
}

int main()
{
    EventLoop event_loop(1);
    std::thread t([&event_loop]() { TestRtmpPublisher(&event_loop); });
    t.detach();
    while (true) {
        /* code */
        sleep(1);
    }

    return 0;
}
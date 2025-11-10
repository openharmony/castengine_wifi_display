/*
 * Copyright (c) 2025 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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
#ifndef WFD_SINK_HISYS_EVENT_H
#define WFD_SINK_HISYS_EVENT_H

#include <map>
#include <mutex>
#include <string>

namespace OHOS {
namespace Sharing {
// 定义业务场景枚举
enum class SinkBizScene : int32_t {
    ESTABLISH_MIRRORING = 1,  // 建立miracast镜像投屏
    MIRRORING_STABILITY = 2,  // miracast镜像投屏稳定性
    DISCONNECT_MIRRORING = 3  // 断开miracast镜像投屏
};

// 定义业务阶段枚举
enum class SinkStage : int32_t {
    // 建立miracast镜像投屏阶段
    P2P_CONNECT_SUCCESS = 1,
    SESSION_NEGOTIATION = 2,
    SEND_M7_MSG = 3,
    RECEIVE_DATA = 4,
    FIRST_FRAME_PROCESSED = 5,
    // miracast镜像投屏稳定性阶段
    RECEIVE_DATA_STABILITY = 1,
    RTP_DEMUX = 2,
    VIDEO_DECODE = 3,
    AUDIO_DECODE = 4,
    AV_SYNC_PROCESS = 5,
    // 断开miracast镜像投屏阶段
    RECEIVE_DISCONNECT_EVENT = 1,
    DISCONNECT_COMPLETE = 2
};

// 定义阶段结果枚举
enum class SinkStageRes : int32_t {
    START = 0,
    SUCCESS = 1,
    FAIL = 2,
    CANCEL = 3
};

enum class SinkBIZState : int32_t {
    BIZ_STATE_BEGIN = 1,
    BIZ_STATE_END = 2
};

// 定义错误码枚举
enum class SinkErrorCode : int32_t {
    // P2P连接失败错误码
    WIFI_DISPLAY_P2P_FAILED = 476119140,
    WIFI_DISPLAY_P2P_OPT_NOT_SUPPORTED,
    WIFI_DISPLAY_P2P_OPT_INVALID_PARAM,
    WIFI_DISPLAY_P2P_OPT_FORBID_AIRPLANE,
    WIFI_DISPLAY_P2P_OPT_FORBID_POWSAVING,
    WIFI_DISPLAY_P2P_OPT_PERMISSION_DENIED,
    WIFI_DISPLAY_P2P_OPT_OPEN_FAIL_WHEN_CLOSING,
    WIFI_DISPLAY_P2P_OPT_P2P_NOT_OPENED,
    WIFI_DISPLAY_P2P_DISCONNECTED_STATE_CLOSED,
    WIFI_DISPLAY_P2P_GCJOINGROUP_TIMEOUT,
    //会话协商错误码
    WIFI_DISPLAY_RTSP_KEEPALIVE_TIMEOUT = 476184676,
    WIFI_DISPLAY_RTSP_FAILED,
    WIFI_DISPLAY_TCP_FAILED,
    WIFI_DISPLAY_UDP_FAILED,
    WIFI_DISPLAY_VTP_FAILED,
    WIFI_DISPLAY_DATA_INVALID,
    WIFI_DISPLAY_LOCAL_IP_FAILED,
    //音视频解码错误码
    WIFI_DISPLAY_RTP_DATA_INVALID = 476250212,
    WIFI_DISPLAY_VIDEO_DECODE_FAILED,
    WIFI_DISPLAY_AUDIO_DECODE_FAILED,
    WIFI_DISPLAY_VIDEO_DECODE_TIMEOUT,
    WIFI_DISPLAY_AUDIO_DECODE_TIMEOUT,
    WIFI_DISPLAY_SYNC_FAILED,
    //系统模块错误码
    WIFI_DISPLAY_GENERAL_ERROR = 476315748,
    WIFI_DISPLAY_BAD_PARAMETER,
    WIFI_DISPLAY_ADD_SURFACE_ERROR,
    WIFI_DISPLAY_AGENT_ERROR,
    WIFI_DISPLAY_CONSUMER_ERROR
};

enum class MediaReportType : int32_t {
    VIDEO,
    AUDIO
};

class WfdSinkHiSysEvent {
public:

    WfdSinkHiSysEvent(const WfdSinkHiSysEvent&) = delete;
    WfdSinkHiSysEvent& operator=(const WfdSinkHiSysEvent&) = delete;

    static WfdSinkHiSysEvent &GetInstance();

    struct SinkHisyseventDevInfo {
        std::string localNetId = "";
        std::string localWifiMac = "";
        std::string localDevName = "";
        std::string localIp = "";
        std::string peerNetId = "";
        std::string peerWifiMac = "";
        std::string peerIp = "";
        std::string peerDevName = "";
    };
    //获取对端设备信息
    void SetHiSysEventDevInfo(const SinkHisyseventDevInfo &devInfo);

    void GetStartTime(std::chrono::system_clock::time_point startTime);

    void ReportEstablishMirroringError(const std::string &funcName, const std::string &toCallpkg,
                                        SinkStage sinkStage, SinkErrorCode errorCode, int32_t duration);

    void ReportStabilityError(const std::string &funcName, const std::string &toCallpkg,
                                SinkStage sinkStage, SinkErrorCode errorCode);

    void ReportDisconnectError(const std::string &funcName, const std::string &toCallpkg,
                                SinkStage sinkStage, SinkErrorCode errorCode, int32_t duration);

    // 开始打点
    void StartReport(const std::string &funcName, const std::string &toCallpkg,
                    SinkStage sinkStage, SinkStageRes sinkStageRes);

    void Report(const std::string &funcName, const std::string &toCallpkg,
                SinkStage sinkStage, SinkStageRes sinkStageRes);

    void FirstSceneEndReport(const std::string &funcName, const std::string &toCallpkg,
                            SinkStage sinkStage, SinkStageRes sinkStageRes);

    void ThirdSceneEndReport(const std::string &funcName, const std::string &toCallpkg, SinkStage sinkStage);

    void ReportError(const std::string &funcName, const std::string &toCallpkg,
                    SinkStage sinkStage, SinkErrorCode errorCode);

    void P2PReportError(const std::string &funcName, SinkErrorCode errorCode);
    
    //更换场景
    void ChangeHisysEventScene(SinkBizScene scene);

    int32_t GetCurrentScene();

    void RecordMediaDecodeStartTime(MediaReportType type, uint64_t pts);

    void MediaDecodeTimProc(MediaReportType type, uint64_t pts);

    void Reset();

private:
    uint64_t GetCurTimeInUs();
    void ReportDecodeError(MediaReportType type);
    void RecordDecordStartTime(uint64_t pts, std::map<uint64_t, uint64_t>& decodeMap);
    void ReportDecodeTime(uint64_t pts, std::map<uint64_t, uint64_t>& decodeMap, MediaReportType type);

private:
    WfdSinkHiSysEvent() = default;
    ~ WfdSinkHiSysEvent() = default;
    SinkHisyseventDevInfo devInfo_;

    int32_t sinkBizScene_ = 0;
    bool hiSysEventStart_ = true;
    int32_t startTime_ = 0;

    uint32_t audioFreezeCount_ = 0;
    uint32_t videoFreezeCount_ = 0;
    int64_t lastFreezeReportTime_ = 0;
    bool isFreezeReportInTenMinute = false;
    std::map<uint64_t, uint64_t> audioDecoderTime_{};
    std::map<uint64_t, uint64_t> videoDecoderTime_{};
    std::mutex decodeMutex_{};
};

}  // namespace Sharing
}  // namespace OHOS

#endif  // WFD_SINK_HISYS_EVENT_H
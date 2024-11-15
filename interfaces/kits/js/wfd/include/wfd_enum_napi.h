#ifndef OHOS_SHARING_WFD_ENUM_NAPI_H
#define OHOS_SHARING_WFD_ENUM_NAPI_H

#include <unordered_map>
#include "common/sharing_log.h"
#include "napi/native_api.h"
#include "wfd_js_result.h"

namespace OHOS {
namespace Sharing {

static const std::unordered_map<std::string, int32_t> mapVideoFormatId = {
    {"VIDEO_640x480_60", VideoFormat::VIDEO_640x480_60},
    {"VIDEO_1280x720_25", VideoFormat::VIDEO_1280x720_25},
    {"VIDEO_1280x720_30", VideoFormat::VIDEO_1280x720_30},
    {"VIDEO_1920x1080_25", VideoFormat::VIDEO_1920x1080_25},
    {"VIDEO_1920x1080_30", VideoFormat::VIDEO_1920x1080_30}};

static const std::unordered_map<std::string, int32_t> mapAudioFormatId = {
    {"AUDIO_48000_16_2", AudioFormat::AUDIO_48000_16_2}, {"AUDIO_48000_16_4", AudioFormat::AUDIO_48000_16_4}};

static const std::unordered_map<std::string, int32_t> mapDeviceState = {
    {"CONNECTED", DeviceState::CONNECTED},
    {"DISCONNECTED", DeviceState::DISCONNECTED},
    {"INIT", DeviceState::INIT},
    {"READY", DeviceState::READY},
    {"PLAYING", DeviceState::PLAYING},
    {"PAUSED", DeviceState::PAUSED},
    {"STOPPED", DeviceState::STOPPED},};

constexpr int32_t CommonErrorCode = 18800001;
static const std::unordered_map<std::string, int32_t> mapErrorCode = {{"ERR_OK", 0},
                                                                      {"ERR_GENERAL_ERROR", CommonErrorCode},
                                                                      {"ERR_BAD_PARAMETER", 18800002},
                                                                      {"ERR_INVALID_ID", 18800003},
                                                                      {"ERR_SERVICE_LIMIT", 18800004},
                                                                      {"ERR_STATE_EXCEPTION", 18800005},
                                                                      {"ERR_CONNECTION_FAILURE", 18800006},
                                                                      {"ERR_INTERACTION_FAILURE", 18800007},
                                                                      {"ERR_CONNECTION_TIMEOUT", 18800008},
                                                                      {"ERR_CONNECTION_REFUSED", 18800009},
                                                                      {"ERR_NETWORK_ERROR ", 18800011},
                                                                      {"ERR_INVALID_SURFACE_ID", 18800101},
                                                                      {"ERR_SURFACE_FAILURE", 18800102},
                                                                      {"ERR_DECODE_ERROR", 18800601},
                                                                      {"ERR_DECODE_FORMAT", 18800602},
                                                                      {"ERR_DECODE_SCARCE_CAPACITY", 18800603},
                                                                      {"ERR_PLAY_START", 18800608},
                                                                      {"ERR_PLAY_STOP", 18800609},
                                                                      {"ERR_RECEIVING_LIMIT", 18802101}};

static const std::unordered_map<std::string, int32_t> mapSceneType = {{"FOREGROUND", SceneType::FOREGROUND},
                                                                      {"BACKGROUND", SceneType::BACKGROUND}};

static const std::unordered_map<std::string, int32_t> mapCodecType = {
    {"CODEC_H264", CodecId::CODEC_H264},   {"CODEC_H265", CodecId::CODEC_H265},   {"CODEC_AAC", CodecId::CODEC_AAC},
    {"CODEC_G711A", CodecId::CODEC_G711A}, {"CODEC_G711U", CodecId::CODEC_G711U}, {"CODEC_Opus", CodecId::CODEC_Opus},
    {"CODEC_L16", CodecId::CODEC_L16},     {"CODEC_VP8", CodecId::CODEC_VP8},     {"CODEC_VP9", CodecId::CODEC_VP9},
    {"CODEC_AV1", CodecId::CODEC_AV1},     {"CODEC_PCM", CodecId::CODEC_PCM},
};

napi_status InitEnums(napi_env env, napi_value exports);

} // namespace Sharing
} // namespace OHOS
#endif
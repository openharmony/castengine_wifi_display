## Purpose

Sink 竂模块物理隔离与条件编译规范：将 Sink-only 代码从公共模块中分离到 `services/sink/` 目录结构下，并通过 `wifi_display_support_sink` feature flag 控制编译与链接。

## Requirements

### Requirement: Sink-only code physical separation
Sink-only 文件/目录 SHALL 从当前位置移动或拆分到 `services/sink/` 下。Step1 整文件移动清单：

| 原路径 | 目标路径 |
|--------|----------|
| `mediaplayer/` (整个目录) | `sink/mediaplayer/` |
| `agent/sinkagent/` | `sink/agent/sinkagent/` |
| `impl/wfd/wfd_sink/` | `sink/impl/wfd/wfd_sink/` |
| `impl/scene/wfd/wfd_sink_scene.h/cpp` | `sink/impl/scene/wfd/wfd_sink_scene.h/cpp` |
| `impl/scene/wfd/sink/` | `sink/impl/scene/wfd/sink/` |
| `codec/include/video_sink_decoder.h` | `sink/codec/include/video_sink_decoder.h` |
| `codec/src/video_sink_decoder.cpp` | `sink/codec/src/video_sink_decoder.cpp` |
| `codec/include/audio_avcodec_decoder.h` | `sink/codec/include/audio_avcodec_decoder.h` |
| `codec/src/audio_avcodec_decoder.cpp` | `sink/codec/src/audio_avcodec_decoder.cpp` |
| `codec/include/video_audio_sync.h` | `sink/codec/include/video_audio_sync.h` |
| `codec/src/video_audio_sync.cpp` | `sink/codec/src/video_audio_sync.cpp` |

Step2 追加以下整文件/类拆分：

| 原路径 | 目标路径 | 类型 |
|--------|----------|------|
| `codec/include/audio_aac_codec.h/.cpp` 中 AudioAACDecoder | `sink/codec/include/audio_aac_decoder.h` + `sink/codec/src/audio_aac_decoder.cpp` | 类拆分 |
| `codec/include/audio_g711_codec.h/.cpp` 中 AudioG711Decoder+G711_TYPE | `sink/codec/include/audio_g711_decoder.h` + `sink/codec/src/audio_g711_decoder.cpp` | 类拆分 |
| `codec/include/codec_factory.h/.cpp` 中 Sink 部分 | `sink/codec/include/sink_codec_factory.h` + `sink/codec/src/sink_codec_factory.cpp` | 类拆分（SinkCodecFactory 新类） |
| `common/sharing_sink_hisysevent.h/.cpp` | `sink/common/include/` + `sink/common/src/` | 整文件移动 |
| `protocol/rtp/include/rtp_codec.h` 中 RtpDecoder | `sink/protocol/rtp/include/rtp_decoder.h` | 类拆分 |
| `protocol/rtp/include/rtp_factory.h/.cpp` 中 CreateRtpUnpack | `sink/protocol/rtp/include/rtp_sink_factory.h` + `sink/protocol/rtp/src/rtp_sink_factory.cpp` | 类拆分（RtpSinkFactory） |
| `protocol/rtp/include/rtp_unpack.h` | `sink/protocol/rtp/include/rtp_unpack.h` | 整文件移动 |
| `protocol/rtp/include/rtp_unpack_impl.h/.cpp` | `sink/protocol/rtp/include/` + `sink/protocol/rtp/src/` | 整文件移动 |
| `protocol/rtp/include/rtp_queue.h/.cpp` | `sink/protocol/rtp/include/` + `sink/protocol/rtp/src/` | 整文件移动 |
| `protocol/rtp/include/adts.h/.cpp` | `sink/protocol/rtp/include/` + `sink/protocol/rtp/src/` | 整文件移动 |
| `protocol/rtp/include/rtp_codec_h264.h/.cpp` 中 RtpDecoderH264 | `sink/protocol/rtp/include/rtp_decoder_h264.h` + `sink/protocol/rtp/src/rtp_decoder_h264.cpp` | 类拆分 |
| `protocol/rtp/include/rtp_codec_ts.h/.cpp` 中 RtpDecoderTs | `sink/protocol/rtp/include/rtp_decoder_ts.h` + `sink/protocol/rtp/src/rtp_decoder_ts.cpp` | 类拆分 |
| `protocol/rtp/include/rtp_codec_aac.h/.cpp` 中 RtpDecoderAAC | `sink/protocol/rtp/include/rtp_decoder_aac.h` + `sink/protocol/rtp/src/rtp_decoder_aac.cpp` | 类拆分 |
| `protocol/rtp/include/rtp_codec_g711.h/.cpp` 中 RtpDecoderG711 | `sink/protocol/rtp/include/rtp_decoder_g711.h` + `sink/protocol/rtp/src/rtp_decoder_g711.cpp` | 类拆分 |
| `impl/scene/wfd/wfd_def.h` 中 SinkConfig,DevSurfaceItem,WfdParamsInfo | `sink/impl/scene/wfd/include/sink_def.h` | 类拆分 |
| `impl/wfd/wfd_session_def.h` 中 WfdSinkSessionEventMsg | `sink/impl/wfd/include/sink_session_def.h` | 类拆分 |
| `impl/wfd/wfd_media_def.h` 中 WfdConsumerEventMsg | `sink/impl/wfd/include/sink_media_def.h` | 类拆分（修复 Ptr bug） |

#### Scenario: Sink-only files exist in services/sink/ after move
- **WHEN** Step 1 移动操作完成
- **THEN** 上述所有 Step1 Sink-only 文件 SHALL 存在于 `services/sink/` 对应子目录中，且原位置不再有这些文件

#### Scenario: Sink-only files maintain relative paths
- **WHEN** `sinkagent/sink_agent.cpp` 从 `agent/sinkagent/` 移到 `sink/agent/sinkagent/`
- **THEN** Sink agent 的相对路径 `agent/sinkagent/sink_agent.cpp` 在新位置 `sink/` 下保持一致

#### Scenario: Sink codec files split to sink/codec/
- **WHEN** codec 拆分完成
- **THEN** `sink/codec/include/audio_aac_decoder.h/.cpp`、`audio_g711_decoder.h/.cpp`、`sink_codec_factory.h/.cpp` SHALL 存在，且 `services/codec/include/audio_aac_codec.h/.cpp`、`audio_g711_codec.h/.cpp`、`codec_factory.h/.cpp` SHALL 不再存在

#### Scenario: Sink hisysevent moved to sink/common/
- **WHEN** hisysevent 拆分完成
- **THEN** `sharing_sink_hisysevent.h/.cpp` SHALL 存在于 `services/sink/common/include/` 和 `services/sink/common/src/`

#### Scenario: RTP Sink module fully isolated
- **WHEN** RTP 拆分完成
- **THEN** `sink/protocol/rtp/` SHALL 包含 rtp_decoder.h、rtp_sink_factory.h/.cpp、rtp_unpack.h、rtp_unpack_impl.h/.cpp、rtp_queue.h/.cpp、adts.h/.cpp、rtp_decoder_h264/ts/aac/g711.h/.cpp，且 `services/protocol/rtp/` SHALL 只剩 rtp_def.h、rtp_packet.h/.cpp

#### Scenario: SinkConfig/DevSurfaceItem/WfdParamsInfo in sink_def.h
- **WHEN** wfd_def.h 拆分完成
- **THEN** `sink/impl/scene/wfd/include/sink_def.h` SHALL 包含这三个 struct，且 `wfd_def.h` SHALL 不再包含它们

#### Scenario: WfdConsumerEventMsg::Ptr bug fixed
- **WHEN** wfd_media_def.h 拆分完成
- **THEN** `sink_media_def.h` 中 WfdConsumerEventMsg::Ptr SHALL 为 `std::shared_ptr<WfdConsumerEventMsg>`（而非 `std::shared_ptr<WfdProducerEventMsg>`）

### Requirement: Sink-only compilation gated by wifi_display_support_sink
所有移到 `services/sink/` 的编译产物 SHALL 仅在 `wifi_display_support_sink=true` 时编译和链接。

#### Scenario: Sink modules compiled when sink enabled
- **WHEN** `wifi_display_support_sink=true`
- **THEN** `sink/mediaplayer:sharing_media_player_srcs`、`sink/agent:sharing_sink_agent_srcs`、`sink/impl/wfd:sharing_wfd_sink_session_srcs`、`sink/impl/scene/wfd:sharing_wfd_sink_scene_srcs`、`sink/codec:sharing_codec_sink_srcs` SHALL 被编译和链接到 `sharing_service`

#### Scenario: Sink modules excluded when sink disabled
- **WHEN** `wifi_display_support_sink=false`
- **THEN** `sink/` 下所有 source_set SHALL 不被编译，`sharing_service` 的 deps 中 SHALL 不包含任何 `sink/` target

### Requirement: mediaplayer dependency gated by wifi_display_support_sink
`sharing_service` 和 `sharing_codec` 对 `mediaplayer` 的依赖 SHALL 从无条件改为 `wifi_display_support_sink` 条件依赖。

#### Scenario: mediaplayer compiled when sink enabled
- **WHEN** `wifi_display_support_sink=true` 且 `sharing_framework_support_wfd=true`
- **THEN** `sharing_service` SHALL 依赖 `sink/mediaplayer:sharing_media_player_srcs`，`sharing_codec` SHALL 依赖 `sink/mediaplayer:sharing_media_player_srcs`

#### Scenario: mediaplayer excluded when sink disabled
- **WHEN** `wifi_display_support_sink=false`
- **THEN** `sharing_service` 和 `sharing_codec` SHALL 不依赖 `mediaplayer`

### Requirement: Sink-only BUILD.gn targets created
以下 BUILD.gn 文件 SHALL 在 `services/sink/` 下新建，定义对应的 source_set target：

| BUILD.gn | target 名称 |
|----------|-------------|
| `sink/BUILD.gn` | 聚合所有 sink deps |
| `sink/agent/BUILD.gn` | `sharing_sink_agent_srcs` |
| `sink/mediaplayer/BUILD.gn` | `sharing_media_player_srcs` |
| `sink/impl/wfd/BUILD.gn` | `sharing_wfd_sink_session_srcs` |
| `sink/impl/scene/wfd/BUILD.gn` | `sharing_wfd_sink_scene_srcs` |
| `sink/codec/BUILD.gn` | `sharing_codec_sink_srcs` |

Step2 追加 BUILD.gn 变更：

| 变更 | 内容 |
|------|------|
| `sink/common/BUILD.gn` (新建) | `sharing_sink_common_srcs` source_set |
| `sink/protocol/rtp/BUILD.gn` (新建) | `sharing_sink_rtp_srcs` source_set，包含所有 Sink RTP 源文件 |
| `services/codec/BUILD.gn` (修改) | sink 条件块添加 sink/codec 角色专属 .cpp |
| `services/common/BUILD.gn` (修改) | sink 条件 deps 增加 sink/common |
| `services/protocol/rtp/BUILD.gn` (修改) | sink 条件 deps 增加 sink/protocol/rtp |

#### Scenario: Sink BUILD.gn targets compile correctly
- **WHEN** `wifi_display_support_sink=true`
- **THEN** 每个 `sink/` 下的 source_set SHALL 编译成功，deps 正确引用公共模块和 `sink/mediaplayer`

#### Scenario: Sink codec compiled via sharing_codec.z.so
- **WHEN** `wifi_display_support_sink=true`
- **THEN** sharing_codec sink 条件块 SHALL 包含 sink/codec 角色专属 .cpp

#### Scenario: Sink RTP compiled via sharing_rtp.z.so
- **WHEN** `wifi_display_support_sink=true`
- **THEN** sharing_rtp sink 条件块 SHALL 包含 sink/protocol/rtp 所有 Sink RTP 源文件

### Requirement: Original BUILD.gn targets remove moved sources
原位置的 BUILD.gn SHALL 从 sources 中移除已迁移的 Sink 文件。

#### Scenario: agent/BUILD.gn only contains common code
- **WHEN** `agent/sinkagent/` 移到 `sink/agent/sinkagent/`
- **THEN** `services/agent/BUILD.gn` 的 `sharing_agent_srcs` sources SHALL 只包含 `agent.cpp`，不再包含 `sinkagent/sink_agent.cpp`

#### Scenario: impl/wfd/BUILD.gn only contains common code
- **WHEN** `impl/wfd/wfd_sink/` 移到 `sink/impl/wfd/wfd_sink/`
- **THEN** `services/impl/wfd/BUILD.gn` 的 `sharing_wfd_session_srcs` sources SHALL 只包含 `wfd_message.cpp`，`wifi_display_support_sink` 条件块中不再有 `wfd_sink/` 的条目

#### Scenario: impl/scene/wfd/BUILD.gn only contains common code
- **WHEN** `wfd_sink_scene.*` 和 `sink/` 子目录移走
- **THEN** `services/impl/scene/wfd/BUILD.gn` 的 `sharing_wfd_srcs` sources SHALL 只包含 `wfd_utils.cpp`，`wifi_display_support_sink` 条件块中不再有 Sink 条目

#### Scenario: codec/BUILD.gn removes Sink codec files
- **WHEN** `video_sink_decoder`、`audio_avcodec_decoder`、`video_audio_sync` 移到 `sink/codec/`
- **THEN** `services/codec/BUILD.gn` 的 sources SHALL 不包含这 6 个 Sink codec 文件，`wifi_display_support_sink` 条件 deps 引用 `sink/codec:sharing_codec_sink_srcs`

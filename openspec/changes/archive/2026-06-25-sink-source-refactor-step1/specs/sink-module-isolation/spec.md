## ADDED Requirements

### Requirement: Sink-only code physical separation
Sink-only 文件/目录 SHALL 从当前位置移动到 `services/sink/` 下，保持与原代码的相对路径一致。移动清单 SHALL 包括以下整文件/整目录：

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

#### Scenario: Sink-only files exist in services/sink/ after move
- **WHEN** Step 1 移动操作完成
- **THEN** 上述所有 Sink-only 文件 SHALL 存在于 `services/sink/` 对应子目录中，且原位置不再有这些文件

#### Scenario: Sink-only files maintain relative paths
- **WHEN** `sinkagent/sink_agent.cpp` 从 `agent/sinkagent/` 移到 `sink/agent/sinkagent/`
- **THEN** Sink agent 的相对路径 `agent/sinkagent/sink_agent.cpp` 在新位置 `sink/` 下保持一致

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

#### Scenario: Sink BUILD.gn targets compile correctly
- **WHEN** `wifi_display_support_sink=true`
- **THEN** 每个 `sink/` 下的 source_set SHALL 编译成功，deps 正确引用公共模块和 `sink/mediaplayer`

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
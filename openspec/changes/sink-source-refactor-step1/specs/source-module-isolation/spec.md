## ADDED Requirements

### Requirement: Source-only code physical separation
Source-only 文件/目录 SHALL 从当前位置移动到 `services/source/` 下，保持与原代码的相对路径一致。移动清单 SHALL 包括以下整文件/整目录：

| 原路径 | 目标路径 |
|--------|----------|
| `impl/screen_capture/` (整个目录) | `source/impl/screen_capture/` |
| `agent/srcagent/` | `source/agent/srcagent/` |
| `impl/wfd/wfd_source/` | `source/impl/wfd/wfd_source/` |
| `impl/scene/wfd/wfd_source_scene.h/cpp` | `source/impl/scene/wfd/wfd_source_scene.h/cpp` |
| `codec/include/video_source_encoder.h` | `source/codec/include/video_source_encoder.h` |
| `codec/src/video_source_encoder.cpp` | `source/codec/src/video_source_encoder.cpp` |
| `codec/include/audio_pcm_processor.h` | `source/codec/include/audio_pcm_processor.h` |
| `codec/src/audio_pcm_processor.cpp` | `source/codec/src/audio_pcm_processor.cpp` |

#### Scenario: Source-only files exist in services/source/ after move
- **WHEN** Step 1 移动操作完成
- **THEN** 上述所有 Source-only 文件 SHALL 存在于 `services/source/` 对应子目录中，且原位置不再有这些文件

#### Scenario: Source-only files maintain relative paths
- **WHEN** `srcagent/src_agent.cpp` 从 `agent/srcagent/` 移到 `source/agent/srcagent/`
- **THEN** Source agent 的相对路径 `agent/srcagent/src_agent.cpp` 在新位置 `source/` 下保持一致

### Requirement: Source-only compilation gated by wifi_display_support_source
所有移到 `services/source/` 的编译产物 SHALL 仅在 `wifi_display_support_source=true` 时编译和链接。

#### Scenario: Source modules compiled when source enabled
- **WHEN** `wifi_display_support_source=true`
- **THEN** `source/agent:sharing_source_agent_srcs`、`source/impl/screen_capture:sharing_screen_capture_srcs`、`source/impl/wfd:sharing_wfd_source_session_srcs`、`source/impl/scene/wfd:sharing_wfd_source_scene_srcs`、`source/codec:sharing_codec_source_srcs` SHALL 被编译和链接到 `sharing_service`

#### Scenario: Source modules excluded when source disabled
- **WHEN** `wifi_display_support_source=false`
- **THEN** `source/` 下所有 source_set SHALL 不被编译，`sharing_service` 的 deps 中 SHALL 不包含任何 `source/` target

### Requirement: screen_capture dependency gated by wifi_display_support_source
`sharing_service` 对 `impl/screen_capture` 的依赖 SHALL 从 `sharing_framework_support_wfd` 条件改为 `wifi_display_support_source` 条件（仍在 wfd support 大条件内）。

#### Scenario: screen_capture compiled when source enabled under wfd
- **WHEN** `sharing_framework_support_wfd=true` 且 `wifi_display_support_source=true`
- **THEN** `sharing_service` SHALL 依赖 `source/impl/screen_capture:sharing_screen_capture_srcs`

#### Scenario: screen_capture excluded when source disabled
- **WHEN** `wifi_display_support_source=false`
- **THEN** `sharing_service` SHALL 不依赖 `screen_capture`

### Requirement: Source-only BUILD.gn targets created
以下 BUILD.gn 文件 SHALL 在 `services/source/` 下新建，定义对应的 source_set target：

| BUILD.gn | target 名称 |
|----------|-------------|
| `source/BUILD.gn` | 聚合所有 source deps |
| `source/agent/BUILD.gn` | `sharing_source_agent_srcs` |
| `source/impl/screen_capture/BUILD.gn` | `sharing_screen_capture_srcs` |
| `source/impl/wfd/BUILD.gn` | `sharing_wfd_source_session_srcs` |
| `source/impl/scene/wfd/BUILD.gn` | `sharing_wfd_source_scene_srcs` |
| `source/codec/BUILD.gn` | `sharing_codec_source_srcs` |

#### Scenario: Source BUILD.gn targets compile correctly
- **WHEN** `wifi_display_support_source=true`
- **THEN** 每个 `source/` 下的 source_set SHALL 编译成功，deps 正确引用公共模块

### Requirement: Original BUILD.gn targets remove moved Source sources
原位置的 BUILD.gn SHALL 从 sources 中移除已迁移的 Source 文件。

#### Scenario: agent/BUILD.gn only contains common code
- **WHEN** `agent/srcagent/` 移到 `source/agent/srcagent/`
- **THEN** `services/agent/BUILD.gn` 的 `sharing_agent_srcs` sources SHALL 只包含 `agent.cpp`，不再包含 `srcagent/src_agent.cpp`

#### Scenario: impl/wfd/BUILD.gn only contains common code
- **WHEN** `impl/wfd/wfd_source/` 移到 `source/impl/wfd/wfd_source/`
- **THEN** `services/impl/wfd/BUILD.gn` 的 `sharing_wfd_session_srcs` sources SHALL 只包含 `wfd_message.cpp`，`wifi_display_support_source` 条件块中不再有 `wfd_source/` 的条目

#### Scenario: impl/scene/wfd/BUILD.gn only contains common code
- **WHEN** `wfd_source_scene.*` 移走
- **THEN** `services/impl/scene/wfd/BUILD.gn` 的 `sharing_wfd_srcs` sources SHALL 只包含 `wfd_utils.cpp`，`wifi_display_support_source` 条件块中不再有 Source 条目

#### Scenario: codec/BUILD.gn removes Source codec files
- **WHEN** `video_source_encoder`、`audio_pcm_processor` 移到 `source/codec/`
- **THEN** `services/codec/BUILD.gn` 的 sources SHALL 不包含这 4 个 Source codec 文件，`wifi_display_support_source` 条件 deps 引用 `source/codec:sharing_codec_source_srcs`
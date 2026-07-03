## ADDED Requirements

### Requirement: WFD_SINK and WFD_SOURCE compile macros defined in BUILD.gn config
`WFD_SINK` 和 `WFD_SOURCE` 编译宏 SHALL 在 BUILD.gn 的 config 中通过 cflags 定义，与 `config.gni` 中的 `wifi_display_support_sink` 和 `wifi_display_support_source` feature flag 对应。

#### Scenario: WFD_SINK macro defined when sink feature enabled
- **WHEN** `wifi_display_support_sink=true`
- **THEN** 引入 `wifi_display_feature_flags` config 的所有 source_set/shared_library SHALL 在编译时获得 `-DWFD_SINK` cflag

#### Scenario: WFD_SOURCE macro defined when source feature enabled
- **WHEN** `wifi_display_support_source=true`
- **THEN** 引入 `wifi_display_feature_flags` config 的所有 source_set/shared_library SHALL 在编译时获得 `-DWFD_SOURCE` cflag

#### Scenario: Macros undefined when features disabled
- **WHEN** `wifi_display_support_sink=false`
- **THEN** `-DWFD_SINK` cflag SHALL 不出现在任何编译命令中
- **WHEN** `wifi_display_support_source=false`
- **THEN** `-DWFD_SOURCE` cflag SHALL 不出现在任何编译命令中

### Requirement: Unified config for feature flag macros
`config("wifi_display_feature_flags")` SHALL 定义在 services 顶层 BUILD.gn（或独立 config 文件中），被所有需要条件编译的 target 通过 configs 引入。

#### Scenario: All conditional targets use feature flag config
- **WHEN** 一个 BUILD.gn target 的代码中使用了 `#ifdef WFD_SINK` 或 `#ifdef WFD_SOURCE`
- **THEN** 该 target 的 configs 列表 SHALL 包含 `wifi_display_feature_flags`

#### Scenario: Existing WFD_SINK usage unified with new mechanism
- **WHEN** `agent.cpp` 当前使用 `#ifdef WFD_SINK`（宏名一致但之前未定义）
- **THEN** 通过 `wifi_display_feature_flags` config 定义后，`agent.cpp` 的 `#ifdef WFD_SINK` SHALL 正常生效

### Requirement: codec_factory.cpp uses WFD_SINK/WFD_SOURCE for conditional compilation
`codec_factory.cpp` SHALL 使用 `#ifdef WFD_SINK` 保护 Sink codec 的 `#include` 和 `switch case` 分支，使用 `#ifdef WFD_SOURCE` 保护 Source codec 的 `#include` 和 `switch case` 分支。

#### Scenario: Sink codec include guarded by WFD_SINK
- **WHEN** `audio_avcodec_decoder.h` 移到 `sink/codec/include/`
- **THEN** `codec_factory.cpp` 中 `#include "audio_avcodec_decoder.h"` SHALL 被 `#ifdef WFD_SINK` 包裹，`CreateAudioDecoder` 中 `CODEC_AAC` case 的 `AudioAvCodecDecoder` 创建 SHALL 被 `#ifdef WFD_SINK` 包裹

#### Scenario: Source codec include guarded by WFD_SOURCE
- **WHEN** `audio_pcm_processor.h` 移到 `source/codec/include/`
- **THEN** `codec_factory.cpp` 中 `#include "audio_pcm_processor.h"` SHALL 被 `#ifdef WFD_SOURCE` 包裹，`CreateAudioEncoder` 中 `CODEC_PCM` case 的 `AudioPcmProcessor` 创建 SHALL 被 `#ifdef WFD_SOURCE` 包裹

#### Scenario: Sink disabled codec factory behavior
- **WHEN** `WFD_SINK` 未定义
- **THEN** `CodecFactory::CreateAudioDecoder(CODEC_AAC)` SHALL return nullptr 并 log error

#### Scenario: Source disabled codec factory behavior
- **WHEN** `WFD_SOURCE` 未定义
- **THEN** `CodecFactory::CreateAudioEncoder(CODEC_PCM)` SHALL return nullptr 并 log error

### Requirement: codec BUILD.gn conditional include_dirs for Sink/Source
`services/codec/BUILD.gn` SHALL 在 `wifi_display_support_sink` 条件下加入 `sink/codec/include` 到 include_dirs 并 deps `sink/codec:sharing_codec_sink_srcs` 和 `sink/mediaplayer:sharing_media_player_srcs`；在 `wifi_display_support_source` 条件下加入 `source/codec/include` 到 include_dirs 并 deps `source/codec:sharing_codec_source_srcs`。

#### Scenario: Sink codec include_dirs available when sink enabled
- **WHEN** `wifi_display_support_sink=true`
- **THEN** `sharing_codec` 的 include_dirs SHALL 包含 `$SHARING_ROOT_DIR/services/sink/codec/include` 和 `$SHARING_ROOT_DIR/services/sink/mediaplayer/include`

#### Scenario: Source codec include_dirs available when source enabled
- **WHEN** `wifi_display_support_source=true`
- **THEN** `sharing_codec` 的 include_dirs SHALL 包含 `$SHARING_ROOT_DIR/services/source/codec/include`
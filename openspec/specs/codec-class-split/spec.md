## Purpose

Codec 模块中混合 Sink/Source 类的文件拆分为角色专属文件，实现 Sink 与 Source codec 代码的物理隔离。

## Requirements

### Requirement: Codec mixed-class physical split
同文件中混合 Sink 和 Source 类的 codec 文件 SHALL 从 `services/codec/` 拆分到 `services/sink/codec/` 和 `services/source/codec/` 对应目录下，每个角色专属文件只包含该角色的类定义和实现。拆分清单：

| 原文件 | Sink 类 → 目标 | Source 类 → 目标 |
|--------|----------------|------------------|
| `audio_aac_codec.h/.cpp` | AudioAACDecoder → `sink/codec/include/audio_aac_decoder.h` + `sink/codec/src/audio_aac_decoder.cpp` | AudioAACEncoder → `source/codec/include/audio_aac_encoder.h` + `source/codec/src/audio_aac_encoder.cpp` |
| `audio_g711_codec.h/.cpp` | AudioG711Decoder + G711_TYPE → `sink/codec/include/audio_g711_decoder.h` + `sink/codec/src/audio_g711_decoder.cpp` | AudioG711Encoder + G711_TYPE → `source/codec/include/audio_g711_encoder.h` + `source/codec/src/audio_g711_encoder.cpp` |
| `codec_factory.h/.cpp` | SinkCodecFactory → `sink/codec/include/sink_codec_factory.h` + `sink/codec/src/sink_codec_factory.cpp` | SourceCodecFactory → `source/codec/include/source_codec_factory.h` + `source/codec/src/source_codec_factory.cpp` |

原文件 SHALL 在拆分完成后从 `services/codec/` 中删除。

#### Scenario: AudioAACDecoder exists in sink/codec after split
- **WHEN** audio_aac_codec.h/.cpp 拆分完成
- **THEN** `sink/codec/include/audio_aac_decoder.h` SHALL 包含 AudioAACDecoder 类定义，`sink/codec/src/audio_aac_decoder.cpp` SHALL 包含 AudioAACDecoder 完整实现，且原 `services/codec/include/audio_aac_codec.h` 和 `services/codec/src/audio_aac_codec.cpp` SHALL 不再存在

#### Scenario: AudioG711Decoder with G711_TYPE in sink/codec
- **WHEN** audio_g711_codec.h/.cpp 拆分完成
- **THEN** `sink/codec/include/audio_g711_decoder.h` SHALL 包含 G711_TYPE 枚举和 AudioG711Decoder 类定义

#### Scenario: CodecFactory deleted after split
- **WHEN** codec_factory.h/.cpp 拆分完成
- **THEN** `services/codec/include/codec_factory.h` 和 `services/codec/src/codec_factory.cpp` SHALL 不再存在

### Requirement: CodecFactory split into role-specific factories
CodecFactory SHALL 拆分为 SinkCodecFactory（Sink 侧 codec 创建）和 SourceCodecFactory（Source 侧 codec 创建）两个独立类。

#### Scenario: SinkCodecFactory creates Sink decoders
- **WHEN** `wifi_display_support_sink=true`
- **THEN** SinkCodecFactory::CreateAudioDecoder(CODEC_AAC) SHALL 创建 AudioAvCodecDecoder，CODEC_G711A/U SHALL 创建 AudioG711Decoder

#### Scenario: SinkCodecFactory returns nullptr when Sink disabled
- **WHEN** `wifi_display_support_sink=false`
- **THEN** SinkCodecFactory::CreateAudioDecoder 所有 codecId SHALL return nullptr 并 log error

#### Scenario: SourceCodecFactory creates Source encoders
- **WHEN** `wifi_display_support_source=true`
- **THEN** SourceCodecFactory::CreateAudioEncoder SHALL 创建对应编码器

#### Scenario: SourceCodecFactory returns nullptr when Source disabled
- **WHEN** `wifi_display_support_source=false`
- **THEN** SourceCodecFactory::CreateAudioEncoder 所有 codecId SHALL return nullptr 并 log error

### Requirement: Codec callers use role-specific factories
所有原 CodecFactory 调用方 SHALL 改用对应角色工厂类。

#### Scenario: Sink audio player uses SinkCodecFactory
- **WHEN** audio_player.cpp 改用 SinkCodecFactory
- **THEN** `#include "codec_factory.h"` SHALL 改为 `#include "sink_codec_factory.h"`，`CodecFactory::CreateAudioDecoder` SHALL 改为 `SinkCodecFactory::CreateAudioDecoder`

#### Scenario: Source screen capture uses SourceCodecFactory
- **WHEN** screen_capture_consumer.cpp 改用 SourceCodecFactory
- **THEN** `#include "codec_factory.h"` SHALL 改为 `#include "source_codec_factory.h"`，`CodecFactory::CreateAudioEncoder` SHALL 改为 `SourceCodecFactory::CreateAudioEncoder`

### Requirement: Common codec infrastructure unchanged
公共 codec 基类 SHALL 保留在 `services/codec/` 原位不动。

#### Scenario: Common codec base classes remain in place
- **WHEN** codec 拆分完成
- **THEN** `audio_decoder.h`、`audio_encoder.h`、`video_decoder.h`、`video_encoder.h`、`media_frame_pipeline.h/.cpp` SHALL 保持不变

### Requirement: Codec BUILD.gn updated for split
`services/codec/BUILD.gn` SHALL 从 sources 删除已搬走公共 .cpp，条件块添加角色专属 .cpp。

#### Scenario: sharing_codec sources updated
- **WHEN** BUILD.gn 更新完成
- **THEN** sources SHALL 不再包含 `audio_aac_codec.cpp`、`audio_g711_codec.cpp`、`codec_factory.cpp`

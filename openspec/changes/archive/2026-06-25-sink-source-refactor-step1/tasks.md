## 1. 编译宏基础设施

- [ ] 1.1 在 services/BUILD.gn 中定义 `config("wifi_display_feature_flags")`，根据 `wifi_display_support_sink` 添加 `-DWFD_SINK` cflag，根据 `wifi_display_support_source` 添加 `-DWFD_SOURCE` cflag
- [ ] 1.2 在需要条件编译的现有 BUILD.gn target 中引入 `wifi_display_feature_flags` config：agent、codec、mediaplayer、sharing_service 主 target
- [ ] 1.3 将 `agent.cpp:239` 的现有 `#ifdef WFD_SINK` 确认与新宏定义机制一致（无需改名，宏名已一致）

## 2. 创建目录结构

- [ ] 2.1 创建 `services/sink/` 目录及其子目录：sink/agent/sinkagent/、sink/mediaplayer/include/ + src/、sink/impl/wfd/wfd_sink/、sink/impl/scene/wfd/sink/、sink/codec/include/ + src/、sink/common/
- [ ] 2.2 创建 `services/source/` 目录及其子目录：source/agent/srcagent/、source/impl/screen_capture/ + mediasource/、source/impl/wfd/wfd_source/、source/impl/scene/wfd/、source/codec/include/ + src/、source/common/

## 3. Sink 文件物理移动

- [ ] 3.1 移动 `services/agent/sinkagent/` → `services/sink/agent/sinkagent/`
- [ ] 3.2 移动 `services/mediaplayer/` 整目录 → `services/sink/mediaplayer/`
- [ ] 3.3 移动 `services/impl/wfd/wfd_sink/` → `services/sink/impl/wfd/wfd_sink/`
- [ ] 3.4 移动 `services/impl/scene/wfd/wfd_sink_scene.h/cpp` → `services/sink/impl/scene/wfd/wfd_sink_scene.h/cpp`
- [ ] 3.5 移动 `services/impl/scene/wfd/sink/` → `services/sink/impl/scene/wfd/sink/`
- [ ] 3.6 移动 `services/codec/include/video_sink_decoder.h` → `services/sink/codec/include/video_sink_decoder.h`
- [ ] 3.7 移动 `services/codec/src/video_sink_decoder.cpp` → `services/sink/codec/src/video_sink_decoder.cpp`
- [ ] 3.8 移动 `services/codec/include/audio_avcodec_decoder.h` → `services/sink/codec/include/audio_avcodec_decoder.h`
- [ ] 3.9 移动 `services/codec/src/audio_avcodec_decoder.cpp` → `services/sink/codec/src/audio_avcodec_decoder.cpp`
- [ ] 3.10 移动 `services/codec/include/video_audio_sync.h` → `services/sink/codec/include/video_audio_sync.h`
- [ ] 3.11 移动 `services/codec/src/video_audio_sync.cpp` → `services/sink/codec/src/video_audio_sync.cpp`

## 4. Source 文件物理移动

- [ ] 4.1 移动 `services/agent/srcagent/` → `services/source/agent/srcagent/`
- [ ] 4.2 移动 `services/impl/screen_capture/` 整目录 → `services/source/impl/screen_capture/`
- [ ] 4.3 移动 `services/impl/wfd/wfd_source/` → `services/source/impl/wfd/wfd_source/`
- [ ] 4.4 移动 `services/impl/scene/wfd/wfd_source_scene.h/cpp` → `services/source/impl/scene/wfd/wfd_source_scene.h/cpp`
- [ ] 4.5 移动 `services/codec/include/video_source_encoder.h` → `services/source/codec/include/video_source_encoder.h`
- [ ] 4.6 移动 `services/codec/src/video_source_encoder.cpp` → `services/source/codec/src/video_source_encoder.cpp`
- [ ] 4.7 移动 `services/codec/include/audio_pcm_processor.h` → `services/source/codec/include/audio_pcm_processor.h`
- [ ] 4.8 移动 `services/codec/src/audio_pcm_processor.cpp` → `services/source/codec/src/audio_pcm_processor.cpp`

## 5. Sink BUILD.gn 新建与调整

- [ ] 5.1 新建 `services/sink/BUILD.gn`：聚合 group，条件 `wifi_display_support_sink` 下包含所有 sink deps
- [ ] 5.2 新建 `services/sink/agent/BUILD.gn`：`sharing_sink_agent_srcs` source_set，deps 引用 `agent:sharing_agent_srcs`
- [ ] 5.3 修改 `services/sink/mediaplayer/BUILD.gn`：从原 mediaplayer BUILD.gn 改写，include_dirs 指向 `$SHARING_ROOT_DIR/services/sink/mediaplayer/include` 等新路径，configs 加入 `wifi_display_feature_flags`
- [ ] 5.4 新建 `services/sink/impl/wfd/BUILD.gn`：`sharing_wfd_sink_session_srcs` source_set
- [ ] 5.5 新建 `services/sink/impl/scene/wfd/BUILD.gn`：`sharing_wfd_sink_scene_srcs` source_set，包含 wfd_sink_scene.cpp + sink/ 子目录源文件，deps 引用 `sink/common`（hisysevent暂留common/不引）和公共模块
- [ ] 5.6 新建 `services/sink/codec/BUILD.gn`：`sharing_codec_sink_srcs` source_set，include_dirs 包含 sink/codec/include/ 和公共 codec/include/ 和 sink/mediaplayer/include/（video_audio_sync依赖），deps 引用 mediaplayer 和公共 codec
- [ ] 5.7 新建 `services/sink/common/BUILD.gn`：暂为空 placeholder（hisysevent暂不移，Step 2 再填充）

## 6. Source BUILD.gn 新建与调整

- [ ] 6.1 新建 `services/source/BUILD.gn`：聚合 group，条件 `wifi_display_support_source` 下包含所有 source deps
- [ ] 6.2 新建 `services/source/agent/BUILD.gn`：`sharing_source_agent_srcs` source_set，deps 引用 `agent:sharing_agent_srcs`
- [ ] 6.3 修改 `services/source/impl/screen_capture/BUILD.gn`：从原 screen_capture BUILD.gn 改写，路径更新，configs 加入 `wifi_display_feature_flags`
- [ ] 6.4 新建 `services/source/impl/wfd/BUILD.gn`：`sharing_wfd_source_session_srcs` source_set
- [ ] 6.5 新建 `services/source/impl/scene/wfd/BUILD.gn`：`sharing_wfd_source_scene_srcs` source_set
- [ ] 6.6 新建 `services/source/codec/BUILD.gn`：`sharing_codec_source_srcs` source_set，include_dirs 包含 source/codec/include/ 和公共 codec/include/，不依赖 mediaplayer
- [ ] 6.7 新建 `services/source/common/BUILD.gn`：暂为空 placeholder（hisysevent暂不移，Step 2 再填充）

## 7. 公共模块 BUILD.gn 调整

- [ ] 7.1 修改 `services/agent/BUILD.gn`：sources 只保留 `agent.cpp`，移除 `sinkagent/sink_agent.cpp` 和 `srcagent/src_agent.cpp`，configs 加入 `wifi_display_feature_flags`
- [ ] 7.2 修改 `services/impl/wfd/BUILD.gn`：sources 只保留 `wfd_message.cpp`，删除 `wifi_display_support_sink` 和 `wifi_display_support_source` 条件块（已移到 sink/source BUILD.gn）
- [ ] 7.3 修改 `services/impl/scene/wfd/BUILD.gn`：sources 只保留 `wfd_utils.cpp`，删除 sink/source 条件块（已移到 sink/source BUILD.gn）
- [ ] 7.4 修改 `services/codec/BUILD.gn`：sources 移除 6 个 sink/source codec 源文件，mediaplayer dep 从无条件改为 `if (wifi_display_support_sink)` 条件，新增 `if (wifi_display_support_sink)` 下 deps `sink/codec:sharing_codec_sink_srcs` + `sink/mediaplayer:sharing_media_player_srcs`，新增 `if (wifi_display_support_source)` 下 deps `source/codec:sharing_codec_source_srcs`，include_dirs 条件加入 sink/source codec 路径，configs 加入 `wifi_display_feature_flags`
- [ ] 7.5 修改 `services/common/BUILD.gn`：不做改动（hisysevent 暂留）

## 8. 主入口 BUILD.gn 调整

- [ ] 8.1 修改 `services/BUILD.gn`（sharing_service shared_library）：deps 中 `mediaplayer:sharing_media_player_srcs` 从无条件改为 `if (wifi_display_support_sink)` 条件（在 wfd support 大条件内）；在 `if (sharing_framework_support_wfd)` 内新增 `if (wifi_display_support_sink)` 和 `if (wifi_display_support_source)` 分别包含对应 sink/source deps；`impl/screen_capture:sharing_screen_capture_srcs` 从 wfd support 条件改为 `wifi_display_support_source` 条件（仍在 wfd support 内）；`sharing_services_package` group 加入 sink/source conditional deps
- [ ] 8.2 修改 `services/BUILD.gn`（sharing_services_package group）：加入 sink/source conditional deps 或保持仅引用 sharing_service + 公共 so（如果 sharing_service 已包含条件 deps，group 可能不需要额外改）

## 9. 代码改动（非移动文件）

- [ ] 9.1 修改 `services/codec/src/codec_factory.cpp`：`#include "audio_avcodec_decoder.h"` 改为 `#ifdef WFD_SINK` 条件包裹，`#include "audio_pcm_processor.h"` 改为 `#ifdef WFD_SOURCE` 条件包裹；`CreateAudioDecoder` 的 `CODEC_AAC` case 中 `AudioAvCodecDecoder` 创建改为 `#ifdef WFD_SINK` 包裹（else 分支 log error + return nullptr）；`CreateAudioEncoder` 的 `CODEC_PCM` case 中 `AudioPcmProcessor` 创建改为 `#ifdef WFD_SOURCE` 包裹（else 分支 log error + return nullptr）
- [ ] 9.2 修改所有移走文件内部的 `#include` 路径：确保引用公共模块的路径通过 `$SHARING_ROOT_DIR/services` include_dir 正确解析；检查跨模块引用（如 `wfd_sink_scene.cpp` 引用 `wfd_sink_session.h`、`wfd_rtp_consumer.h`、`sharing_sink_hisysevent.h` 等的路径是否需要更新）
- [ ] 9.3 修改 `sink/codec/src/video_audio_sync.cpp`：`#include "audio_play_controller.h"` 路径改为从 `sink/mediaplayer/include/` 引入
- [ ] 9.4 修改 `sink/codec/include/video_audio_sync.h`：`#include "audio_play_controller.h"` 路径改为从 `sink/mediaplayer/include/` 引入
- [ ] 9.5 修改 `sink/codec/include/video_sink_decoder.h`：`#include "video_audio_sync.h"` 路径改为从 `sink/codec/include/` 引用（路径不变，通过 include_dirs 解决）
- [ ] 9.6 检查并修改所有移走 .cpp 文件中对 Sink/Source 模块的交叉引用路径（如 `wfd_sink_scene.cpp` 引用 `sink/wfd_trust_list_manager.h` 的相对路径）

## 10. 测试 BUILD.gn 路径调整

- [ ] 10.1 修改 `tests/unittest/mediaplayer/BUILD.gn`：`mediaplayer/include` include_dirs 改为 `$SHARING_ROOT_DIR/services/sink/mediaplayer/include`；`codec/include` include_dirs 保留公共路径 + 添加 `$SHARING_ROOT_DIR/services/sink/codec/include`（使用 Sink codec 头文件）
- [ ] 10.2 修改 `tests/unittest/mediachannel/channel/BUILD.gn`：`mediaplayer/include` → `sink/mediaplayer/include`；`codec/include` 保留公共 + 添加 `sink/codec/include`
- [ ] 10.3 修改 `tests/unittest/mediachannel/dispatcher/BUILD.gn`：`mediaplayer/include` → `sink/mediaplayer/include`；`codec/include` 保留公共 + 添加 `sink/codec/include`
- [ ] 10.4 修改 `tests/unittest/impl/session/BUILD.gn`：`impl/wfd/wfd_sink` → `sink/impl/wfd/wfd_sink`；`impl/wfd/wfd_source` → `source/impl/wfd/wfd_source`；`mediaplayer/include` → `sink/mediaplayer/include`；sources 中 `wfd_sink_session.cpp` → `$SHARING_ROOT_DIR/services/sink/impl/wfd/wfd_sink/wfd_sink_session.cpp`；sources 中 `wfd_source_session.cpp` → `$SHARING_ROOT_DIR/services/source/impl/wfd/wfd_source/wfd_source_session.cpp`
- [ ] 10.5 修改 `tests/unittest/impl/rtp/BUILD.gn`：`impl/wfd/wfd_sink` → `sink/impl/wfd/wfd_sink`；`impl/wfd/wfd_source` → `source/impl/wfd/wfd_source`；`mediaplayer/include` → `sink/mediaplayer/include`；sources 中 `wfd_rtp_consumer.cpp` → `$SHARING_ROOT_DIR/services/sink/impl/wfd/wfd_sink/wfd_rtp_consumer.cpp`；sources 中 `wfd_rtp_producer.cpp` → `$SHARING_ROOT_DIR/services/source/impl/wfd/wfd_source/wfd_rtp_producer.cpp`
- [ ] 10.6 修改 `tests/unittest/impl/scene/BUILD.gn`：`impl/scene/wfd` include_dir 保留（公共头文件暂不拆分）；`impl/wfd` 保留
- [ ] 10.7 修改 `tests/unittest/impl/screen_capture/BUILD.gn`：`impl/screen_capture` → `source/impl/screen_capture`；`impl/screen_capture/mediasource` → `source/impl/screen_capture/mediasource`；`mediaplayer/include` → `sink/mediaplayer/include`；`codec/include` 保留公共 + 添加 `source/codec/include`
- [ ] 10.8 修改 `tests/unittest/codec/BUILD.gn`：`codec/include` 保留公共路径 + 添加 `$SHARING_ROOT_DIR/services/sink/codec/include`（Sink codec 测试需要）+ 添加 `$SHARING_ROOT_DIR/services/source/codec/include`（Source codec 测试需要）；configs 加入 `wifi_display_feature_flags`
- [ ] 10.9 修改 `tests/demo/player/BUILD.gn`：`mediaplayer/include` → `sink/mediaplayer/include`；`codec/include` 保留公共 + 添加 `sink/codec/include`
- [ ] 10.10 修改 `tests/demo/codec/BUILD.gn`：`codec/include` 保留公共 + 添加 `sink/codec/include` + 添加 `source/codec/include`
- [ ] 10.11 修改 `tests/unittest/agent/BUILD.gn`：deps 中 `agent:sharing_agent_srcs` 保留公共部分；如测试 SinkAgent/SrcAgent 需额外添加 `sink/agent:sharing_sink_agent_srcs` 和 `source/agent:sharing_source_agent_srcs` deps
- [ ] 10.12 修改 `tests/demo/wfd/BUILD.gn`：`impl/scene/wfd` 保留（wfd_def.h 等公共文件不移动）；检查是否引用了其它需要改的路径

## 11. 测试源文件 #include 路径调整

- [ ] 11.1 修改 `tests/unittest/agent/sink_agent_unit_test.cpp`：`#include "agent/sinkagent/sink_agent.h"` → `#include "sinkagent/sink_agent.h"`（通过 include_dirs `$SHARING_ROOT_DIR/services/sink/agent` 解析）
- [ ] 11.2 修改 `tests/unittest/agent/src_agent_unit_test.cpp`：`#include "agent/srcagent/src_agent.h"` → `#include "srcagent/src_agent.h"`（通过 include_dirs `$SHARING_ROOT_DIR/services/source/agent` 解析）
- [ ] 11.3 修改 `tests/unittest/agent/agent_dt_test.cpp`：同时引用 sinkagent 和 srcagent，两个 `#include` 都需要调整，BUILD.gn 需同时包含 sink/agent 和 source/agent 的 include_dirs
- [ ] 11.4 修改 `tests/unittest/impl/session/mock/mock_wfd_sink_session.h`：include_dirs 已改为 `sink/impl/wfd/wfd_sink`，`#include` 保持裸名即可
- [ ] 11.5 修改 `tests/unittest/impl/session/mock/mock_wfd_source_session.h`：include_dirs 已改为 `source/impl/wfd/wfd_source`，`#include` 保持裸名即可
- [ ] 11.6 修改 `tests/unittest/impl/rtp/mock/mock_wfd_rtp_consumer.h`：include_dirs 已改为 `sink/impl/wfd/wfd_sink`
- [ ] 11.7 修改 `tests/unittest/impl/scene/mock/mock_wfd_sink_scene.h`：include_dirs 保留 `impl/scene/wfd` 公共路径 + 添加 `sink/impl/scene/wfd`（wfd_sink_scene.h 移到 sink/）
- [ ] 11.8 修改 `tests/unittest/impl/scene/mock/mock_wfd_source_scene.h`：include_dirs 添加 `source/impl/scene/wfd`
- [ ] 11.9 修改 `tests/unittest/impl/scene/wfd_source_scene_dt_test.cpp`：`#include "wfd_source_scene.h"` 通过 include_dirs 从 `source/impl/scene/wfd` 解析
- [ ] 11.10 修改 `tests/unittest/impl/screen_capture/mock/mock_screen_capture_consumer.h`：include_dirs 已改为 `source/impl/screen_capture`
- [ ] 11.11 修改 `tests/unittest/impl/screen_capture/screen_capture_consumer_dt_test.cpp`：`video_source_encoder.h` 通过 `source/codec/include` include_dirs 解析；`video_source_screen.h`、`screen_capture_def.h` 通过 `source/impl/screen_capture` include_dirs 解析
- [ ] 11.12 修改 `tests/unittest/codec/video_audio_sync_test.cpp`：`video_audio_sync.h` 通过 `sink/codec/include` include_dirs 解析
- [ ] 11.13 修改 `tests/unittest/codec/video_source_encoder_test.cpp`：`video_source_encoder.h` 通过 `source/codec/include` include_dirs 解析
- [ ] 11.14 修改 `tests/unittest/codec/audio_pcm_processor_test.cpp`：`audio_pcm_processor.h` 通过 `source/codec/include` include_dirs 解析
- [ ] 11.15 修改 `tests/unittest/serviceplayer/video_play_controller_test.cpp`：`video_play_controller.h` 通过 `sink/mediaplayer/include` include_dirs 解析；`VideoAudioSync` 头文件通过 `sink/codec/include` 解析
- [ ] 11.16 修改 `tests/unittest/context/context_unit_test.cpp`：检查是否引用了 `srcagent/src_agent.h` 或 `sinkagent/sink_agent.h`，如有需调整路径
- [ ] 11.17 修改 `tests/demo/codec/video_sink_decoder_demo.cpp`：`video_sink_decoder.h` 通过 `sink/codec/include` 解析
- [ ] 11.18 检查所有 mock 文件继承移走类的情况：确保 mock 头文件能通过 include_dirs 正确找到被继承的基类头文件

## 12. 编译验证

- [ ] 10.1 在 OH 完整编译环境下验证 `wifi_display_support_sink=true` + `wifi_display_support_source=true`（默认配置）编译通过
- [ ] 10.2 验证 `wifi_display_support_sink=false` + `wifi_display_support_source=true` 编译通过（Sink 模块不编译，codec_factory 中 WFD_SINK 分支不参与编译）
- [ ] 10.3 验证 `wifi_display_support_sink=true` + `wifi_display_support_source=false` 编译通过（Source 模块不编译）
- [ ] 10.4 验证 `sharing_framework_support_wfd=false`（关闭整个 WFD 支持）编译通过
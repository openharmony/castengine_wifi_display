# AGENTS.md

## 项目概述

本仓是 OpenHarmony 音视频投播子系统（castplus）的流媒体能力部件，别名 **Sharing**。主要实现 Miracast（WFD）投屏协议，提供 Sink 端接收与 Source 竂发送能力。语言为 **Native C++**，构建体系为 OpenHarmony GN + ohos。

- 部件名：`sharing_framework`，子系统名：`castplus`
- 进程名：`sharing_service`，SA ID：**5527** / **5528**
- 对外接口头文件：`interfaces/innerkits/native/wfd/include/wfd.h`

## 构建与编译

- 本仓属于 OpenHarmony 源码树的一部分，不能独立编译，需在完整 OH 源码环境下通过 `build.sh` 或 `hb` 编译整系统或单独部件
- 编译命令：./build.sh --product-name <product-name> --build-target sharing_framework --ccache
- 所有 `BUILD.gn` 中的路径均以 `//foundation/CastEngine/castengine_wifi_display` 为根，通过 `config.gni` 的 `SHARING_ROOT_DIR` 变量引用

### GN 编译特性开关

`config.gni` 中定义了三个 `declare_args()`：
- `sharing_framework_support_wfd` — 是否支持 WFD 业务（默认 true）
- `wifi_display_support_sink` — 是否支持 Sink 竂（默认 true），此开关还控制编译警告 cflags 配置
- `wifi_display_support_source` — 是否支持 Source 竂（默认 true）

修改这些开关需在整系统 `args.gn` 中设置，不能在本仓单独改。

### Coverage 开关

`sharing_framework_feature_coverage`（默认 false），在 `tests/BUILD.gn` 中声明，启用后给 cflags/ldflags 加 `--coverage`。

## 代码风格

- `.c lang-format` 文件在根目录，风格为自定义 LLVM 变体：4空格缩进、120列宽、函数体大括号换行（`AfterFunction: true`）、指针靠右（`Right alignment`）、`BreakConstructorInitializers: BeforeColon`
- 编译 cflags 包含 `-Wall -Wextra -Werror`，且有多个 `-Wno-` 豁免（shadow、unused-parameter、missing-field-initializers）
- cflags 有 `-Wno-c++20-extensions`，允许部分 C++20 特性扩展
- sanitize 配置统一启用：cfi、boundary_sanitize、ubsan、integer_overflow

## 架构要点

本仓为 C/S 架构，服务端单进程 `sharing_service`，客户端通过 IPC 代理访问：

- **服务端入口**：`services/` — 包含 Interaction、Scene、ContextMgr、Agent、MediaChannel、Codec、Network、Protocol 等模块，共同组成流媒体框架
- **客户端框架层**：`frameworks/innerkitsimpl/native/wfd/` — WfdSinkImpl / WfdSourceImpl / WifiDisplay 实现，依赖 IPC 与服务端通信
- **JS NAPI 层**：`frameworks/kitsimpl/js/wfd/` — 将客户端能力封装为 JS 接口
- **关键 IPC 路径**：`services/interaction/interprocess/` — 客户端代理/服务端 stub 定义，IPC codec 在 `services/interaction/ipc_codec/`
- **WFD 业务实现**：`services/impl/wfd/` — WFD Source/Sink Session、RTP Consumer/Producer；`services/impl/scene/wfd/` 为 WFD Scene

服务端依赖链：`sharing_service` → agent, configuration, context, event, interaction, mediachannel, mediaplayer, scheduler → codec, network, protocol(rtp/rtsp), utils, common

## 测试

- 单元测试入口：`tests/unittest:wfd_unit_test`，当前仅包含 `network:network_unittest`
- 模块级单元测试分布在 `tests/unittest/` 下的各子目录（agent、codec、context、event、interaction、mediachannel、mediaplayer、protocol、scheduler、impl 等），每个子目录有独立 `BUILD.gn`
- Fuzz 测试入口：`tests/fuzztest/sink_fuzzer:wfd_sink_fuzz_test`，分为 sink_fuzzer 和 source_fuzzer 两类
- Demo 测试入口：`tests/demo:demo_test`
- 所有测试需要在 OH 整系统编译环境下构建，无法在本仓独立运行

## 重要约束

- **命名空间**：所有代码使用 `OHOS::Sharing`
- **IPC 类型**：服务端使用 `ipc_single`（非 ipc_core），客户端使用 `ipc_core + ipc_single`
- **进程权限**：`sharing_service` 运行于 `audio` UID，需要 `CAPTURE_SCREEN`、`DISTRIBUTED_DATASYNC`、`CAMERA`、`MICROPHONE` 等高权限
- **数据目录**：服务启动时创建 `/data/service/el1/public/database/sharingcodec/` 及 `/data/service/el1/public/sharing_service/`
- **第三方依赖**：ffmpeg、openssl、cJSON、jsoncpp（注意：bundle.json 中 deps.components 和 deps.third_party 重复列出了 cJSON/jsoncpp/openssl/ffmpeg）
- **magic_enum**：`services/extend/magic_enum/magic_enum.hpp` 为 header-only 库，被 include_dirs 直接引用而非编译为独立 target
- **hisysevent**：定义了 SHARING 和 WIFI_DISPLAY 两个 domain 的打点事件，配置在 `hisysevent.yaml` 和 `hisysevent_sink.yaml`
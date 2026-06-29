## ADDED Requirements

### Requirement: MediaChannel 管理生产者和消费者生命周期
`MediaChannel` SHALL 基于代理命令创建、启动、停止、暂停、恢复和销毁生产者和消费者。

#### Scenario: 创建生产者
- **WHEN** 收到创建生产者命令
- **THEN** 系统 SHALL 实例化生产者类（如 WfdRtpProducer、ScreenCaptureConsumer 作为生产端）并注册到生产者映射表

#### Scenario: 创建消费者
- **WHEN** 收到创建消费者命令
- **THEN** 系统 SHALL 实例化消费者类（如 WfdRtpConsumer）并注册为活动消费者

#### Scenario: 启动生产者和消费者
- **WHEN** 收到生产者和消费者的启动命令
- **THEN** 系统 SHALL 通过 BufferDispatcher 启动从生产者到消费者的媒体数据流

#### Scenario: 停止和销毁
- **WHEN** 收到停止和销毁命令
- **THEN** 系统 SHALL 停止所有生产者和消费者、释放编解码资源并清理缓冲区调度

### Requirement: WfdRtpProducer 从源传输 RTP 数据
`WfdRtpProducer` SHALL 将屏幕采集数据编码为 MPEG2-TS，打包为 RTP 数据包，通过 UDP 传输到端设备。

#### Scenario: 初始化和启动 RTP 生产者
- **WHEN** WfdRtpProducer 使用目标 IP 和端口初始化
- **THEN** 系统 SHALL 创建用于 RTP 和 RTCP 数据传输的 UDP 客户端、初始化 TS 模式负载的 RTP 打包器并启动媒体数据调度线程

#### Scenario: 传输编码帧
- **WHEN** 从屏幕采集管道收到编码的视频/音频帧
- **THEN** 系统 SHALL 将帧打包为 RTP 数据包并通过 UDP 发送到端的 RTP 端口

#### Scenario: RTCP 超时检测
- **WHEN** 在配置的超时时间内未收到来自端的 RTCP 反馈
- **THEN** 系统 SHALL 向代理报告超时通知

### Requirement: WfdRtpConsumer 在端接收 RTP 数据
`WfdRtpConsumer` SHALL 通过 UDP 从源接收 RTP 数据包，解包为帧并分发到 BufferDispatcher 进行解码。

#### Scenario: 初始化和启动 RTP 消费者
- **WHEN** WfdRtpConsumer 使用本地 RTP/RTCP 端口初始化
- **THEN** 系统 SHALL 创建用于 RTP 和 RTCP 接收的 UDP 服务器、初始化 RTP 解包器并开始处理入站 RTP 数据包

#### Scenario: 接收并解包 RTP 数据包
- **WHEN** 从源收到 RTP 数据包
- **THEN** 系统 SHALL 解包 RTP 负载为帧、检测关键帧、缓存 H.264 的 SPS/PPS NALU 并将帧分发到 BufferDispatcher

### Requirement: BufferDispatcher 管理媒体数据的环形缓冲区
`BufferDispatcher` SHALL 维护存储 GOP 感知媒体数据的环形缓冲区，并将数据分发给多个 Surface 读取器。

#### Scenario: 写入帧到缓冲区
- **WHEN** 解码或接收的帧写入 BufferDispatcher
- **THEN** 系统 SHALL 将帧存储在环形缓冲区中，附带时间戳和关键帧元数据，维护 GOP 边界

#### Scenario: 从缓冲区为 Surface 读取帧
- **WHEN** Surface 读取器向 BufferDispatcher 请求数据
- **THEN** 系统 SHALL 返回下一个可用帧，处理关键帧模式（仅关键帧）、关键帧重定向（快速跳到关键帧）和加速模式

#### Scenario: 缓冲区溢出
- **WHEN** 环形缓冲区达到最大容量（800帧，可配置）
- **THEN** 系统 SHALL 丢弃最旧的帧，同时保留至少一个 GOP 起始（关键帧 + 后续帧）

### Requirement: VideoSinkDecoder 为端播放解码视频
`VideoSinkDecoder` SHALL 使用 OHOS 硬件解码器（AVCodecVideoDecoder）解码入站视频帧并在 Surface 上渲染。

#### Scenario: 使用编解码 ID 初始化解码器
- **WHEN** VideoSinkDecoder 使用编解码 ID（H264/H265）初始化
- **THEN** 系统 SHALL 用编解码格式配置 OHOS AVCodecVideoDecoder 并准备 Surface 模式或缓冲区模式解码

#### Scenario: 设置渲染 Surface
- **WHEN** Surface 追加到端
- **THEN** 系统 SHALL 在 VideoSinkDecoder 上设置 Surface 用于直接 Surface 模式渲染

#### Scenario: 解码帧
- **WHEN** 从 BufferDispatcher 收到视频帧
- **THEN** 系统 SHALL 将帧提交到硬件解码器并在配置的 Surface 上渲染解码输出

#### Scenario: 解码器加速通知
- **WHEN** 硬件解码器完成加速操作
- **THEN** 系统 SHALL 向客户端发送 decoderAccelerationDone 通知

### Requirement: VideoSourceEncoder 为源编码屏幕采集
`VideoSourceEncoder` SHALL 使用 OHOS 硬件编码器（AVCodecVideoEncoder）编码采集的屏幕数据用于 RTP 传输。

#### Scenario: 使用配置初始化编码器
- **WHEN** VideoSourceEncoder 使用 VideoSourceConfigure（分辨率、帧率、codecType）初始化
- **THEN** 系统 SHALL 配置 OHOS AVCodecVideoEncoder并提供 Surface 用于屏幕采集输入

#### Scenario: 编码并输出 IDR 帧
- **WHEN** 编码器产出包含 SPS/PPS 的 IDR（关键帧）
- **THEN** 系统 SHALL 将编码帧分发给 WfdRtpProducer，缓存 SPS/PPS 用于后续帧前置

#### Scenario: 编码器错误通知
- **WHEN** 硬件编码器遇到错误
- **THEN** 系统 SHALL 发送 errorMsg 通知并停止编码管道

### Requirement: 音频编码器和解码器
系统 SHALL 在源端支持音频编码（AAC、G.711A/U），在端端支持解码。

#### Scenario: 源端音频编码
- **WHEN** 收到采集的音频 PCM 数据
- **THEN** 系统 SHALL 使用协商的音频编解码（AAC 或 G.711）编码并通过 RTP 传输

#### Scenario: 端端音频解码
- **WHEN** 接收的音频 RTP 负载被解包
- **THEN** 系统 SHALL 用匹配的音频解码器解码并通过 OHOS AudioRenderer 渲染

### Requirement: 音视频同步
`VideoAudioSync` SHALL 在端端同步视频和音频播放。

#### Scenario: 视频比音频快
- **WHEN** 视频帧提前于音频时间到达
- **THEN** 系统 SHALL 保持视频帧直到音频赶上，在可配置的容差内维持唇音同步

#### Scenario: 音频比视频快
- **WHEN** 音频提前于视频时间到达
- **THEN** 系统 SHALL 丢弃或跳过音频包以维持同步

### Requirement: 媒体播放控制器
`MediaController` SHALL 管理音频和视频播放控制器，协调所有 Surface 的启动/停止/暂停/恢复。

#### Scenario: 在所有 Surface 上启动播放
- **WHEN** 收到播放命令
- **THEN** 系统 SHALL 启动 AudioPlayController 和所有 VideoPlayController，开始同步渲染
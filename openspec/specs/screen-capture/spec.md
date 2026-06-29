## ADDED Requirements

### Requirement: ScreenCaptureSession 管理屏幕和音频采集
`ScreenCaptureSession` SHALL 协调 WFD 源管道的屏幕视频采集和音频采集。

#### Scenario: 初始化屏幕采集会话
- **WHEN** 为源代理创建 ScreenCaptureSession
- **THEN** 系统 SHALL 准备屏幕采集和音频采集资源，创建 ScreenCaptureConsumer 和 AudioSourceCapturer

#### Scenario: 启动屏幕采集
- **WHEN** WFD 源会话转换为播放状态
- **THEN** ScreenCaptureSession SHALL 启动 `VideoSourceScreen` 进行屏幕采集和 `AudioSourceCapturer` 进行音频采集，将数据馈入编码器

#### Scenario: 停止屏幕采集
- **WHEN** WFD 源会话被拆除
- **THEN** ScreenCaptureSession SHALL 停止屏幕和音频采集、释放编码器资源并清理

### Requirement: VideoSourceScreen 采集显示帧
`VideoSourceScreen` SHALL 使用 OHOS `DisplayManager` 采集屏幕帧并通过 Surface 提供给硬件编码器。

#### Scenario: 采集屏幕帧
- **WHEN** 屏幕采集活动时
- **THEN** `VideoSourceScreen` SHALL 通过 `DisplayManager::CreateVirtualScreen` 采集显示内容，并将 Surface 作为输入提供给 `VideoSourceEncoder`

#### Scenario: 采集配置
- **WHEN** 屏幕采集初始化
- **THEN** 系统 SHALL 基于 WFD 协商的视频格式配置采集分辨率、帧率和显示 ID

### Requirement: AudioSourceCapturer 采集音频
`AudioSourceCapturer` SHALL 使用 OHOS `AudioCapturer` 采集系统音频并馈入音频编码器。

#### Scenario: 启动音频采集
- **WHEN** 音频采集启动
- **THEN** `AudioSourceCapturer` SHALL 用协商的音频格式（采样率、通道数、位深）初始化 OHOS `AudioCapturer` 并开始采集

#### Scenario: 采集音频帧
- **WHEN** 音频帧被采集
- **THEN** 系统 SHALL 用配置的音频编码器（AAC 或 G.711）编码并分发给 WfdRtpProducer

### Requirement: ScreenCaptureConsumer 协调编码管道
`ScreenCaptureConsumer` SHALL 在源端协调屏幕采集 → 视频编码 → RTP 传输管道。

#### Scenario: 编码管道设置
- **WHEN** ScreenCaptureConsumer 初始化
- **THEN** 系统 SHALL 创建 `VideoSourceScreen`（采集源）、`VideoSourceEncoder`（硬件编码器），并连接为管道：Screen → Encoder Surface → Encoder → WfdRtpProducer

#### Scenario: SPS/PPS 帧处理
- **WHEN** VideoSourceEncoder 输出包含 SPS/PPS NALU 的 IDR 帧
- **THEN** ScreenCaptureConsumer SHALL 缓存 SPS/PPS 并在分发给 WfdRtpProducer 前将其前置到后续 IDR 帧

#### Scenario: 音频编码管道
- **WHEN** 音频采集活动
- **THEN** ScreenCaptureConsumer SHALL 协调音频编码：AudioSourceCapturer → AudioEncoder（AAC）→ WfdRtpProducer（音频 RTP）

### Requirement: 屏幕采集权限
系统 SHALL 要求 `CAPTURE_SCREEN` 权限用于屏幕采集和 `MICROPHONE` 权限用于音频采集。

#### Scenario: 缺少屏幕采集权限
- **WHEN** 没有 `CAPTURE_SCREEN` 权限时尝试屏幕采集
- **THEN** 系统 SHALL 初始化失败并发送 errorMsg 通知

#### Scenario: 缺少麦克风权限
- **WHEN** 没有 `MICROPHONE` 权限时尝试音频采集
- **THEN** 系统 SHALL 仅进行视频投屏（无音频通道）
## ADDED Requirements

### Requirement: 源场景初始化 P2P 发现
`WfdSourceScene` SHALL 管理 Wi-Fi P2P 设备发现生命周期，包括启动/停止发现和处理发现的设备事件。

#### Scenario: 启动 P2P 发现
- **WHEN** 收到 startDiscovery IPC 消息
- **THEN** 系统 SHALL 通过 DeviceManager 调用 Wi-Fi P2P startDiscovery 并开始监听 P2P 设备发现事件

#### Scenario: P2P 设备发现通知
- **WHEN** 在发现期间收到 Wi-Fi P2P 设备发现事件
- **THEN** 系统 SHALL 构造包含 deviceId、ipAddr、deviceName 和设备类型的 `WfdCastDeviceInfo`，并通过 `OnDeviceFound` 通知发送给客户端

#### Scenario: 停止 P2P 发现
- **WHEN** 收到 stopDiscovery IPC 消息
- **THEN** 系统 SHALL 调用 Wi-Fi P2P stopDiscovery 并停止处理设备发现事件

### Requirement: 源场景管理设备连接
`WfdSourceScene` SHALL 管理 P2P 与所选端设备的连接，包括连接建立和 WFD 会话创建。

#### Scenario: 添加设备连接
- **WHEN** 收到包含已发现设备信息的 addDevice IPC 消息
- **THEN** 系统 SHALL 发起 P2P 连接到该设备，等待 P2P 连接事件，然后创建带有 WfdSourceSession 的源代理进行 WFD RTSP 协商

#### Scenario: 连接超时
- **WHEN** P2P 连接在配置的超时时间内未完成
- **THEN** 系统 SHALL 取消连接尝试并通知客户端连接失败

#### Scenario: P2P 连接断开
- **WHEN** 在活动 WFD 会话期间 P2P 连接丢失
- **THEN** 系统 SHALL 拆除 WFD 会话、销毁源代理并通知客户端连接状态变更为 DISCONNECTED

### Requirement: 源场景移除设备
`WfdSourceScene` SHALL 通过断开 P2P 和销毁相关资源来处理设备移除。

#### Scenario: 移除设备
- **WHEN** 收到包含已连接设备 ID 的 removeDevice IPC 消息
- **THEN** 系统 SHALL 断开 P2P 与设备的连接、销毁源代理及其 WFD 会话并停止屏幕采集

### Requirement: 源场景创建屏幕采集
`WfdSourceScene` SHALL 在 WFD 会话进入播放状态时创建屏幕采集源和编码器管道。

#### Scenario: 启动屏幕采集进行投屏
- **WHEN** WFD 源会话完成 M7（PLAY）协商并转换为播放状态
- **THEN** 系统 SHALL 创建带有 ScreenCaptureConsumer 和 VideoSourceEncoder 的 ScreenCaptureSession，并将其链接到 WfdRtpProducer 进行媒体传输

### Requirement: 源场景处理 IPC 请求
`WfdSourceScene` SHALL 处理 startDiscovery、stopDiscovery、addDevice 和 removeDevice 的 IPC 请求，路由到相应的 P2P 和会话管理。

#### Scenario: IPC 请求路由
- **WHEN** 收到带有 WfdSource 特定 MsgId 的 IPC 消息
- **THEN** 系统 SHALL 基于 MsgId 分发到相应处理程序（startDiscovery → P2P 发现、addDevice → P2P 连接等）

### Requirement: 源场景防止并发投屏
一个 WfdSource SHALL 同时只能向一个设备投屏。一个设备不能同时作为源和端。

#### Scenario: 尝试添加第二个设备
- **WHEN** addDevice 在已向另一设备投屏时调用
- **THEN** 系统 SHALL 拒绝请求或断开第一个设备后再连接新设备
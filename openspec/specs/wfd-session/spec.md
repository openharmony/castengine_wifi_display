## ADDED Requirements

### Requirement: WFD 源会话 RTSP 服务器
`WfdSourceSession` SHALL 在 WFD 控制端口（默认7236）上运行 TCP RTSP 服务器，接受端设备的入站连接。

#### Scenario: 源会话启动 RTSP 服务器
- **WHEN** WfdSourceSession 被初始化
- **THEN** 系统 SHALL 在配置的控制端口上创建 TCP 服务器并监听端设备连接

#### Scenario: 端设备连接到源 RTSP 服务器
- **WHEN** 端设备建立与源 RTSP 服务器之间的 TCP 连接
- **THEN** 系统 SHALL 接受连接、创建网络会话用于 RTSP 消息交换，并开始 WFD 协商

### Requirement: WFD 源会话 M1-M8 状态机
`WfdSourceSession` SHALL 遵循 WFD RTSP M1-M8 协议状态机进行会话建立和拆除。

#### Scenario: M1 OPTIONS 交换
- **WHEN** 端设备连接到源 RTSP 服务器
- **THEN** 源 SHALL 发送 M1 OPTIONS 请求（`org.wfa.wfd1.0`）并期望端返回 M2 OPTIONS 响应

#### Scenario: M3 GET_PARAMETER 能力查询
- **WHEN** 收到 M2 响应确认端支持 WFD
- **THEN** 源 SHALL 发送 M3 GET_PARAMETER 请求查询端的视频格式、音频编解码和 RTP 端口

#### Scenario: M4 SET_PARAMETER 协商参数
- **WHEN** 收到包含端能力的 M3 响应
- **THEN** 源 SHALL 发送 M4 SET_PARAMETER 请求，基于源和端能力的交集设置协商的视频和音频格式

#### Scenario: M5 TRIGGER 建立触发
- **WHEN** M4 响应确认参数协商完成
- **THEN** 源 SHALL 发送 M5 SET_PARAMETER 触发命令发起 SETUP

#### Scenario: M6 SETUP 媒体传输
- **WHEN** 端发送包含 RTP 端口信息的 SETUP 请求
- **THEN** 源 SHALL 以 SETUP OK 响应，包含源的 RTP/RTCP 端口信息，建立媒体传输参数

#### Scenario: M7 PLAY 开始流传输
- **WHEN** 端发送 PLAY 请求
- **THEN** 源 SHALL 以 PLAY OK 响应，启动 WfdRtpProducer 进行 RTP/RTCP 数据传输，并开始屏幕采集和编码

#### Scenario: M8 TEARDOWN 结束会话
- **WHEN** 收到或发送 TEARDOWN 请求
- **THEN** 源 SHALL 停止 RTP 生成、释放屏幕采集资源并关闭 RTSP TCP 连接

### Requirement: WFD 源会话保活（M16）
`WfdSourceSession` SHALL 在活动会话期间定期发送 M16 GET_PARAMETER 保活消息。

#### Scenario: 活动会话期间的保活
- **WHEN** WFD 会话处于播放状态
- **THEN** 源 SHALL 定期发送 GET_PARAMETER 保活消息维持会话，并在端未响应时检测超时

#### Scenario: 保活超时
- **WHEN** 端在配置的超时时间内未响应保活消息
- **THEN** 源 SHALL 拆除会话并通知客户端连接丢失

### Requirement: WFD 端会话 RTSP 客户端
`WfdSinkSession` SHALL 运行 TCP RTSP 客户端连接到源的 RTSP 服务器进行 WFD 协商。

#### Scenario: 端会话连接到源
- **WHEN** WfdSinkSession 使用源 IP 和端口初始化
- **THEN** 系统 SHALL 创建到源 RTSP 服务器的 TCP 客户端连接并开始 WFD 协商

### Requirement: WFD 端会话状态机
`WfdSinkSession` SHALL 遵循状态机：INIT → READY → PLAYING → STOPPING。

#### Scenario: INIT 到 READY 转换
- **WHEN** 端连接到源并完成 M1/M2 OPTIONS 交换及 M3/M4 参数协商
- **THEN** 端会话 SHALL 从 INIT 状态转换为 READY 状态

#### Scenario: READY 到 PLAYING 转换
- **WHEN** 端发送 M6 SETUP 和 M7 PLAY 请求并收到正面响应
- **THEN** 端会话 SHALL 从 READY 状态转换为 PLAYING 状态，启动 WfdRtpConsumer 接收 RTP 数据并初始化解码

#### Scenario: PLAYING 到 STOPPING 转换
- **WHEN** 收到 TEARDOWN 或端会话检测到超时/错误
- **THEN** 端会话 SHALL 转换到 STOPPING 状态，停止 RTP 接收并关闭 TCP 连接

### Requirement: WFD 端会话保活
`WfdSinkSession` SHALL 在活动会话期间定期发送 OPTIONS 保活消息。

#### Scenario: 端保活
- **WHEN** 端会话处于 PLAYING 状态
- **THEN** 端 SHALL 定期发送 OPTIONS 请求作为保活，并在源未响应时检测会话超时

### Requirement: WFD 会话 IDR 请求（M13）
系统 SHALL 支持 M13 IDR（即时解码器刷新）请求，用于从源请求关键帧。

#### Scenario: 请求 IDR 关键帧
- **WHEN** 端需要关键帧（如 Surface 变换或解码器重置后）
- **THEN** 端 SHALL 通过 SET_PARAMETER 发送 IDR 请求，源 SHALL 编码并发送 IDR 帧

### Requirement: WFD 源处理端的 RTSP 请求
`WfdSourceSession` SHALL 处理来自端的入站 RTSP 请求（OPTIONS、SETUP、PLAY、PAUSE、TEARDOWN、GET_PARAMETER、SET_PARAMETER）。

#### Scenario: 处理入站 SETUP 请求
- **WHEN** 源收到来自端包含 RTP 端口信息的 SETUP 请求
- **THEN** 源 SHALL 以 SETUP OK 响应，包含源 RTP/RTCP 端口和会话 ID

#### Scenario: 处理入站 PLAY 请求
- **WHEN** 源收到 PLAY 请求
- **THEN** 源 SHALL 以 PLAY OK 响应并开始流传输媒体数据
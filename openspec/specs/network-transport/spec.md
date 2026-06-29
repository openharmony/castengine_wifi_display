## ADDED Requirements

### Requirement: NetworkFactory 创建 TCP/UDP 服务器和客户端
`NetworkFactory` SHALL 提供静态工厂方法创建 TCP 和 UDP 服务器及客户端实例。

#### Scenario: 创建 TCP 服务器
- **WHEN** `NetworkFactory::CreateTcpServer(callback)` 被调用
- **THEN** 系统 SHALL 返回配置了所提供回调的 `TcpServer` 实例，用于连接和数据事件

#### Scenario: 创建 TCP 客户端
- **WHEN** `NetworkFactory::CreateTcpClient(callback)` 被调用
- **THEN** 系统 SHALL 返回配置了所提供回调的 `TcpClient` 实例

#### Scenario: 创建 UDP 服务器
- **WHEN** `NetworkFactory::CreateUdpServer(callback)` 被调用
- **THEN** 系统 SHALL 返回 `UdpServer` 实例

#### Scenario: 创建 UDP 客户端
- **WHEN** `NetworkFactory::CreateUdpClient(callback)` 被调用
- **THEN** 系统 SHALL 返回 `UdpClient` 实例

### Requirement: TCP 服务器接受连接和管理会话
`TcpServer` SHALL 在指定端口监听、接受入站 TCP 连接并管理每连接的会话。

#### Scenario: 启动 TCP 服务器
- **WHEN** `TcpServer::Start(port)` 被调用
- **THEN** 系统 SHALL 在指定端口绑定并监听，当客户端连接时通过 `OnAccept` 回调通知

#### Scenario: 在 TCP 会话上接收数据
- **WHEN** 在已接受的 TCP 连接上收到数据
- **THEN** 系统 SHALL 通过 `OnServerReadData` 回调通知数据和关联的会话引用

#### Scenario: 关闭客户端套接字
- **WHEN** `TcpServer::CloseClientSocket(socketId)` 被调用
- **THEN** 系统 SHALL 关闭指定客户端连接并释放会话

### Requirement: TCP 客户端连接和通信
`TcpClient` SHALL 连接到远程 TCP 服务器并发送/接收数据。

#### Scenario: 连接到远程服务器
- **WHEN** `TcpClient::Connect(host, port)` 被调用
- **THEN** 系统 SHALL 建立 TCP 连接并通过 `OnClientConnect` 回调通知

#### Scenario: 通过 TCP 客户端发送数据
- **WHEN** `TcpClient::Send(data, size)` 在连接状态下调用
- **THEN** 系统 SHALL 通过 TCP 连接发送数据

#### Scenario: 通过 TCP 客户端接收数据
- **WHEN** 从远程服务器收到数据
- **THEN** 系统 SHALL 通过 `OnClientReadData` 回调通知

#### Scenario: TCP 连接被远程关闭
- **WHEN** 远程服务器关闭连接
- **THEN** 系统 SHALL 通过 `OnClientClose` 回调通知

#### Scenario: TCP 连接错误
- **WHEN** TCP 连接上发生网络错误
- **THEN** 系统 SHALL 通过 `OnClientException` 回调通知

### Requirement: UDP 服务器接收数据报
`UdpServer` SHALL 在指定端口监听并接收 UDP 数据报。

#### Scenario: 启动 UDP 服务器
- **WHEN** `UdpServer::Start(port)` 被调用
- **THEN** 系统 SHALL 绑定到指定端口并开始接收 UDP 数据报

#### Scenario: 接收 UDP 数据报
- **WHEN** 收到 UDP 数据报
- **THEN** 系统 SHALL 通过 `OnServerReadData` 回调通知数据报数据和源地址

### Requirement: UDP 客户端发送和接收数据报
`UdpClient` SHALL 向远程端点发送 UDP 数据报并接收响应。

#### Scenario: 连接并通过 UDP 发送
- **WHEN** `UdpClient::Connect(host, port)` 和 `UdpClient::Send(data, size)` 被调用
- **THEN** 系统 SHALL 向指定远程端点发送 UDP 数据报

#### Scenario: 接收 UDP 响应
- **WHEN** 从远程端点收到 UDP 响应数据报
- **THEN** 系统 SHALL 通过 `OnClientReadData` 回调通知

### Requirement: 网络会话管理
`NetworkSessionManager` SHALL 管理活动网络会话，跟踪连接状态并提供会话查询。

#### Scenario: 接受时创建会话
- **WHEN** TCP 服务器接受新连接
- **THEN** 系统 SHALL 创建包装连接的 `TcpSession` 并注册到会话管理器

### Requirement: 套接字信息跟踪
系统 SHALL 通过 `SocketInfo` 跟踪套接字元数据，包括文件描述符、本地地址、对端地址和端口。

#### Scenario: 获取套接字信息
- **WHEN** 在客户端或服务器上调用 `GetSocketInfo()`
- **THEN** 系统 SHALL 返回包含 fd、localAddr、peerAddr 和 port 的 `SocketInfo`

### Requirement: EventHandler 集成用于异步 I/O
网络模块 SHALL 通过 `EventDescriptorListener` 使用 OHOS `EventHandler` 进行异步 I/O 通知。

#### Scenario: 异步读取通知
- **WHEN** 套接字文件描述符上有数据可用
- **THEN** `EventDescriptorListener` SHALL 检测事件并在事件处理线程上触发读取回调

### Requirement: UDP 端口范围配置
系统 SHALL 从可配置范围（默认6700-7000）分配 UDP 端口，如 `sharing_config.json` 所指定。

#### Scenario: 从范围分配 UDP 端口
- **WHEN** UDP 服务器或客户端需要端口
- **THEN** 系统 SHALL 从配置的 UDP 端口范围分配
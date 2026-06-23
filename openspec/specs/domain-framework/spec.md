## ADDED Requirements

### Requirement: DomainManager 管理跨设备对端
`DomainManager` SHALL 为单例，通过 `ITransmitMgr` 实现管理跨设备通信对端。

#### Scenario: 域管理器初始化
- **WHEN** DomainManager 单例第一次访问
- **THEN** 系统 SHALL 初始化 RPC 传输管理器并准备跨设备连接

#### Scenario: 注册域对端
- **WHEN** 发现或连接远程设备
- **THEN** DomainManager SHALL 创建连接到远程设备 `DomainRpcService` SA 的 `DomainRpcClient`

### Requirement: DomainRpcService 支持跨设备 SA 访问
`DomainRpcService` SHALL 为每个设备上的系统能，允许远程设备访问本地 Sharing 服务。

#### Scenario: 远程设备连接到本地 DomainRpcService
- **WHEN** 远程设备连接到本地 `DomainRpcService`
- **THEN** 系统 SHALL 接受连接、为远程监听器创建代理并启用双向 RPC 通信

#### Scenario: 远程 DoRpcCommand
- **WHEN** 远程设备调用 `IDomainRpcService::DoRpcCommand(msg, replyMsg)`
- **THEN** 系统 SHALL 解码 `BaseDomainMsg`、路由到本地 `InteractionManager` 并返回响应

### Requirement: BaseDomainMsg 为跨设备扩展 BaseMsg
`BaseDomainMsg` SHALL 在 `BaseMsg` 基础上添加跨设备路由字段：`fromDevId`、`toDevId`、`fromRpcKey`、`toRpcKey` 和 `pullUpApp`。

#### Scenario: 跨设备消息路由
- **WHEN** `BaseDomainMsg` 从设备 A 发送到设备 B
- **THEN** 消息 SHALL 包含源设备 ID（`fromDevId`）、目标设备 ID（`toDevId`）、源 RPC 密钥（`fromRpcKey`）和目标 RPC 密钥（`toRpcKey`）用于路由

#### Scenario: 拉起远程应用
- **WHEN** `pullUpApp = true` 的 `BaseDomainMsg` 发送到远程设备
- **THEN** 远程设备上的 DomainRpcService SHALL 在处理命令前尝试通过 `DeviceManager` 启动 Sharing 应用

### Requirement: InteractionManager 桥接域消息
`InteractionManager` SHALL 在本地交互和远程设备之间桥接域消息。

#### Scenario: 发送域消息
- **WHEN** Interaction 需要向远程设备发送消息
- **THEN** `InteractionManager::SendDomainMsg` SHALL 通过 `DomainManager::SendDomainRequest` 路由消息到相应的 `ITransmitMgr`

#### Scenario: 接收域消息
- **WHEN** 从远程设备收到域消息
- **THEN** `DomainManager::OnDomainRequest` SHALL 路由消息到 `InteractionManager::OnDomainMsg`，后者 SHALL 转发到相应的 Interaction

### Requirement: 域 RPC 传输类型
域框架 SHALL 支持 RPC 作为跨设备通信的传输类型（DomainType::RPC）。

#### Scenario: RPC 传输创建
- **WHEN** DomainManager 需要为远程设备创建传输管理器
- **THEN** 系统 SHALL 创建带有 `DomainRpcService` 的 `DomainRpcManager` 用于双向 RPC 通信

### Requirement: 域消息与交互层集成
域框架 SHALL 通过 `Interaction::OnDomainMsg` 和 `Interaction::ForwardDomainMsg` 与交互层集成。

#### Scenario: 转发域消息到场景
- **WHEN** 域消息到达 Interaction
- **THEN** Interaction SHALL 调用 `BaseScene::OnDomainMsg` 进行场景特定处理
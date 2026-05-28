# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Communication Language

Please respond in Chinese (Simplified) when discussing code, architecture, and development tasks. All explanations, suggestions, and code reviews should be written in Chinese.

## Project Overview

This is a **Mu Online private server** emulator based on IGCN / X-Team 系列的 GameServer 修改版。包含 6 个 C++ (Win32) 服务器组件、一个 OpenGL 游戏客户端 (Main)、以及一个启动器配置打包工具 (GetMainInfo)。支持 Season 4/6/8 切换。

### 组件清单

| # | 组件 | 源文件数 | 端口 | 说明 |
|---|------|---------|------|------|
| 1 | **ConnectServer** | ~40 文件 | TCP:63000, UDP:63001 | 客户端入口，服务器列表分发 |
| 2 | **DataServer** | ~70 文件 | TCP:63002 | SQL 数据库中间层 (ODBC) |
| 3 | **JoinServer** | ~40 文件 | TCP:63003 | 账号认证与会话管理 |
| 4 | **GameServer** | ~450 文件 | 55901~55999 | 核心游戏逻辑 |
| 5 | **Main (客户端)** | ~760 文件 | — | 游戏客户端 (OpenGL) |
| 6 | **GetMainInfo** | ~20 文件 | — | 启动器配置打包工具 |
| — | **Util** | 工具库 | — | CRC32/MD5/Crypto++/Lua/Detours |

### 完整连接流程

```
客户端 → ConnectServer (:63000)
  → 获取服务器列表 (C1:F4:06)
  → 选择服务器获取 GS IP:Port (C1:F4:03)
  → GameServer (:55901~55999)
    → JoinServer 账号认证 (ODBC → SQL)
    → DataServer 角色数据读写 (ODBC → SQL)
    → Antihack (:55858) 反外挂监控
```

### 数据流

```
.ini/.txt 配置文件
  → GetMainInfo.exe (编译 + XOR 混淆加密)
  → ClientFile/Data/Local/CBGetMain.bin (主配置)
  → ClientFile/Data/Local/CBTextInfo.bin (提示文本)
  → ClientFile/Data/Local/License.json (反外挂配置)
  → Main.exe 客户端读取解析
```

## Build System

- **IDE**: Visual Studio 2019+ (`.vcxproj` + `.sln` per component)
- **Platform**: Win32 (x86) only
- **Toolset**: v143 for Main client, v100 for servers
- **Build via command line**:
  ```
  msbuild Source\4.GameServer\GameServer.sln /p:Configuration=Release_EX803 /p:Platform=Win32
  ```
- **Build configurations** (GameServer): `Release_EX401` (Season 4), `Release_EX603` (Season 6), `Release_EX803` (Season 8), each with `CS` variants (Castle Siege). Debug configs also available.
- **Build configurations** (Main/Client): `Global Release`, `Global Debug`
- **Dependencies**: Boost 1.80.0 (in-tree), DirectX 9 SDK (in-tree), Crypto++, detours, Lua, pugixml

## Season Versioning

GameServer 使用预处理器宏切换版本和语言。定义位于 `stdafx.h` 和 `.vcxproj` 配置：
```cpp
GAMESERVER_UPDATE=401  // Season 4
GAMESERVER_UPDATE=603  // Season 6
GAMESERVER_UPDATE=803  // Season 8
```
版本特定代码通过 `#if(GAMESERVER_UPDATE>=XXX)` 守卫。

## 语言宏 GAMESERVER_LANGUAGE

`GAMESERVER_LANGUAGE` 控制移动/攻击等协议的**封包头字节码**，定义在 `Protocol.h:8-28`。

| 值 | 协议码 (CODE1~4) | 对应客户端区域 |
|----|-------------------|---------------|
| 0  | 0xD3/0xD7/0xDF/0x10 | 韩/日/英语(旧) |
| 1  | 0xD4/0x11/0x15/0xDB | 国际服(默认) |
| 2  | 0x1D/0xDC/0xD6/0xD7 | 越南/其他 |
| 3  | 0xD9/0xD7/0xD0/0x1D | **中国 (CHN)** |
| 4+ | 自定义预留 | — |

**当前设定**：
- `Release_EX603` → 必须使用 `GAMESERVER_LANGUAGE=3`（匹配中文客户端）
- `Release_EX803` → 使用 `GAMESERVER_LANGUAGE=1`（国际服）
- `Release_EX401` → 使用 `GAMESERVER_LANGUAGE=0`（旧版）

> **注意**：Release_EX603 配置在 `.vcxproj` 中默认为 `GAMESERVER_LANGUAGE=1`，若对接中文客户端需改为 `3`。否则客户端发送的 0xD9/0xD7 等协议头无法被服务器识别。

---

## GameServer 深度分析 (`Source/4.GameServer/GameServer/`)

### 主循环架构

入口 `GameServer.cpp` WinMain 执行顺序：
1. MiniDump 崩溃转储初始化
2. 加载 `GameServerInfo - Common.ini`
3. WSAStartup (Winsock 2.2)
4. IOCP Socket 管理器启动监听
5. `GameMainInit(hWnd)` 游戏主初始化
6. 连接 JoinServer / DataServer
7. 创建 11 个队列定时器
8. 进入 Windows 消息循环

### 队列定时器系统

所有游戏逻辑在 `QueueTimerCallback` 中轮询执行，临界区保护：

| 周期 | 定时器 | 功能 |
|------|--------|------|
| 100ms | MONSTER / MONSTER_MOVE / MONSTER_AI / MONSTER_AI_MOVE / EVENT | 怪物消息/移动/AI/事件轮询 |
| 1000ms | VIEWPORT / FIRST / CLOSE | 视口更新/复活/断线处理 |
| 6000ms | PICK_COMMAND | 物品拾取命令 |
| 10000ms | MATH_AUTHENTICATOR | 数学验证器 |
| 60000ms | ACCOUNT_LEVEL | 账户等级检查 |

### 对象系统

**全局对象池**: `gObj[MAX_OBJECT]` — 测试服 10000 (怪物 8000 + 用户 1200), 正式服 4500

**OBJECTSTRUCT**: 巨型结构体 (~1000 行)，包含身份/属性/技能/效果/背包/视口/坐骑/经济/自定义等

**对象状态机**: EMPTY → CREATE → PLAYING → DYING → DIECMD → DIED → DELCMD

**连接状态**: OFFLINE → CONNECTED → LOGGED → ONLINE

### 网络层 (IOCP)

- 1 Accept 线程 + 16 Worker 线程 + 1 Queue 线程
- `PER_SOCKET_CONTEXT` 管理每个 Socket 的收/发重叠上下文
- **收包流**: WSARecv → IOCP 完成 → OnRecv → DataRecv(解析) → CQueue → ProtocolCore
- **包格式**: 0xC1/C3(短包, size 1B), 0xC2/C4(长包, size 2B), 支持 4 种加密方案

### 包分发系统 (Protocol.cpp)

`ProtocolCore(BYTE head, BYTE* lpMsg, int size, int aIndex, ...)` 是核心分发器。

head 处理范围 0x00~0xFC，关键分配：
- 0x00~0x02: 聊天
- 0x18 附近: 攻击 (由 PROTOCOL_CODE2 宏控制)
- 0x22~0x26: 物品拾取/丢弃/移动/使用
- 0x30~0x34: NPC 对话/购买/出售/修理
- 0x36~0x3D: 交易系统
- 0x3F: 个人商店
- 0x40~0x43: 组队
- 0x50~0x66: 战盟
- 0x81~0x87: 仓库/混沌合成
- 0x90~0x9A: 事件 (广场/血堡)
- 0xB2: 罗兰城战 (子码 0x00~0x1F)
- 0xF3: 角色系统 (子码空间含大量自定义功能)
- 0xFA: 共鸣系统 (自定义)
- 自定义功能映射到 0xF3 子码 (0xE6 排行榜, 0xEB 离线商店, 0xF0 VIP 等)

### 怪物 AI — 四层 FSM

1. **MonsterAIRule (规则引擎)**: 条件触发（时间/事件/周期），控制切换 AI 单元
2. **MonsterAIUnit (AI 单元)**: 包含 1 个自动机 + 9 类行为（常规/移动/攻击/治疗/躲避/支援/特殊/事件/地图）
3. **MonsterAIAutomataInfo (状态机)**: 转换矩阵 [8 状态 × 30 优先级]
4. **MonsterAIState/Element**: 12 种转移类型（进/出敌人范围、HP 变化、仇恨、死亡等）+ 17 种行为（攻击/范围攻击/治疗/召唤/传送/免疫/AI 切换等）

### 事件系统 — 统一状态机

所有事件公用状态: `BLANK → EMPTY → STAND → START → CLEAN`

通过 100ms 定时器 `MainProc()` 驱动。事件包括:
- BloodCastle (血色城堡, 8 级)
- DevilSquare (恶魔广场, 7 级, 含排名系统)
- ChaosCastle (混沌城堡, 7 级, 空心区缩小机制)
- CastleSiege (罗兰城战, 10 个独立状态)
- IllusionTemple (幻影寺院), ImperialGuardian (帝国守卫军), Crywolf (狼堡), Kanturu (坎特鲁) 等

### 自定义系统 (32 个 `Custom*.cpp/h`)

遵循统一模式：全局单例 → Load(INI/XML) → LPOBJ 上下文操作 → 自定义协议头映射到原版未用子码

包括: Arena, Attack(离线挂机), BuyVip, Combo, Jewel, Mix, Monster, MonsterSkill, Move, NpcCollector/Command/Move/Quest, NoticeDrop, OnlineLottery, Pet, Pick, PKFree, Quest, Quiz, Ranking, StartItem, Store(离线商店), Top, VongQuay(转盘), Wing, WingMix 等

### 服务器间通信协议

- GameServer ↔ DataServer: `DSProtocol` (读写角色/物品/公会/活动数据)
- GameServer ↔ JoinServer: `JSProtocol` (C1:01 认证, C1:03 跨服移动, C1:30 踢下线)
- ConnectServer ↔ GameServer: `ESProtocol` (心跳上报, UDP)

---

## Main 客户端深度分析 (`Source/5.Main/source/`)

### 技术栈
- **语言**: C++ / Win32 API
- **渲染**: OpenGL (wgl)，伪装为 DirectX 9（窗口类名/标题伪装）
- **目标 FPS**: 60 (`SetTargetFps(60)`)
- **基础版本**: Season 6.3 架构 + 大量自定义扩展

### 初始化流程 (WinMain.cpp)
1. 异常处理 → NVAPI 初始化 → 版本提取 → `gMainLoad.Load()`
2. 解析命令行获取连接服务器 IP/端口
3. 加载加密密钥 (Enc1.dat/Dec2.dat)
4. 读取 config.ini → 多语言初始化 → 显示模式枚举
5. 创建 OpenGL 窗口 → PeekMessage 消息循环

### 场景状态机
```
WEBZEN_SCENE(1) → LOG_IN_SCENE(2) → LOADING_SCENE(3) → CHARACTER_SCENE(4) → MAIN_SCENE(5)
```

每帧 `MainScene(hDC)` 处理: 逻辑更新 → 3D 渲染 → 2D UI 渲染

### 渲染管线 (每帧顺序)
1. MoveMainCamera (摄像机) → 视锥体裁剪
2. RenderTerrain (地面) → RenderObjects (世界物体) → RenderEffectShadows (阴影)
3. RenderBoids (鸟群) → RenderCharactersClient (角色/怪物/NPC)
4. RenderItems (掉落物品) → RenderFishs/Bugs/Leaves (生物粒子)
5. ThePetProcess().RenderPets (宠物) → RenderJoints/Effects/Blurs (特效)
6. Sprite/Particle 系统 → RenderWaterTerrain (水面)
7. SelectObjects (拾取选择)
8. **2D UI 层**: RenderInterface(原生) → g_pNewUISystem->Render() (S3/S6 新版 UI) → gInterface.Draw() (自定义 CB 层)
9. RenderCursor (鼠标指针)

### 双 UI 层系统
- **底层**: 原版 `NewUISystem` — Season 3/6 风格 UI, 100+ 接口
- **上层**: 自定义 `gInterface` (CBInterface) — 覆盖式绘制，200+ 自定义 UI 元素
- 26 个 CB_* 类: JewelBank, DanhHieu(称号), BotTrader, LockItem, BossGuild, CTCMini, StatsAdvance, MUHelper, OffTrade, NewQuest 等

### 模型系统 (SMD)
- MAX_BONES=200, MAX_MESH=50, MAX_VERTICES=15000
- BMD → Bone_t(骨骼层次+四元数) → Mesh_t(顶点/法线/纹理/三角形) → Action_t(动画关键帧) → Texture_t
- 20+ 渲染标志: TEXTURE, CHROME, METAL, LIGHTMAP, WAVE 等
- 外部数组: `Models[]`, `BoneTransform[200][3][4]`, `VertexTransform[MAX_MESH][MAX_VERTICES]`

### 网络通信
- `CWsctlc` socket 客户端 + `CSimpleModulus` 加密
- 加密流程: 附加随机字节 → 序列号 → Encrypt → 选 0xC3 或 0xC4 格式
- 新/旧协议通过 `NEW_PROTOCOL_SYSTEM` 宏切换, `gProtocolSend` 单例封装

### 地图流程 (工厂模式)
特殊地图派生自 `BaseMap`，通过 `MapProcess` 注册:
- Raklion, SantaTown, PK Field, DuelArena
- DoppelGanger(4 副本), EmpireGuardian(4), Karutan 等
- 使用 `BoostSmart_Ptr` (shared_ptr) 管理生命周期

---

## 基础设施服务器分析

### ConnectServer (`Source/1.ConnectServer/`)
- IOCP 网络模型, MAX_CLIENT=10000, 最长在线 5 分钟
- `ClientManager` 管理客户端，`ServerList` 管理服务器列表
- **心跳**: GameServer 发送 `C1:01` 含在线数, JoinServer 发送 `C1:02`
- **客户端协议**: `C1:F4:03` 请求特定服务器, `C1:F4:06` 请求列表
- **定时器**: 1s 检测心跳, 2s 刷新显示, 5s 超时断线

### JoinServer (`Source/3.JoinServer/`)
- ODBC 直连 SQL 做账号认证，端口 63003
- `std::map<string, ACCOUNT_INFO>` 线程安全, MAX_ACCOUNT=400000
- **ACCOUNT_INFO**: 包含 AuthCode1~4 四重防篡改认证码
- **核心协议**: C1:01 登录认证(→result/BlockCode/AccountLevel), C1:03 跨服移动, C1:30 强制踢下线

### DataServer (`Source/2.DataServer/`)
- SQL 中间层，GameServer 不直连数据库
- `QueryManager` 封装 ODBC API (ExecQuery/Fetch/BindParameter)
- **角色管理器**: `std::map<string, CHARACTER_INFO>` 在线角色映射
- **公会管理器**: `vector<GUILD_INFO>`, 最多 80 会员，支持联盟/敌对
- **核心协议**: C1:01 角色列表, C1:04/C2:30 角色加载/保存, C1:70/71 上下线

### 共享基础设施模式

所有服务器复用以下组件:
- **SocketManager**: IOCP 引擎 (AcceptThread + 多 WorkerThread + QueueThread)
- **Queue**: 线程安全消息队列 (MAX_QUEUE_SIZE=2048, CriticalSection + Semaphore)
- **CriticalSection**: CRITICAL_SECTION 封装
- **MemScript**: 脚本/配置 Token 解析器
- **MiniDump**: 崩溃转储
- **Log**: 按日期自动切分日志

**协议头格式**:
- `PBMSG_HEAD {type, size, head}` — 0xC1/C3 短包
- `PSBMSG_HEAD {type, size, head, subh}` — 带子码
- `PWMSG_HEAD {type, size[2], head}` — 0xC2/C4 长包
- `PSWMSG_HEAD {type, size[2], head, subh}` — 带子码长包

---

## 部署与配置 (`MuServer/`)

### DataServer 配置
- `DataServer.ini`: ODBC=MuOnline, Port=63002
- `AllowableIpList.txt`: 仅 127.0.0.1
- `BadSyntax.txt`: 敏感词过滤

### JoinServer 配置
- `JoinServer.ini`: ODBC=MuOnline, Port=63003, ConnectServer=127.0.0.1:63001
- MD5Encryption=0 (明文密码), CaseSensitive=0, GlobalPassword=KENDEV2039

### GameServer 数据目录 (`Data/`)

| 子目录 | 用途 |
|--------|------|
| `Custom/` | 25+ 自定义系统配置 (.txt/.xml) |
| `Event/` | 事件配置 (.dat 二进制) |
| `EventItemBag/` | 事件物品掉落包 (分段文本) |
| `Hack/` | 反外挂检测 + 加密表 (Enc2.dat/Dec1.dat) |
| `Item/` | 物品系统 (380 选项/套装/孔装/堆叠等) |
| `Monster/` | 怪物 AI 规则/掉落设置 |
| `Shop/` | NPC 商店 ({编号} - {名称}.txt) |
| `Quest/`, `QuestWorld/` | 任务/世界任务 |
| `Ruud/` | Ruud 货币系统 |

### 配置格式规范
1. **INI**: `[Section] Key=Value`
2. **表格文本**: 空格/Tab 分隔, `//` 注释, `end` 结束, `*` 通配
3. **分段格式**: 多 section (0 标记开始，子循环读至 end)
4. **二进制 DAT**: 事件系统编译格式
5. **XML**: 少数复杂配置 (CustomSkinModel, CustomChoTroi)
6. **OZT/OZJ**: 客户端界面 UI 格式

### 反外挂系统 (`5.Antihack/`)
- XShield, 监听 55858, 加密密钥 `B7BDBCA8BF`
- 客户端校验: Version=1.04.05, Serial=DjGamer2022
- 洪水防护: 1 秒 150 连接阈值, 60 秒自动解封
- 可选: 内存保护/宏保护/虚拟机检测/Discord 通知/FTP 截图上传

### 数据库 (`7.DataBase/`)
- `MuOnline.bak` — SQL Server 备份
- `MuOnline.reg` / `MuOnline64.reg` — ODBC 注册配置

---

## GetMainInfo 启动器 (`GetMain/` + `Source/6.GetMainInfo/`)

### 配置编译打包流程

```
MainInfo.ini
CommonManager/*.txt (消息/Buff状态/VIP/命令)
CustomManager/*.txt (怪物/NPC/宠物/地图/伤害颜色)
EffectManager/*.txt (特效/发光/宠物特效)
ItemManager/*.txt (物品/翅膀/首饰/皮肤/提示)
  → GetMainInfo.exe (编译 + XOR 混淆加密)
  → ClientFile/Data/Local/... (三个输出文件)
```

### MainInfo.ini 关键配置
- 连接: IpAddress=192.168.1.200:63000, ClientVersion=1.04.05, ClientSerial=TbYehR2hFUPBKgZj
- 功能开关: Menu0~14 (事件时间/VIP/排行榜/变性/摆摊/称号/充值/宝石银行等)
- 自动按钮: AutoHP/AutoRS/AutoPK
- 各职业攻击速度上限 67000

### 安全层次
1. CRC32 文件完整性校验 (Main.exe/Plugin/Verify)
2. XOR 混淆加密 (.bin/License.json)
3. HWID 注册表绑定 (`SYSTEM\HardwareConfig\Current`)
4. AntiFlood 洪水防护
5. 封包加密表 (Enc2.dat/Dec1.dat)
6. 可选 Discord 通知 + FTP 截图上传

---

## Development Notes

- All components compile to **native Win32 executables** — no .NET dependency
- Database is **SQL Server** (ODBC), backup at `MuServer/7.DataBase/MuOnline.bak`
- The game client (Main) uses **OpenGL** (window class disguised as DirectX 9)
- Debug builds produce `.pdb` files and write crash dumps (`.dmp`) on exceptions
- Log files cleaned by running `MuServer/XoaLog.bat`
- The GameServer `Data/` folder mirrors runtime structure — map terrain, monster spawns, shops, item bags, quest data
- GetMainInfo 项目是配置供应链核心，将服务器文本配置编译为客户端可读的 XOR 加密二进制
- **Main.exe 调试**: 客户端可执行文件位于 `E:\MU\ClientFile\Main.exe`，配合 `config.ini` 及 `Data/Local/` 下的 CBGetMain.bin/CBTextInfo.bin/License.json 运行。调试时需确保 ConnectServer 已启动并可连接
- **编译后复制**: 每次编译完成后，将 `ClientFile\Main.exe` 复制到 `\\ALENANDAMINA\Photos\MU\`

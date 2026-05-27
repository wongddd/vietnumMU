# Mu Online 私服模拟器 — 二次开发指南

## 一、项目全景

### 1.1 仓库结构

```
E:\MU\
├── Source/               # 全部源代码（7 个子项目 + Util 工具库）
│   ├── 1.ConnectServer/  # 连接服务器（客户端入口）
│   ├── 2.DataServer/     # 数据库中间层
│   ├── 3.JoinServer/     # 账号认证服务器
│   ├── 4.GameServer/     # 核心游戏逻辑服务器
│   ├── 5.Main/           # 游戏客户端（OpenGL）
│   ├── 6.GetMainInfo/    # 启动器配置打包工具
│   └── Util/             # 共享工具库
├── MuServer/             # 部署运行目录
│   ├── 1.ConnectServer/  # CS 运行文件
│   ├── 2.DataServer/     # DS 运行文件
│   ├── 3.JoinServer/     # JS 运行文件
│   ├── 4.GameServer/Sub 1/ # GS 运行文件 + Data 配置
│   ├── 5.Antihack/       # XShield 反外挂
│   ├── 6.DatEditor/      # 数据编辑器
│   └── 7.DataBase/       # SQL 数据库备份
├── ClientFile/           # 客户端运行目录（编译输出目标）
│   ├── Main.exe          # 游戏客户端主程序
│   └── Data/             # 客户端资源文件
├── GetMain/              # 启动器配置打包工具运行目录
│   ├── MainInfo.ini      # 主配置
│   ├── GetMainInfo.exe   # 配置编译器
│   └── *Manager/         # 各分类文本配置
└── BuildLog/             # 编译中间文件
```

### 1.2 组件清单

| # | 项目 | 解决方案 | 源文件数 | 工具集 | 字符集 | 输出 | 功能 |
|---|------|----------|---------|--------|--------|------|------|
| 1 | ConnectServer | ConnectServer.sln | ~35 | v100 | MultiByte | EXE | 服务列表分发，端口 63000/63001 |
| 2 | DataServer | DataServer.sln | ~75 | v100 | MultiByte | EXE | SQL ODBC 中间层，端口 63002 |
| 3 | JoinServer | JoinServer.sln | ~35 | v100 | MultiByte | EXE | 账号认证，端口 63003 |
| 4 | GameServer | GameServer.sln | ~430 | v142/v143 | MultiByte | EXE | 核心游戏逻辑，端口 55901+ |
| 5 | Main (客户端) | Main.sln | ~760+ | v142 | MultiByte (UTF-8) | EXE | OpenGL 游戏客户端 |
| 6 | GetMainInfo | GetMainInfo.sln | ~55 | v100 | MultiByte | EXE | 配置 XOR 编译打包 |
| — | Util | (静态库) | ~10 | v100+ | — | LIB/H | CRC32/MD5/Crypto++/Lua/Detours |

### 1.3 编译配置一览

| 项目 | 构建配置 | 预处理器宏 |
|------|---------|-----------|
| GameServer | Release_EX401 / EX401CS | GAMESERVER_UPDATE=401 (Season 4) |
| GameServer | Release_EX603 / EX603CS | GAMESERVER_UPDATE=603 (Season 6) |
| GameServer | Release_EX803 / EX803CS | GAMESERVER_UPDATE=803 (Season 8) |
| Main | Global Release / Global Debug | — |
| ConnectServer | Release | — |
| DataServer | Release | — |
| JoinServer | Release | — |
| GetMainInfo | Release | — |

---

## 二、项目间依赖关系

### 2.1 编译依赖（硬依赖）

```
GetMainInfo (v100, Win32)
  └── 无外部依赖（纯 Win32 API + 标准库）

ConnectServer (v100, Win32)
  └── 无外部依赖（纯 Win32 API + IOCP）

DataServer (v100, Win32)
  └── 无外部依赖（ODBC API + Win32）

JoinServer (v100, Win32)
  └── 无外部依赖（ODBC API + Win32）

GameServer (v142/v143, Win32)
  ├── Util/pugixml (内联源码)
  └── 无外部链接库依赖（自包含）

Main (v142, Win32)
  ├── Boost 1.80.0 (in-tree: boost_1_80_0/)
  ├── DirectX 9 SDK (in-tree: sdk-directx9/)
  ├── dependencies/include (ASIO)
  ├── dependencies/lib (jpeglib, glew, SimpleModulus, ShareMemory, wzAudio)
  └── 链接库：imm32, vfw32, dsound, dxguid, opengl32, glu32, winmm, ws2_32
```

**关键发现**：所有服务器组件（CS/DS/JS/GS）均为自包含 Win32 项目，无项目间二进制链接。Main 客户端是唯一有第三方依赖的项目。GetMainInfo 既无依赖也不被链接，独立运行。

### 2.2 运行时依赖（数据流）

```
                     ┌─────────────┐
                     │   Main.exe  │ (客户端)
                     └──────┬──────┘
                            │ TCP :63000
                            ▼
               ┌─────────────────────┐
               │  ConnectServer.exe  │ (连接服务器)
               └──┬──────────────┬───┘
                  │ UDP :63001   │ TCP :63003
                  ▼              ▼
          ┌────────────┐  ┌────────────┐
          │ JoinServer │  │ GameServer │ (核心)
          └─────┬──────┘  └──┬──┬──┬──┘
                │ TCP:63002  │  │  │
                ▼            │  │  │
        ┌────────────┐       │  │  │
        │ DataServer │◄──────┘  │  │
        └─────┬──────┘          │  │
              │ (ODBC)          │  │
              ▼                 │  │
        ┌────────────┐          │  │
        │  SQL Server│          │  │
        └────────────┘          │  │
                           ┌────┘  │
                           │ UDP   │
                           ▼       ▼
                    ┌────────────────┐
                    │   XShield.exe  │ (反外挂 :55858)
                    └────────────────┘
```

#### 数据编译流（配置供应链）

```
GetMain/MainInfo.ini + *Manager/*.txt
  │
  ▼
GetMainInfo.exe (编译 + XOR 混淆加密)
  │
  ├──► ClientFile/Data/Local/CBGetMain.bin   (主配置：IP、端口、版本、物品、怪物等)
  ├──► ClientFile/Data/Local/CBTextInfo.bin   (文本数据：Buff 提示、TRS 数据)
  └──► ClientFile/Data/Local/CBBuffInfo.bin   (Buff 图标配置)

Main.exe 启动时加载：
  CBGetMain.bin  →  gProtect.ReadMainFile()
  CBTextInfo.bin →  gProtect.ReadTextFile()
```

### 2.3 依赖关系总结

| 类型 | 关系 |
|------|------|
| 编译依赖 | Main → Boost + DX9 + ASIO + 第三方库; 其余项目自包含 |
| 运行时依赖 | Main → ConnectServer → GameServer ↔ JoinServer ↔ DataServer → SQL |
| 数据依赖 | GetMainInfo(编译) → Main.exe(读取) |
| 部署依赖 | 所有 Exe 运行于 MuServer/ 目录，读取同目录下 .ini 和数据文件 |

---

## 三、Main.exe 资源与 Data 目录一致性分析

### 3.1 资源引用方式

Main.exe 通过两种方式引用 Data 下的资源：

1. **直接路径引用**：`"Data\\Local\\CBGetMain.bin"`、`"Data\\Enc1.dat"` 等
2. **动态路径引用**：`"Data\\Local\\%s\\Item_%s.bmd"`（%s = 语言代码）、`"Data\\World%d\\Terrain.map"`（%d = 地图编号）

### 3.2 目录一致性检查

#### ✅ 已存在的目录

所有对象目录（Object1~Object99）、世界目录（World1~World99）、Player、Monster、NPC、Skill、Item、Effect、Interface、Local、Custom、Launcher、Logo — 均已存在。

#### ❌ **缺失的关键目录**

| 缺失目录 | 引用文件数 | 影响 |
|---------|-----------|------|
| `Data\Sound\` | **~500 个 .wav 文件** | 游戏无音效：怪物叫声、技能音效、UI 点击、环境音全部静音 |
| `Data\Music\` | **~40 个 .mp3/.ogg 文件** | 游戏无背景音乐：登录音乐、地图音乐、事件音乐全部静音 |
| `Data\Movie\` | 2 个文件（Movie.txt, MU.wmv） | 启动动画无法播放（非致命，通常可跳过） |

**严重程度**：Sound 和 Music 缺失不会导致崩溃，但完全静音的游戏体验对玩家不可接受。

### 3.3 语言本地化资源状态

```
ClientFile/Data/Local/
├── Eng/           ✅ 完整（包含所有 .bmd 和 .txt 文件）
├── ImgsMapName/   ✅ 存在
├── Minimap/       ✅ 存在
├── *.bin          ✅ 3 个 XOR 加密配置
├── *.bmd          ✅ 通用界面资源
├── *_(eng).bmd    ✅ 英文文本资源
└── Text_eng.txt/.bmd ✅
```

**注意**：当前只有 `Eng` 语言目录有完整资源。若需支持中文（CHN），需创建：
- `Data/Local/CHN/` 目录及对应 `*_chn.bmd` 文件
- `Data/Custom/Text_CHN.ini` 文本配置

### 3.4 资源配置数量汇总

| 资源类型 | 目录 | 文件数量 |
|---------|------|---------|
| 3D 模型 (.bmd) | Object*, Player, Monster, NPC, Skill, Item | **~2767** |
| 地图文件 (.map/.att/.obj) | World* | 各约 3 个/地图 |
| 加密配置 (.bin) | Local/ | 3 |
| 文本数据 (.bmd/.txt) | Local/Eng/ | ~30 |
| 界面配置 (.ozj/.ozt) | Custom/, Interface/ | ~60 |
| 声音文件 (.wav) | ❌ Sound/ (缺失) | ~500 |
| 音乐文件 (.mp3/.ogg) | ❌ Music/ (缺失) | ~40 |
| 其他 (Glow/DAT) | Data/ | ~5 |

---

## 四、二次开发逻辑

### 4.1 开发路线图（按优先级）

#### Phase 1：基础建设
```
[1.1] 补全缺失资源
      ├── 从原始客户端提取 Sound/、Music/、Movie/ 目录
      └── 或配置静默跳过（修改代码避免加载报错）

[1.2] 建立 CHN 语言支持
      ├── 创建 Local/CHN/ 目录及 _chn.bmd 文件
      ├── 创建 Custom/Text_CHN.ini
      └── 配置注册表 LangSelection=CHN + 中文字体

[1.3] 统一字符编码
      └── 多语言已支持 GBK (936)，可进一步扩展其他编码
```

#### Phase 2：功能定制
```
[2.1] 客户端 UI 定制（CBInterface 层）
      ├── 修改 CBInterface.cpp 自定义界面
      ├── 创建新 CB_*.cpp 功能窗口
      └── 对应服务器端 Custom*.cpp 协议支持

[2.2] 服务器逻辑扩展
      ├── 添加新 Custom*.cpp（遵循单例→Load→LPOBJ 模式）
      ├── 注册新协议头映射（0xF3 子码空间）
      └── 添加数据表扩展

[2.3] 游戏内容定制
      ├── 物品/怪物/NPC/地图配置（Data/Custom/ 下 .txt/.xml）
      └── 重新运行 GetMainInfo.exe 编译 .bin 文件
```

#### Phase 3：深度开发
```
[3.1] 网络协议升级
      ├── 研究 Protocol.cpp 分发机制
      ├── 添加新包头类型
      └── 客户端-服务器双端同步修改

[3.2] 渲染系统优化
      ├── OpenGL 渲染管线（ZzzScene.cpp）
      ├── 粒子/特效系统
      └── 双 UI 层协调（NewUISystem + CBInterface）

[3.3] 自动化工具链
      ├── 批处理编译脚本
      ├── 资源打包流水线
      └── 数据库迁移工具
```

### 4.2 代码修改模式

#### 服务器端自定义系统模式
所有 `Custom*.cpp` 遵循统一模式：
1. 全局单例类
2. `Load(INI/XML)` 从配置文件初始化
3. `LPOBJ` 上下文操作接口
4. 自定义协议头映射到原版未用 `0xF3` 子码

**添加新功能模板**：
```
Server: CustomXxx.cpp/.h → Protocol.cpp 注册子码 → Data/Custom/Xxx.txt
Client: CB_Xxx.cpp/.h → CBInterface.cpp 注册 → 自定义 UI 绘制
Config: GetMain/ItemManager/ 添加 → GetMainInfo 重新编译
```

#### 客户端双 UI 层开发模式
```
底层 UI（原版）：NewUISystem::Render() — Season 3/6 风格
上层 UI（自定义）：gInterface.Draw() / CBInterface — 覆盖式绘制

添加新 UI 窗口流程：
1. 创建 CB_Xxx.cpp/.h（继承 UI 基类）
2. 在 CBInterface.cpp 中注册窗口和渲染回调
3. 添加 OZT/OZJ 界面布局文件
4. 实现消息处理（鼠标/键盘/网络）
```

### 4.3 跨项目修改注意事项

| 修改类型 | 需修改项目 | 风险 |
|---------|-----------|------|
| 添加新物品 | GameServer(CustomItem) + Main(CustomItem) + GetMainInfo | 低 |
| 添加新怪物 | GameServer(CustomMonster) + Main(CustomMonster) | 低 |
| 添加新地图 | GameServer(MapManager) + Main(GM_*.cpp/MapManager) | 中 |
| 添加新技能 | GameServer(SkillManager) + Main(SkillManager) | 高 |
| 添加新 UI | Main(CB_*.cpp) + GameServer(协议) | 中 |
| 修改协议 | GameServer(Protocol) + Main(Protocol) | 高 |
| 修改加密 | GameServer + Main + GetMainInfo | 高（需同步密钥） |

### 4.4 数据流修改链路

```
修改配置 → GetMainInfo 编译
  → CBGetMain.bin/CBTextInfo.bin/CBBuffInfo.bin
  → Main.exe 启动时加载

修改 SQL → 数据库备份（MuServer/7.DataBase/）
  → DataServer ODBC 查询
  → GameServer DSProtocol 通信

修改文本 → Custom/Text_{LANG}.ini
  → Other.cpp Load() 读取
  → 游戏内显示
```

### 4.5 安全注意事项

| 层面 | 说明 |
|------|------|
| 配置加密 | XOR 算法 `byte ^= (0xCA ^ LOBYTE(n)); byte += (0x95 ^ HIBYTE(n));` 需保持客户端-GetMainInfo 一致 |
| 反外挂 | XShield 端口 55858，加密密钥 `B7BDBCA8BF` |
| 网络加密 | Enc2.dat/Dec1.dat 加密表，需 GameServer 和 Main 同步 |
| 洪水防护 | ConnectServer 150 连接/秒阈值，60 秒自动解封 |

---

## 五、关键文件索引

### 5.1 入口与初始化

| 文件 | 功能 |
|------|------|
| `Source/4.GameServer/GameServer/GameServer.cpp` | GS WinMain 入口 |
| `Source/5.Main/source/Winmain.cpp` | 客户端 WinMain 入口 |
| `Source/5.Main/source/MainLoad.cpp` | 客户端配置加载（.bin 文件） |
| `Source/5.Main/source/ZzzOpenData.cpp` | 客户端模型/纹理/世界数据加载 |
| `Source/5.Main/source/Other.cpp` | 客户端多语言文本加载 |

### 5.2 网络协议

| 文件 | 功能 |
|------|------|
| `*/SocketManager.cpp` | IOCP 网络引擎（所有服务器） |
| `Source/4.GameServer/GameServer/Protocol.cpp` | GS 包分发核心 |
| `Source/5.Main/source/Protocol.cpp` | 客户端协议处理 |
| `Source/4.GameServer/GameServer/DSProtocol.cpp` | GS↔DS 通信 |
| `Source/4.GameServer/GameServer/JSProtocol.cpp` | GS↔JS 通信 |

### 5.3 自定义系统

| 文件 | 功能 |
|------|------|
| `Source/4.GameServer/GameServer/Custom*.cpp` (×32) | GS 端自定义功能 |
| `Source/5.Main/source/CB_*.cpp` (×26) | 客户端自定义 UI |
| `Source/5.Main/source/CBInterface.cpp` | 自定义 UI 注册/渲染 |
| `MuServer/4.GameServer/Sub 1/Data/Custom/*.txt` | 自定义系统配置 |

### 5.4 配置编译

| 文件 | 功能 |
|------|------|
| `Source/6.GetMainInfo/GetMainInfo/GetMainInfo.cpp` | .bin 文件编译器 |
| `GetMain/MainInfo.ini` | 主配置 |
| `GetMain/*Manager/*.txt` | 各分类文本配置 |

---

## 六、构建操作指南

### 6.1 使用 MSBuild 编译

```bash
# GameServer (Season 6)
msbuild "Source/4.GameServer/GameServer.sln" /p:Configuration=Release_EX603 /p:Platform=Win32

# GameServer (Season 8)
msbuild "Source/4.GameServer/GameServer.sln" /p:Configuration=Release_EX803 /p:Platform=Win32

# Main 客户端
msbuild "Source/5.Main/Main.sln" /p:Configuration="Global Release" /p:Platform=Win32

# 其他服务器
msbuild "Source/1.ConnectServer/ConnectServer.sln" /p:Configuration=Release /p:Platform=Win32
msbuild "Source/2.DataServer/DataServer.sln" /p:Configuration=Release /p:Platform=Win32
msbuild "Source/3.JoinServer/JoinServer.sln" /p:Configuration=Release /p:Platform=Win32

# 配置编译器
msbuild "Source/6.GetMainInfo/GetMainInfo.sln" /p:Configuration=Release /p:Platform=Win32
```

### 6.2 开发注意事项

1. **字符集**：所有项目使用 MultiByte Character Set，Main 使用 `/utf-8` 编译标志
2. **调试 Main.exe**：运行 ClientFile/Main.exe，确保 ConnectServer 已启动
3. **配置生效**：修改 GetMain/*.txt 后需重新运行 GetMainInfo.exe 编译 .bin
4. **版本切换**：GameServer 通过 `GAMESERVER_UPDATE` 宏切换 Season 4/6/8
5. **SQL 依赖**：DataServer 和 JoinServer 通过 ODBC 连接 SQL Server

---

## 七、数据/资源缺口总结

| 缺失资源 | 路径 | 严重程度 | 解决方式 |
|---------|------|---------|---------|
| 声音文件 | `Data/Sound/` (~500 .wav) | **高** | 从原始客户端提取 |
| 音乐文件 | `Data/Music/` (~40 mp3/ogg) | **高** | 从原始客户端提取 |
| 启动动画 | `Data/Movie/` (2 文件) | 低 | 可跳过 |
| 中文语言包 | `Data/Local/CHN/` | 中 | 需创建 |
| 中文文本 | `Data/Custom/Text_CHN.ini` | 中 | 需翻译创建 |

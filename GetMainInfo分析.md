# GetMainInfo 源代码分析

## 1. 项目结构

```
Source/6.GetMainInfo/
├── GetMainInfo.sln                  # Visual Studio 2010 解决方案
├── GetMainInfo.suo / .sdf           # VS 用户/浏览数据
├── .gitattributes / .gitignore
├── .vs/                             # VS 本地缓存
│
└── GetMainInfo/
    ├── GetMainInfo.vcxproj          # 项目文件 (PlatformToolset v143, VS2022)
    ├── GetMainInfo.vcxproj.filters  # 文件过滤器
    ├── GetMainInfo.vcxproj.user     # 用户配置
    ├── GetMainInfo.rc               # 资源文件
    ├── resource.h                   # 资源 ID
    ├── Main_48.ico                  # 图标
    ├── ReadMe.txt                   # 说明文件
    ├── ThemidaSDK.h                 # Themida 代码保护 SDK (加壳/虚拟化/反调试)
    │
    ├── stdafx.h / stdafx.cpp        # 预编译头 (常量定义 + 最大数组尺寸)
    ├── GetMainInfo.cpp              # 主入口 (3 个核心生成函数 + _tmain)
    ├── MemScript.h / MemScript.cpp  # 文本配置解析器 (Token 分词/忽略注释)
    │
    ├── CustomItem.h / .cpp          # CUSTOM_ITEM_INFO 加载
    ├── CustomWing.h / .cpp          # CUSTOM_WING_INFO 加载
    ├── CustomWIngEffect.h / .cpp    # CUSTOM_WING_EFFECT_INFO 加载
    ├── DynamicEffect.h / .cpp       # DYNAMIC_WING_EFFECT_INFO 加载
    ├── CustomJewel.h / .cpp         # CUSTOM_JEWEL_INFO 加载
    ├── CustomCloak.h / .cpp         # CUSTOMCLOAK_DATA + CEFFECT_DATA 加载
    ├── CustomGloves.h / .cpp        # CUSTOM_RF_GLOVES 加载
    ├── CustomCrossBow.h / .cpp      # CUSTOM_BOW_INFO 加载
    ├── CustomItemPosition.h / .cpp  # CUSTOM_POSITION_INFO 加载
    ├── CustomMessage.h / .cpp       # CUSTOM_MESSAGE_INFO 加载 (ENG+VTM)
    ├── Message.h / .cpp             # MESSAGE_INFO 加载 (ENG+POR+SPN)
    ├── CustomCommandInfo.h / .cpp   # CUSTOM_COMMAND_INFO 加载
    ├── CustomDmgColor.h / .cpp      # CUSTOM_DMG_COLOR 加载
    │
    ├── CustomMonster.h / .cpp       # CUSTOMMONSTER_DATA + BOSSCLASS_DATA 加载
    ├── CustomNpcName.h / .cpp       # NPCNAME_DATA 加载
    ├── CustomPet.h / .cpp           # CUSTOM_PET_INFO 加载
    ├── CustomPetEffect.h / .cpp     # PET_EFFECT_CUSTOM + RENDER_MESH 加载
    ├── UIMapName.h / .cpp           # PathMap (地图名称) 加载
    ├── TooltipBuff.h / .cpp         # RenderTooltipBuff (Buff 图标提示) 加载
    ├── ItemToolTip.h / .cpp         # nInformation + nText (物品提示) 加载
    ├── CustomMonsterGlow.h / .cpp   # CUSTOM_MONSTERGLOW_INFO + CUSTOM_GLOW_INFO 加载
    ├── MonsterEffect.h / .cpp       # xMonsterEffect (怪物特效) 加载
    ├── CustomBuyVip.h / .cpp        # CUSTOM_BUYVIP_INFO 加载
    │
    └── (外部引用) ../../5.Main/source/Utilities/CCRC32.H/.Cpp
```

---

## 2. 源文件清单

### 头文件 (`.h` / `.hpp` — 共 28 个)

| # | 文件名 | 从属 | 主要数据结构 |
|---|--------|------|-------------|
| 1 | `stdafx.h` | 项目 | 预编译头, 所有 MAX_* 常量定义, 客户端内存地址宏 |
| 2 | `ThemidaSDK.h` | 项目 | ENCODE_START/END, CLEAR_START/END 等加壳宏 |
| 3 | `MemScript.h` | 项目 | CMemScript 文本分词解析器 |
| 4 | `CustomItem.h` | 自定义物品 | CUSTOM_ITEM_INFO, CUSTOM_RINGPEN_INFO |
| 5 | `CustomWing.h` | 自定义翅膀 | CUSTOM_WING_INFO (含 4 组 OptionIndex/Value) |
| 6 | `CustomWIngEffect.h` | 翅膀特效 | CUSTOM_WING_EFFECT_INFO (RGB/Size/Main/Side) |
| 7 | `DynamicEffect.h` | 动态特效 | DYNAMIC_WING_EFFECT_INFO |
| 8 | `CustomJewel.h` | 自定义宝石 | CUSTOM_JEWEL_INFO + SUCCESS/FAILURE_INFO |
| 9 | `CustomCloak.h` | 自定义披风 | CUSTOMCLOAK_DATA, CEFFECT_DATA |
| 10 | `CustomGloves.h` | RF 拳套 | CUSTOM_RF_GLOVES (纹理/颜色/模型) |
| 11 | `CustomCrossBow.h` | 自定义弩 | CUSTOM_BOW_INFO (SkillShot/颜色) |
| 12 | `CustomItemPosition.h` | 物品位置 | CUSTOM_POSITION_INFO (平移/旋转/缩放) |
| 13 | `CustomMessage.h` | 消息文本 | CUSTOM_MESSAGE_INFO (ENG + VTM) |
| 14 | `Message.h` | 反外挂消息 | MESSAGE_INFO (ENG + POR + SPN) |
| 15 | `CustomCommandInfo.h` | 命令信息 | CUSTOM_COMMAND_INFO (命令名+颜色+说明) |
| 16 | `CustomDmgColor.h` | 伤害颜色 | CUSTOM_DMG_COLOR |
| 17 | `CustomMonster.h` | 自定义怪物 | CUSTOMMONSTER_DATA, BOSSCLASS_DATA |
| 18 | `CustomNpcName.h` | NPC 名称 | NPCNAME_DATA |
| 19 | `CustomPet.h` | 自定义宠物 | CUSTOM_PET_INFO (缩放/角度/模型) |
| 20 | `CustomPetEffect.h` | 宠物特效 | PET_EFFECT_CUSTOM, RENDER_MESH |
| 21 | `UIMapName.h` | 地图名称 | PathMap (world/MapName/TgaMapName/Mp3) |
| 22 | `TooltipBuff.h` | Buff 提示 | RenderTooltipBuff |
| 23 | `ItemToolTip.h` | 物品提示 | nInformation, nText |
| 24 | `CustomMonsterGlow.h` | 怪物发光 | CUSTOM_MONSTERGLOW_INFO, CUSTOM_GLOW_INFO |
| 25 | `MonsterEffect.h` | 怪物特效 | xMonsterEffect (动态/静态/技能) |
| 26 | `CustomBuyVip.h` | 购买 VIP | CUSTOM_BUYVIP_INFO |
| 27 | `CCRC32.H` | 外部引用 | CCRC32 校验类 |
| 28 | `resource.h` | 项目 | 资源 ID |

### 源文件 (`.cpp` — 共 27 个)

| # | 文件名 | 功能 |
|---|--------|------|
| 1 | `GetMainInfo.cpp` | **主入口** — `_tmain`, `GetMainInfo()`, `GetTextInfo()`, `GetAntiInfo()` |
| 2 | `stdafx.cpp` | 预编译头创建 |
| 3 | `MemScript.cpp` | 文本配置文件解析器实现 |
| 4 | `CustomItem.cpp` | 加载 CustomItem.txt + CustomRingPen.txt |
| 5 | `CustomWing.cpp` | 加载 CustomWing.txt |
| 6 | `CustomWIngEffect.cpp` | 加载 CustomEffectStatic.txt |
| 7 | `DynamicEffect.cpp` | 加载 CustomEffectDynamic.txt |
| 8 | `CustomJewel.cpp` | 加载 CustomJewel.txt |
| 9 | `CustomMessage.cpp` | 加载 CustomMessage.txt |
| 10 | `CustomCloak.cpp` | 加载 CustomCape.txt + CEffect.txt |
| 11 | `CustomGloves.cpp` | 加载 CustomClaws.txt |
| 12 | `CustomCrossBow.cpp` | 加载 CustomCrossbow.txt |
| 13 | `CustomItemPosition.cpp` | 加载 CustomItemPosition.txt |
| 14 | `CustomMonster.cpp` | 加载 CustomMonster.txt |
| 15 | `CustomNpcName.cpp` | 加载 CustomNPCName.txt |
| 16 | `CustomPet.cpp` | 加载 CustomPet.txt |
| 17 | `CustomPetEffect.cpp` | 加载 CustomPetEffect.txt + CustomPetGlow.txt |
| 18 | `UIMapName.cpp` | 加载 CustomMapList.txt |
| 19 | `TooltipBuff.cpp` | 加载 CustomBuffEffect_ENG.txt + CustomBuffEffect_VTM.txt |
| 20 | `ItemToolTip.cpp` | 加载 CustomItemToolTip.txt + CustomTextTooltip.txt |
| 21 | `CustomMonsterGlow.cpp` | 加载 CustomMonsterGlow.txt |
| 22 | `MonsterEffect.cpp` | 加载 CustomMonsterEffect.txt |
| 23 | `CustomBuyVip.cpp` | 加载 CustomBuyVip.txt |
| 24 | `CustomCommandInfo.cpp` | 加载 CustomCommandInfo.txt |
| 25 | `CustomDmgColor.cpp` | 加载 CustomDmgColor.txt |
| 26 | `Message.cpp` | 加载 Message.txt (ANTIHACK) |
| 27 | `CCRC32.Cpp` | 外部引用 — CRC32 计算 |

### 项目/资源文件

| 文件 | 说明 |
|------|------|
| `GetMainInfo.vcxproj` | MSBuild 项目 (PlatformToolset v143, MultiByte, Win32) |
| `GetMainInfo.vcxproj.filters` | 文件过滤分类 |
| `GetMainInfo.sln` | VS2010 格式解决方案 |
| `GetMainInfo.rc` | Windows 资源脚本 |
| `Main_48.ico` | 应用程序图标 |
| `ReadMe.txt` | 自述文件 |

---

## 3. 核心功能

`_tmain` 主函数依次调用三个生成函数:

```
_tmain()
  ├── CreateDirectory("..\\ClientFile\\Data")
  ├── CreateDirectory("..\\ClientFile\\Data\\Local")
  ├── GetMainInfo()     → 输出 CBGetMain.bin
  ├── GetTextInfo()     → 输出 CBTextInfo.bin
  └── GetAntiInfo()     → 输出 License.json
```

### 3.1 输入文件

#### GetMainInfo() 读取的输入文件 (共 26 个 .txt 加 MainInfo.ini)

##### MainInfo.ini (主配置)
- `[AntihackConfig]` — LoadAntihack, GSPortMin/Max, AntihackServerPort/Name
- `[MainInfo]` — LauncherType/Name/File, CustomerName, IpAddress/Port, ClientVersion/Serial, WindowName, ScreenShotPath, ClientName, PluginName, CameraName, GTM, IsSeason, ShowInfoItemDrop
- `[HelperInfo]` — HelperActiveAlert/Level
- `[CharacterInfo]` — DW/DK/FE/MG/DL/SU/RF 各职业 MaxAttackSpeed
- `[ReconnectInfo]` — ReconnectTime
- `[Custom]` — EnableCsSkillsAllMaps, PlayerShowName/GuildLogo, ShowCommandInfo, FpsMonitor/Limit, ZoomMin/Max, Menu0~14 按钮开关, 银行信息, FB/Zalo 按钮, EnableSlotPet, RemoveClass, DisableMix380, CapDo_Blood/Devil, AutoHP/PK/Rate, HidenPet 各职业开关, OnBangChiSo, OnHealBar

##### EffectManager/ (7 个文件)
| 源代码路径 | 对应文件 | 数据结构 |
|-----------|---------|---------|
| `EffectManager\\CustomEffectStatic.txt` | `GetMain/EffectManager/CustomEffectStatic.txt` | CUSTOM_WING_EFFECT_INFO[5000] |
| `EffectManager\\CustomEffectDynamic.txt` | `GetMain/EffectManager/CustomEffectDynamic.txt` | DYNAMIC_WING_EFFECT_INFO[5000] |
| `EffectManager\\CEffect.txt` | `GetMain/EffectManager/CEffect.txt` | CEFFECT_DATA[200] (披风特效) |
| `EffectManager\\CustomMonsterGlow.txt` | `GetMain/EffectManager/CustomMonsterGlow.txt` | CUSTOM_MONSTERGLOW_INFO[100] + CUSTOM_GLOW_INFO[100] |
| `EffectManager\\CustomMonsterEffect.txt` | `GetMain/EffectManager/CustomMonsterEffect.txt` | xMonsterEffect[1500] |
| `EffectManager\\CustomPetEffect.txt` | `GetMain/EffectManager/CustomPetEffect.txt` | PET_EFFECT_CUSTOM[3000] |
| `EffectManager\\CustomPetGlow.txt` | `GetMain/EffectManager/CustomPetGlow.txt` | RENDER_MESH[1000] |

##### ItemManager/ (10 个文件)
| 源代码路径 | 对应文件 | 数据结构 |
|-----------|---------|---------|
| `ItemManager\\CustomCape.txt` | `GetMain/ItemManager/CustomCape.txt` | CUSTOMCLOAK_DATA[20] |
| `ItemManager\\CustomClaws.txt` | `GetMain/ItemManager/CustomClaws.txt` | CUSTOM_RF_GLOVES[50] |
| `ItemManager\\CustomCrossbow.txt` | `GetMain/ItemManager/CustomCrossbow.txt` | CUSTOM_BOW_INFO[50] |
| `ItemManager\\CustomItem.txt` | `GetMain/ItemManager/CustomItem.txt` | CUSTOM_ITEM_INFO[2000] |
| `ItemManager\\CustomRingPen.txt` | `GetMain/ItemManager/CustomRingPen.txt` | CUSTOM_RINGPEN_INFO[2000] |
| `ItemManager\\CustomItemPosition.txt` | `GetMain/ItemManager/CustomItemPosition.txt` | CUSTOM_POSITION_INFO[1000] |
| `ItemManager\\CustomJewel.txt` | `GetMain/ItemManager/CustomJewel.txt` | CUSTOM_JEWEL_INFO[25] |
| `ItemManager\\CustomWing.txt` | `GetMain/ItemManager/CustomWing.txt` | CUSTOM_WING_INFO[100] |

##### CommonManager/ (6 个文件)
| 源代码路径 | 对应文件 | 数据结构 |
|-----------|---------|---------|
| `CommonManager\\CustomMessage.txt` | `GetMain/CommonManager/CustomMessage.txt` | CUSTOM_MESSAGE_INFO[500] × 2 (ENG+VTM) |
| `CommonManager\\CustomCommandInfo.txt` | `GetMain/CommonManager/CustomCommandInfo.txt` | CUSTOM_COMMAND_INFO[100] |
| `CommonManager\\CustomBuyVip.txt` | `GetMain/CommonManager/CustomBuyVip.txt` | CUSTOM_BUYVIP_INFO[3] |
| `CommonManager\\CustomBuffEffect_ENG.txt` | `GetMain/CommonManager/CustomBuffEffect_ENG.txt` | RenderTooltipBuff[256] (英语) |
| `CommonManager\\CustomBuffEffect_VTM.txt` | `GetMain/CommonManager/CustomBuffEffect_VTM.txt` | RenderTooltipBuff[256] (越南语) |

##### CustomManager/ (5 个文件)
| 源代码路径 | 对应文件 | 数据结构 |
|-----------|---------|---------|
| `CustomManager\\CustomMonster.txt` | `GetMain/CustomManager/CustomMonster.txt` | CUSTOMMONSTER_DATA[100] + BOSSCLASS_DATA[100] |
| `CustomManager\\CustomNPCName.txt` | `GetMain/CustomManager/CustomNPCName.txt` | NPCNAME_DATA[100] |
| `CustomManager\\CustomPet.txt` | `GetMain/CustomManager/CustomPet.txt` | CUSTOM_PET_INFO[300] |
| `CustomManager\\CustomDmgColor.txt` | `GetMain/CustomManager/CustomDmgColor.txt` | CUSTOM_DMG_COLOR[15] |
| `CustomManager\\CustomMapList.txt` | `GetMain/CustomManager/CustomMapList.txt` | PathMap[200] (地图名/TGA/MP3) |

#### GetTextInfo() 额外读取的输入文件 (4 个)
| 源代码路径 | 对应文件 | 数据结构 |
|-----------|---------|---------|
| `CommonManager\\CustomBuffEffect_ENG.txt` | 同上 | RenderTooltipBuff[256] |
| `CommonManager\\CustomBuffEffect_VTM.txt` | 同上 | RenderTooltipBuff[256] |
| `ItemManager\\CustomItemToolTip.txt` | `GetMain/ItemManager/CustomItemToolTip.txt` | nInformation[3000] |
| `ItemManager\\CustomTextTooltip.txt` | `GetMain/ItemManager/CustomTextTooltip.txt` | nText[3000] |

#### GetAntiInfo() 额外读取的输入文件 (1 个)
| 源代码路径 | 对应文件 | 数据结构 |
|-----------|---------|---------|
| `CommonManager\\Message.txt` | `GetMain/CommonManager/Message.txt` | MESSAGE_INFO[50] × 3 (ENG+POR+SPN) |

#### CRC32 校验的文件 (运行时计算, 不读取内容, 仅计算哈希)
| 配置来源 | 文件 |
|---------|------|
| MainInfo.ini → ClientName | Main.exe |
| MainInfo.ini → PluginName | Plugin (可空) |
| MainInfo.ini → CameraName | Camera (可空) |

---

### 3.2 输出文件

| 输出文件 | 生成函数 | 结构体类型 | 大小 (当前) | 加密算法 |
|---------|---------|-----------|------------|---------|
| `../ClientFile/Data/Local/CBGetMain.bin` | `GetMainInfo()` | `MAIN_FILE_INFO` | 1,138,724 bytes | XOR + ADD (字节位置相关) |
| `../ClientFile/Data/Local/CBTextInfo.bin` | `GetTextInfo()` | `TEXT_FILE_INFO` | 2,343,456 bytes | XOR + ADD (字节位置相关) |
| `../ClientFile/Data/Local/License.json` | `GetAntiInfo()` | `ANTIHACK_CLIENT_INFO` | 20,052 bytes | XOR + SUB (常数) |

#### CBGetMain.bin 格式

直接以 `MAIN_FILE_INFO` 结构体的原始内存二进制写入，经过位置相关 XOR 加密。

**MAIN_FILE_INFO 结构体布局 (约 1.1 MB)**:

```
字段                                  类型              大小 (约)
────────────────────────────────────────────────────────────
GSPortMin/GSPortMax                  DWORD × 2         8
LoadAntihack/LauncherType           BYTE × 2          2
LauncherName/LauncherFile           char[32] × 2      64
CustomerName                         char[32]          32
IpAddress                            char[32]          32
IpAddressPort                        WORD              2
ClientVersion                        char[8]           8
ClientSerial                         char[17]          17
WindowName                           char[32]          32
ScreenShotPath                       char[50]          50
ClientName                           char[32]          32
TIME_GMT                             DWORD             4
PluginName/CameraName                char[32] × 2      64
ClientCRC32/PluginCRC32/CameraCRC32 DWORD × 3         12
HelperActiveAlert/Level              DWORD × 2         8
DW/DK/FE/MG/DL/SU/RF MaxAttackSpeed DWORD × 7         28
ReconnectTime                        DWORD             4
EnableCsSkillsAllMaps                DWORD             4
PlayerShowName/GuildLogo             DWORD × 2         8
FpsMonitor/FpsLimit                  DWORD × 2         8
ZoomMin/Max                          DWORD × 2         8
Menu[15]                             DWORD × 15        60
EnableNapBankButton                  DWORD             4
Bank_NameBank/STK/ChuTK/ChiNhanh    char[126] × 4     504
EnableButtonFB/ZL/RS                 BOOL × 3          12
LinkFacbook/Zalo                     char[128] × 2     256
ViTriFB/ZL/RS/AutoHP/AutoPK[2]      int × 10          40
EnableSlotPet/DisableMix380          BOOL × 2          8
RemoveClass/CapDo_Blood/Devil        BYTE × 3          3
EnableButtonAutoHP/DelayAutoHP       BOOL + int        8
RateHP                               int               4
OnBangChiSo/OnHealBar                BYTE × 2          2
────────────────────────────────────────────────────────────
// 数组 (此后的字段占主体)
EngCustomMessageInfo[500]            104 字节/元素    52,000
VtmCustomMessageInfo[500]            104 字节/元素    52,000
CustomJewelInfo[25]                  244 字节/元素     6,100
CustomWingInfo[100]                  180 字节/元素    18,000
m_CustomCloak[20]                    196 字节/元素     3,920
m_CustomCEffect[200]                 16 字节/元素      3,200
CustomItemInfo[2000]                 48 字节/元素     96,000
CustomRingPenInfo[2000]              16 字节/元素     32,000
CustomWingEffectInfo[5000]           52 字节/元素    260,000
DynamicWingEffectInfo[5000]          52 字节/元素    260,000
CustomBowInfo[50]                    52 字节/元素      2,600
CustomGloves[50]                     148 字节/元素     7,400
CustomPosition[1000]                 32 字节/元素     32,000
IsVersion                             DWORD             4
ShowInfoItemDrop                      BOOL              4
CustomMonsters[100]                  145 字节/元素    14,500
CustomBossClass[100]                 48 字节/元素      4,800
CustomNPCName[100]                   48 字节/元素      4,800
CustomPetInfo[300]                   104 字节/元素    31,200
m_PetCEffectBMD[3000]                48 字节/元素    144,000
RenderMeshPet[1000]                  32 字节/元素     32,000
m_MapInfo[200]                       268 字节/元素    53,600
CustomBuyVipInfo[3]                  44 字节/元素        132
m_CustomMonsterGlow[100]             32 字节/元素      3,200
m_CustomMonsterbrightness[100]       24 字节/元素      2,400
m_CustomMonsterEffect[1500]          48 字节/元素     72,000
ShowCommandInfo                       bool              1
CustomCommandInfo[100]               184 字节/元素    18,400
HidenPetIsClass[7]                    BYTE × 7          7
HidenPetDelay                         DWORD             4
CustomDmgColor[15]                   16 字节/元素        240
────────────────────────────────────────────────────────────
约计:                                 ~1,130,000 字节
```

#### CBTextInfo.bin 格式

以 `TEXT_FILE_INFO` 结构体原始内存二进制写入。

**TEXT_FILE_INFO 结构体布局 (约 2.3 MB)**:

```
字段                                  类型              大小 (约)
────────────────────────────────────────────────────────────
m_TooltipTRSDataEng[256]             RenderTooltipBuff  165,888
m_TooltipTRSDataVTM[256]             RenderTooltipBuff  165,888
  (每个 RenderTooltipBuff = 字节 × 16 = 648 字节)
n_TRSTooltipData[3000]               nInformation       336,000
  (每个 nInformation = 112 字节)
n_TRSTooltipSetData[3000]            nInformation       336,000
n_TRSTooltipText[3000]               nText              774,000
  (每个 nText = 258 字节)
────────────────────────────────────────────────────────────
约计:                                 ~2,340,000 字节
```

#### License.json 格式

注意: 文件后缀为 `.json`，但实际内容是 `ANTIHACK_CLIENT_INFO` 结构体的**二进制**数据（非 JSON 文本），经过常数 XOR 加密。

**ANTIHACK_CLIENT_INFO 结构体布局 (约 20 KB)**:

```
字段                                  类型              大小
──────────────────────────────────────────────────────
CustomerName                         char[32]          32
IpAddress                            char[32]          32
ServerPort                           WORD              2
ServerName                           char[32]          32
ClientName                           char[32]          32
PluginName                           char[32]          32
VerifyName                           char[32]          32
RegistryHWID                         char[32]          32
ClientCRC32                          DWORD             4
PluginCRC32                          DWORD             4
VerifyCRC32                          DWORD             4
ShowMessageBox                       BYTE              1
GSPortMin                            DWORD             4
GSPortMax                            DWORD             4
EngMessageInfo[50]                   MESSAGE_INFO      (50×132=6,600)
PorMessageInfo[50]                   MESSAGE_INFO      (50×132=6,600)
SpnMessageInfo[50]                   MESSAGE_INFO      (50×132=6,600)
──────────────────────────────────────────────────────
约计:                                 ~20,050 字节
```

---

### 3.3 加密算法

GetMainInfo 使用了三种不同的加密算法，均为流式逐字节混淆（非标准 AES/DES），解码时反向操作即可。

#### 算法 1: CBGetMain.bin / CBTextInfo.bin (位置依赖 XOR + ADD)

```cpp
// 加密 (写入前)
for (int n = 0; n < sizeof(STRUCTURE); n++)
{
    ((BYTE*)&info)[n] ^= (BYTE)(0xCA ^ LOBYTE(n));   // XOR: 0xCA XOR n的低8位
    ((BYTE*)&info)[n] += (BYTE)(0x95 ^ HIBYTE(n));    // ADD: 0x95 XOR n的高8位
}

// 解密 (读取后)
for (int n = 0; n < sizeof(STRUCTURE); n++)
{
    ((BYTE*)&info)[n] -= (BYTE)(0x95 ^ HIBYTE(n));    // 反向: 先减
    ((BYTE*)&info)[n] ^= (BYTE)(0xCA ^ LOBYTE(n));   // 反向: 后 XOR
}
```

关键特征:
- 每个字节的变换系数取决于其**位置 (n)** 在结构体中的偏移量
- `LOBYTE(n) = n & 0xFF`, `HIBYTE(n) = (n >> 8) & 0xFF`
- 加密和解密使用相同的 `0xCA` 和 `0x95` 常数
- 这是轻量级混淆，非密码学安全

#### 算法 2: License.json (常数 XOR + SUB)

```cpp
// 加密 (写入前)
for (int n = 0; n < sizeof(ANTIHACK_CLIENT_INFO); n++)
{
    ((BYTE*)&info)[n] ^= 0xF5;          // XOR 常数 0xF5
    ((BYTE*)&info)[n] -= 0x7D;          // 减去常数 0x7D
}

// 解密 (读取后)
for (int n = 0; n < sizeof(ANTIHACK_CLIENT_INFO); n++)
{
    ((BYTE*)&info)[n] += 0x7D;          // 反向: 先加
    ((BYTE*)&info)[n] ^= 0xF5;          // 反向: 后 XOR
}
```

关键特征:
- 每个字节使用相同的固定密钥 (`0xF5`, `0x7D`)，与位置无关
- 比算法 1 更简单

#### CRC32 完整性校验

使用标准 CRC-32 算法 (多项式 `0x04C11DB7`, PKZip 标准)，计算 Main.exe / Plugin / Camera 的 CRC32 值并存入结构体，客户端可用于文件完整性校验。

#### Themida 加壳 (编译期)

源代码中嵌入 Themida SDK 宏:
- `ENCODE_START / ENCODE_END` — 标记代码段加密
- `CLEAR_START / CLEAR_END` — 标记清除段（运行后清空内存）
- 发布版 exe 使用 Themida 加壳保护

---

## 4. 缺失资源

### 源代码引用文件 vs GetMain/ 实际文件对比

源代码中共引用 **29 个文件路径**，对比 GetMain/ 目录下 **34 个文件**:

#### 源代码引用的文件 (全部存在) — 29/29 完全命中

| 引用路径 | GetMain/ 实际文件 | 状态 |
|---------|------------------|------|
| `.\MainInfo.ini` | `GetMain/MainInfo.ini` | 存在 |
| `EffectManager\CustomEffectStatic.txt` | `GetMain/EffectManager/CustomEffectStatic.txt` | 存在 |
| `EffectManager\CustomEffectDynamic.txt` | `GetMain/EffectManager/CustomEffectDynamic.txt` | 存在 |
| `EffectManager\CEffect.txt` | `GetMain/EffectManager/CEffect.txt` | 存在 |
| `EffectManager\CustomMonsterGlow.txt` | `GetMain/EffectManager/CustomMonsterGlow.txt` | 存在 |
| `EffectManager\CustomMonsterEffect.txt` | `GetMain/EffectManager/CustomMonsterEffect.txt` | 存在 |
| `EffectManager\CustomPetEffect.txt` | `GetMain/EffectManager/CustomPetEffect.txt` | 存在 |
| `EffectManager\CustomPetGlow.txt` | `GetMain/EffectManager/CustomPetGlow.txt` | 存在 |
| `ItemManager\CustomCape.txt` | `GetMain/ItemManager/CustomCape.txt` | 存在 |
| `ItemManager\CustomClaws.txt` | `GetMain/ItemManager/CustomClaws.txt` | 存在 |
| `ItemManager\CustomCrossbow.txt` | `GetMain/ItemManager/CustomCrossbow.txt` | 存在 |
| `ItemManager\CustomItem.txt` | `GetMain/ItemManager/CustomItem.txt` | 存在 |
| `ItemManager\CustomRingPen.txt` | `GetMain/ItemManager/CustomRingPen.txt` | 存在 |
| `ItemManager\CustomItemPosition.txt` | `GetMain/ItemManager/CustomItemPosition.txt` | 存在 |
| `ItemManager\CustomJewel.txt` | `GetMain/ItemManager/CustomJewel.txt` | 存在 |
| `ItemManager\CustomWing.txt` | `GetMain/ItemManager/CustomWing.txt` | 存在 |
| `ItemManager\CustomItemToolTip.txt` | `GetMain/ItemManager/CustomItemToolTip.txt` | 存在 |
| `ItemManager\CustomTextTooltip.txt` | `GetMain/ItemManager/CustomTextTooltip.txt` | 存在 |
| `CommonManager\CustomMessage.txt` | `GetMain/CommonManager/CustomMessage.txt` | 存在 |
| `CommonManager\CustomCommandInfo.txt` | `GetMain/CommonManager/CustomCommandInfo.txt` | 存在 |
| `CommonManager\CustomBuyVip.txt` | `GetMain/CommonManager/CustomBuyVip.txt` | 存在 |
| `CommonManager\CustomBuffEffect_ENG.txt` | `GetMain/CommonManager/CustomBuffEffect_ENG.txt` | 存在 |
| `CommonManager\CustomBuffEffect_VTM.txt` | `GetMain/CommonManager/CustomBuffEffect_VTM.txt` | 存在 |
| `CommonManager\Message.txt` | `GetMain/CommonManager/Message.txt` | 存在 |
| `CustomManager\CustomMonster.txt` | `GetMain/CustomManager/CustomMonster.txt` | 存在 |
| `CustomManager\CustomNPCName.txt` | `GetMain/CustomManager/CustomNPCName.txt` | 存在 |
| `CustomManager\CustomPet.txt` | `GetMain/CustomManager/CustomPet.txt` | 存在 |
| `CustomManager\CustomDmgColor.txt` | `GetMain/CustomManager/CustomDmgColor.txt` | 存在 |
| `CustomManager\CustomMapList.txt` | `GetMain/CustomManager/CustomMapList.txt` | 存在 |

#### GetMain/ 中存在但未被源代码引用的文件 (可能被客户端直接使用)

| 文件 | 说明 |
|------|------|
| `GetMain/ItemManager/CustomSkinModel.txt` | 自定义皮肤模型配置 — 可能被 Main.exe 客户端直接读取，不由 GetMainInfo 编译 |
| `GetMain/CustomManager/ColorDame.html` | HTML 颜色参考文件 — 辅助开发用，不影响编译 |

#### 缺失文件结论

**源代码引用的全部 29 个输入文件在 GetMain/ 目录下均存在，无缺失文件。**

---

## 5. 编译依赖

### 构建工具链
- **IDE**: Visual Studio 2010+ (解决方案格式 v11)
- **PlatformToolset**: v143 (Visual Studio 2022)
- **Platform**: Win32 (x86)
- **CharacterSet**: MultiByte (非 Unicode)
- **Runtime Library**: MultiThreaded (Release) / MultiThreadedDebug (Debug)
- **SubSystem**: Console

### 外部依赖

| 依赖 | 来源 | 用途 |
|------|------|------|
| `CCRC32.H` / `CCRC32.Cpp` | `../../5.Main/source/Utilities/` (项目内) | CRC32 文件完整性校验 |
| `ThemidaSDK.h` | 项目内自带 | 代码加壳/虚拟化宏 |

### 不需要的外部库

| 库 | 说明 |
|---|------|
| Boost | **未使用** (GameServer 需要但 GetMainInfo 不需要) |
| Crypto++ | **未使用** |
| DirectX 9 | **未使用** |
| ODBC | **未使用** |
| detours | **未使用** |
| Lua | **未使用** |
| pugixml | **未使用** |

### 仅使用的 Windows API
- `kernel32.dll`: `CreateFile`, `WriteFile`, `ReadFile`, `CloseHandle`, `GetPrivateProfileInt`, `GetPrivateProfileString`, `CreateDirectory`, `GetFileSize`, `GetTickCount`
- `msvcrt.dll`: `fopen_s`, `fread`, `fclose`, `malloc`, `free`
- CRT: `strcpy_s`, `strcmp`, `memset`, `memcpy`, `wsprintf`, `isspace`, `isdigit`, `isalpha`, `isalnum`, `atof`

### 编译器选项
- **优化**: `MaxSpeed` (Release), `Disabled` (Debug)
- **警告**: `TurnOffAllWarnings` (Release), `Level3` (Debug)
- **全程序优化**: 启用 (Release)
- **COMDAT 折叠**: 启用 (Release)
- **栈保留大小**: `9999999999` (约 9.3 GB, 巨大的值, 可能笔误或特殊需求)
- **预编译头**: `stdafx.cpp` Create, 其余 Use

### 构建命令
```bash
msbuild "Source/6.GetMainInfo/GetMainInfo.sln" /p:Configuration=Release /p:Platform=Win32
```

### 输出路径
- **Release**: `E:\MU\GetMain\GetMainInfo.exe` (相对于 project: `..\..\..\GetMain`)
- **Debug**: `E:\MuHaiDuong.net\MuCustom 12-5\` (外部路径)

---

## 6. 同步状态

### 输出文件存在性检查

| 文件 | 路径 | 大小 | 修改时间 | 状态 |
|------|------|------|---------|------|
| CBGetMain.bin | `ClientFile/Data/Local/CBGetMain.bin` | 1,138,724 字节 | 2026-05-27 02:32 | 存在 |
| CBTextInfo.bin | `ClientFile/Data/Local/CBTextInfo.bin` | 2,343,456 字节 | 2026-05-27 02:32 | 存在 |
| License.json | `ClientFile/Data/Local/License.json` | 20,052 字节 | 2026-05-27 02:32 | 存在 |
| CBBuffInfo.bin | `ClientFile/Data/Local/CBBuffInfo.bin` | 147,456 字节 | 2023-12-13 | 存在 (非 GetMainInfo 生成) |

### 编译二进制状态

| 文件 | 路径 | 大小 | 修改时间 | 说明 |
|------|------|------|---------|------|
| GetMainInfo.exe | `GetMain/GetMainInfo.exe` | 3,698,176 字节 | 2025-07-06 | 已编译的 Release 二进制 |
| GetMainInfo.pdb | `GetMain/GetMainInfo.pdb` | 6,066,176 字节 | 2025-07-06 | 调试符号 |

### 时间线分析

1. **GetMainInfo.exe 编译时间**: 2025-07-06 — 二进制已有一年未重编译
2. **输出文件生成时间**: 2026-05-27 02:32 — 说明近期有人运行了 GetMainInfo.exe 重新生成了客户端文件
3. 输出文件的时间戳均为同一分钟，说明是单次完整运行生成

### 同步结论

- GetMainInfo 源代码 (`Source/6.GetMainInfo/`) 与可执行文件 (`GetMain/GetMainInfo.exe`) 基于同一套源代码编译
- `GetMain/` 下的所有输入配置文件 (**29 个源文件**) 与源代码引用的路径**完全匹配**
- 三个输出文件 (`CBGetMain.bin`, `CBTextInfo.bin`, `License.json`) 均已存在于 `ClientFile/Data/Local/` 且为**最近生成** (2026-05-27)
- `CBBuffInfo.bin` 存在于客户端目录但不属于 GetMainInfo 的产出，需要单独追踪其生成源
- Main.exe 客户端的 `Data/Local/` 目录配置完整，满足运行条件

### 重新编译并生成步骤

如需重新编译并生成客户端配置:

```bash
# 步骤 1: 编译 GetMainInfo.exe
msbuild "E:\MU\Source\6.GetMainInfo\GetMainInfo.sln" /p:Configuration=Release /p:Platform=Win32

# 步骤 2: 运行生成配置 (执行时会从 .\MainInfo.ini 和子目录读取配置)
cd E:\MU\GetMain
.\GetMainInfo.exe

# 步骤 3: 输出文件将更新到
#   E:\MU\ClientFile\Data\Local\CBGetMain.bin
#   E:\MU\ClientFile\Data\Local\CBTextInfo.bin
#   E:\MU\ClientFile\Data\Local\License.json
```

# JMS583 硬盘盒

> 基于 JMS583 主控的 M.2 NVMe/SATA 固态硬盘盒开源项目，采用「硬盘盒 + M.2 固态」的组合形式，**顺序读取速度可达 400MB/s**，兼顾大容量与高速读写。

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](./LICENSE)
[![Platform](https://img.shields.io/badge/Platform-JMS583-orange.svg)](https://www.jmicron.com/)

- **嘉立创开源硬件平台**：https://oshwhub.com/qiao_wen/jms583-based-hard-disk-box
- **Gitee 仓库**：https://gitee.com/qiao-wenguo/jms583-hard-drive-box

---

## 目录

- [项目简介](#项目简介)
- [特性](#特性)
- [硬件架构](#硬件架构)
- [实物展示](#实物展示)
- [快速开始](#快速开始)
- [固件与 Windows 测试](#固件与-windows-测试)
- [外壳与装配](#外壳与装配)
- [目录结构](#目录结构)
- [许可证](#许可证)
- [致谢与参考](#致谢与参考)

---

## 项目简介

传统的移动存储介质，容量与速度难以兼得。本项目以 **JMicron JMS583** USB 3.1 Gen 2 转 PCIe/NVMe 桥接芯片为核心，搭配一块 **M.2 固态硬盘**，实现一个：
- **大容量**：支持 NVMe M.2 SSD，轻松扩展到数 TB；
- **高速度**：USB 3.1 Gen 2（10Gbps）接口，实测**顺序读取约 700MB/s**；
- **扩展性**：比成品硬盘盒更灵活，加入stc8g单片驱动内置风扇，便于学习与二次开发。
主要面向：需要大容量高速移动存储的场景。 
>——锅包肉

![成品效果3](assets/_8效果2.jpg)
![产品整体效果](assets/渲染图2.png)
> 成品整体效果（PCB + 外壳 + M.2 固态装配完成）。

详见 [特性一览](#特性) 与 [实物展示](#实物展示)。

---

## 特性

- 🚀 **主控芯片**：JMicron JMS583（USB 3.1 Gen 2 to NVMe Bridge）
- 💾 **存储接口**：M.2 M-Key（NVMe），具体以硬件版本为准
- 🔌 **上行接口**：USB Type-C（USB 3.1 Gen 2，10Gbps）
- ⚡ **性能**：顺序读取可达 **700MB/s**（受 SSD 与线缆质量影响）
- 🧊 **散热**：PCB 预留散热片，内置涡轮风扇，外壳开散热窗
- 🔧 **固件**：提供JMS583固件与 stc8g的驱动
- 📦 **外壳**：3D 打印，装配即用

![硬件架构图](assets/架构.png)
> 思路结构。

---

## 硬件架构

系统由 **USB 接口 → JMS583 桥接 → M.2 固态** 三部分组成，整体框图如下：

```
   主机 (PC/Mac)
        │  USB 3.1 Gen 2 (10Gbps)
        ▼
   ┌───────────┐
   │  Type-C   │
   └────┬──────┘
        ▼
   ┌──────────────────────────────┐
   │       JMS583 (桥接芯片)      │
   │  USB 3.1 Gen 2 ──► PCIe / SATA
   └────┬─────────────┬──────────┘
        │             │
        ▼             ▼
   M.2 NVMe        M.2 SATA      （视硬件版本支持其一或二者）
   (M-Key)         (B-Key)
        │
        ▼
    固态硬盘 (SSD)
```

主要元器件：

| 位号 | 器件 | 说明 |
|------|------|------|
| U1 | JMS583 | 核心桥接主控 |
| U2 | 时钟晶振 | 提供 25MHz 参考时钟 |
| U3 | LDO / DCDC | 为核心与 M.2 供电（3.3V / 1.2V 等） |
| J1 | USB Type-C | 上行数据 & 供电 |
| J2 | M.2 插槽 | 安装 NVMe / SATA 固态 |
| — | 滤波电容 / ESD | 电源完整性 & 接口保护 |

> 系统整体架构（USB Type-C → JMS583 桥接 → M.2 固态）。完整原理图见 [`hardware/`](./hardware) 目录。

![PCB 布局](assets/_0背面效果.jpg)
> PCB 布局与焊接完成效果，可见 JMS583 主控、M.2 插槽与 Type-C 接口的位置关系。

> 详见 [`hardware/`](./hardware) 目录下的原理图、PCB 源文件（如立创 EDA / Altium 工程）。

---

## 实物展示

### 整体效果
![成品效果 1](assets/_8效果1.jpg)
![成品效果 2](assets/_8效果3.jpg)

> 分解 / 爆炸动画见 [`分解效果.mp4`](assets/分解效果.mp4)。

### 速度测试

在 Windows / macOS 下实测，顺序读取可达约 **700MB/s**：

#### CrystalDiskMark（Windows）

![CrystalDiskMark 1GB 测试](assets/_test_CrystalDiskMark_1G.png)
> 1GB 测试块，顺序读取约 400MB/s。

![CrystalDiskMark 64GB 测试](assets/_test_CrystalDiskMark_64GB.png)
> 64GB 大文件测试，验证持续读写稳定性。

![CrystalDiskMark USB3 Gen2 4GB 测试](assets/_test_3gen2_CrystalDiskMark_4GB.png)
> USB 3.1 Gen 2 模式下 4GB 测试块结果。

#### Blackmagic Disk Speed Test（macOS）

![DiskSpeedTest 1GB](assets/_test_DiskSpeedTest_1GB.png)
![DiskSpeedTest 5GB](assets/_test_DiskSpeedTest_5GB.png)
> macOS 下使用 Blackmagic Disk Speed Test 测试（1GB / 5GB 测试文件）。

#### macOS 下实际文件拷贝测试

- 读取 17GB 文件实测约 30 秒：[`_test_mac实际读取17GB文件测试_大概过30s.mov`](assets/_test_mac实际读取17GB文件测试_大概过30s.mov)
- 写入 17GB 文件实测约 52 秒：[`_test_mac实际写入17GB文件测试_大概过52s.mov`](assets/_test_mac实际写入17GB文件测试_大概过52s.mov)

> 实际数值以你的 SSD 型号、线缆质量与主机接口为准。

---

## 快速开始

### 所需材料

- [ ] 焊接好的 PCB（见 [焊接风扇](#焊接风扇)）
- [ ] JMS583 主控及外围元器件
- [ ] M.2 NVMe 固态硬盘一块
- [ ] USB Type-C 数据线（**需支持 USB 3.1 Gen 2 / 10Gbps**，普通 2.0 线缆无法达到标称速度）
- [ ] 外壳
- [ ] 导热垫 / 散热片

### 装配步骤
![背面效果](assets/_0背面效果.jpg)

> 装配流程分解：
![烧录JMS583固件](assets/.jpg)
> 烧录固件。

![烧录stc8g](assets/_1stc8g1k08a单片机烧录.jpg)
> 烧录内置风扇以及呼吸灯的驱动代码。

![焊接风扇](assets/_2焊接风扇.jpg)
> 焊接风扇与散热相关元件。

![绿油固化](assets/_3绿油固化.jpg)
> PCB 绿油固化处理。

![装入外壳](assets/_4装外壳.jpg)
> 将 PCB 装入外壳并固定。

![安装散热片](assets/_5加散热片.jpg)
> 在主控 / 颗粒面贴导热垫并加装散热片。

![加装导流板](assets/_6加导流板.jpg)
> 加装导流板，优化风道。

![安装固态](assets/_7安装固态.jpg)
> 安装 M.2 固态硬盘。


---

## 使用说明

### 制作 PCB

见 [`hardware/`](./hardware) 目录，包含原理图、PCB 源文件与 Gerber。推荐使用嘉立创等平台打样：

1. 直接打印Gerber文件；
2. 按默认工艺下单（板厚 1.6mm，沉金或喷锡均可）；
3. 收到板子后按 BOM 焊接。

### 打印 / 制作外壳

见 [`enclosure/`](./enclosure) 目录，包含可打印文件stl以及源文件。

- 3D 打印建议：下壳和导流板材料随意，上盖使用半透明（8001）或透明；
- 装配时先放安装m2*1的磁铁，用螺丝（m2*3或M2*4）固定pcb，最后是上盖。
- 外壳模型渲染见 [项目简介](#项目简介)（[`渲染图.PNG`](assets/渲染图2.PNG)），成品效果见 [实物展示](#实物展示)（[`_8效果1.jpg`](assets/_8效果1.jpg) 等）。

### 焊接元器件

- 推荐使用**热风枪 + 锡膏**焊接 JMS583（QFN）、M.2接口、typec、小元件；
- Type-C 等密脚元件可用烙铁 + 助焊剂；
- 焊接完成后**先检查短路**（尤其电源与地），再上电。

---

## 固件与 Windows 测试

固件与升级 / 测试工具见 [`firmware/`](./firmware) 目录。

> ⚠️ **升级固件有风险**：刷写错误可能导致设备无法识别，请确认文件与硬件版本匹配后再操作，风险自负。

### Windows 下测试步骤

1. 将硬盘盒接入 Windows PC；
2. 确认磁盘管理器中识别到 SSD，完成初始化 / 格式化（NTFS / exFAT 等）；
3. 运行 CrystalDiskMark 或 AS SSD Benchmark，选择目标盘进行测试；
4. 参考 [实物展示 - 速度测试](#速度测试) 对比读取是否达到约 400MB/s。

![Windows 测试](assets/_test_win_1G.png)
> Windows 下 CrystalDiskMark 识别与速度测试界面示意。

![Windows 5G 测试](assets/_test_win_5G.png)

### 固件升级（如需）

1. 在 [`firmware/`](./firmware) 中选取对应版本的 `.bin` 文件；
2. 使用厂商提供的升级工具（通常需进入升级模式，如短接特定引脚）；
3. 按工具提示完成烧录，**过程中切勿断开连接**。

> 若使用 STC8G1K08A 等辅助单片机，需先用对应烧录工具下载程序：

![STC8G1K08A 单片机烧录](assets/_1stc8g1k08a单片机烧录.jpg)
> STC8G1K08A 烧录接线与过程示意。

---

## 目录结构

```
jms583硬盘盒/
├── Datasheet/   # JMS583 等芯片数据手册 (Datasheet)
├── assets/      # README 引用的图片资源（本文件所用配图）
├── docs/        # 补充文档（说明、规格、测试记录等）
├── enclosure/   # 外壳 3D 模型 / 加工图纸
├── firmware/    # 固件文件 + Windows 测试 / 升级工具
├── hardware/    # 原理图、PCB 源文件与 Gerber
├── LICENSE      # GPL-3.0 许可证
└── README.md    # 本文档
```

---


## 许可证

本项目基于 **GNU General Public License v3.0** 开源，详见 [`LICENSE`](./LICENSE) 文件。

> 硬件设计（原理图、PCB）遵循相同的开源许可，允许个人与商业使用，但需保留署名并按 GPL 开源衍生作品。

## 致谢与参考

- [JMicron JMS583 产品页](https://www.jmicron.com/)
- [嘉立创开源硬件平台 - 本项目](https://oshwhub.com/qiao_wen/jms583-based-hard-disk-box)
- 感谢社区对桥接方案与 PCB 设计的讨论与贡献。

---

> 本文档随项目持续更新。如发现错误或有改进建议，欢迎提 Issue / PR。
��如发现错误或有改进建议，欢迎提 Issue / PR。

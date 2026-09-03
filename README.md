# JMS583 硬盘盒

> 基于 JMS583 主控的 M.2 NVMe 固态硬盘盒开源项目，采用「硬盘盒 + M.2 固态」的组合形式，**顺序读取速度可达 700MB/s**，兼顾大容量与高速读写。

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
  - [整体效果](#整体效果)
  - [速度测试](#速度测试)
- [快速开始](#快速开始)
  - [所需材料](#所需材料)
  - [速度测试](#装配步骤)
- [PCB相关](#PCB相关)
  - [JMS583 桥接固件](#JMS583 固件)
  - [STC8G1K08A 辅助固件](#STC8G1K08A 固件)
- [固件说明](#固件说明)
- [目录结构](#目录结构)
- [开源许可](#开源许可)
- [致谢与参考](#致谢与参考)

---

## 项目简介

传统的移动存储介质，容量与速度难以兼得。本项目以 **JMicron JMS583** USB 3.1 Gen 2 转 PCIe/NVMe 桥接芯片为核心，搭配一块 **M.2 固态硬盘**，实现一个：
- **大容量**：支持 NVMe M.2 SSD，轻松扩展到数 TB；
- **高速度**：USB 3.1 Gen 2（10Gbps）接口，实测**顺序读取约 700MB/s**；
- **扩展性**：比成品硬盘盒更灵活，加入stc8g单片驱动内置风扇，便于学习与二次开发。
- **主要面向**：需要大容量高速移动存储的场景。 
>——锅包肉

![产品整体效果](assets/渲染图2.png)
![成品效果3](assets/_8效果2.jpg)
> 成品整体效果（PCB + 外壳 + M.2 固态装配完成）。

详见 [特性一览](#特性) 与 [实物展示](#实物展示)。

---

## 特性

- 🚀 **主控芯片**：JMicron JMS583（USB 3.1 Gen 2 to NVMe Bridge）
- 💾 **存储接口**：M.2 M-Key（NVMe）
- 🔌 **上行接口**：USB Type-C（USB 3.1 Gen 2，10Gbps）
- ⚡ **性能**：顺序读取可达 **700MB/s**（受 SSD 与线缆质量影响）
- 🧊 **散热**：PCB 预留散热片，内置涡轮风扇，外壳开散热窗
- 🔧 **固件**：提供JMS583固件与 stc8g的驱动
- 📦 **外壳**：3D 打印，装配即用

---

## 硬件架构

系统由 **USB 接口 → JMS583 桥接 → M.2 固态** 三部分组成，整体框图如下：

```
┌─────────────────────────────────────────────┐
│                 硬盘盒 PCB                    │
│                                             │
│   Type-C (数据) ──► JMS583 ──► M.2 NVMe SSD │
│                        │                    │
│   Type-C (供电) ──► 电源管理                 │
│                                             │
│                    STC8G1K08A ◄── 风扇/LED  │
│                                             │
└─────────────────────────────────────────────┘
```

![硬件架构图](assets/架构.png)
> 思路结构。

> 详见 [`hardware/`](./hardware) 目录下的原理图、PCB 源文件（如立创EDA）。

---

## 实物展示

### 整体效果
![成品效果 1](assets/_8效果1.jpg)
![成品效果 2](assets/_8效果3.jpg)

> 分解 / 爆炸动画见 [`分解效果.mp4`](assets/分解效果.mp4)。

### 速度测试

在 Windows / macOS 下实测，顺序读取可达约 **700MB/s**：

#### CrystalDiskMark（Windows）

![CrystalDiskMark USB3 Gen2 4GB 测试](assets/_test_3gen2_CrystalDiskMark_4GB.png)
> USB 3.1 Gen 2 模式下 4GB 测试块结果, 顺序读取约 700MB/s。

![CrystalDiskMark 1GB 测试](assets/_test_CrystalDiskMark_1G.png)
> 1GB 测试块，顺序读取约 400MB/s。

![CrystalDiskMark 64GB 测试](assets/_test_CrystalDiskMark_64GB.png)
> 64GB 大文件测试，验证持续读写稳定性。

![SSD benchmark 1G](assets/_test_win_1G.png)
![SSD benchmark 5G](assets/_test_win_5G.png)
> benchmark，验证持续读写稳定性。

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

## PCB相关

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

## 固件说明

> 本项目包含 **两套固件**，分别对应两个主控：
> 1. **JMS583** 桥接芯片固件（USB ↔ NVMe 桥接逻辑）
> 2. **STC8G1K08A** 单片机固件（风扇 / 呼吸灯 / adc等控制逻辑）

### JMS583 固件

- **作用**：实现 USB 3.2 Gen 2 与 PCIe NVMe 之间的协议桥接。
- **文件**：见仓库 `firmware/jms583/` 目录。
- **升级方式**：通常通过厂商工具FwUpdateTool.exe `firmware/jms583/tools`通过 USB 接口在线升级。
- **注意**：
  - 在刷写的时候看看有没有Flash的信息，jms583支持挺多厂商的，如果没有flash的信息，烧录会是比啊，检查是不是焊接问题（包括那个QFN的焊接）或者是换个芯片。
  - 固件升级过程与本仓库的 STC8G1K08A 烧录是**两个独立流程**，请勿混淆。

### STC8G1K08A 固件

- **作用**：控制散热风扇、WS2812呼吸灯、adc等辅助逻辑。
- **开发环境**：Keil C51。
- **烧录方式**：通过串口（UART）连接 STC8G1K08A 的 **P3.0 (RxD) / P3.1 (TxD)** 引脚，使用 STC-ISP 工具烧录。
- **烧录要点**：stc8g的供电和串口的供电时同一路，见![STC8G1K08A 烧录](assets/_1stc8g1k08a单片机烧录.jpg)

---

## 目录结构

```
jms583硬盘盒/
├── Datasheet/   # JMS583 等芯片数据手册 (Datasheet)
├── assets/      # README 引用的图片资源（本文件所用配图）
├── docs/        # 补充文档（说明、规格、测试记录等）
├── enclosure/   # 外壳 3D 模型 / 加工图纸
├── firmware/    # 两种固件
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

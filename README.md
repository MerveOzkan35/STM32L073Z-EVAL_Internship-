# STM32L073Z-EVAL Embedded Internship Project

This repository contains the firmware and application source code developed during my engineering internship. 

> **Note:** Due to company confidentiality restrictions, certain proprietary files and full project configurations have been omitted. Peripheral configurations can be inferred from the `stm32l0xx_hal_msp.c` file; after replicating these configurations in STM32CubeMX, the provided source files can be integrated into the generated code.

## Project Overview
An environmental monitoring and data logging system designed for cold food and pharmaceutical storage applications, developed on the **STM32L073Z-EVAL** development board.

### Key Features
* **Real-Time Sensor Acquisition:** Continuous monitoring of temperature and pressure data via hardware sensors.
* **Dual Interface Display:** Live data visualization on an onboard TFT LCD and a desktop control panel built with PyQt.
* **Alert & Threshold Mechanisms:** Automated warning triggers when critical environmental thresholds are exceeded.
* **Persistent Logging:** Robust EEPROM-based storage architecture for long-term historical sensor logs.

## Tech Stack & Hardware Architecture

| Domain | Technology / Component | Details & Use Case |
| :--- | :--- | :--- |
| **Microcontroller** | STM32L073VZ (ARM Cortex-M0+) | STM32L073Z-EVAL evaluation board |
| **Communication Protocols** | I2C, SPI, UART | Sensor acquisition, display driver & PC telemetry |
| **Sensors & Peripherals** | Temperature & Pressure Sensors, TFT LCD | Real-time monitoring & local graphical interface |
| **Memory Architecture** | EEPROM (via I2C) | Non-volatile ring buffer / historical data logging |
| **Host Application** | Python, PyQt | Desktop telemetry interface & configuration dashboard |
| **Toolchain & Software** | STM32CubeIDE, STM32CubeMX, HAL Drivers | Firmware development, peripheral init & debugging |

## Key Technical Concepts & Implementation
* **DMA-Driven Telemetry:** Utilized UART with Direct Memory Access (DMA) circular buffering to stream sensor telemetry without CPU overhead.
* **Non-Volatile Storage:** Implemented structured EEPROM memory addressing for cyclic sensor event logs.
* **Low-Power Efficiency:** Leveraged ARM Cortex-M0+ architecture features for low-power cold-chain monitoring.

## Project Setup & Build Configuration

To compile and link the firmware within STM32CubeIDE, the Board Support Packages (BSP) and custom font definitions must be present, and their header search paths must be configured in the compiler settings.

### 1. Required Source & Driver Files
The following external and BSP directories from the STMicroelectronics STM32CubeL0 MCU Package must be included in your project hierarchy:

* **BSP Components:**
  * `Drivers/BSP/Components/Common/`
  * `Drivers/BSP/Components/hx8347d/` (LCD Controller Driver)
  * `Drivers/BSP/Components/mfxstm32l152/` (IO Expander)
  * `Drivers/BSP/STM32L073Z_EVAL/` (Board-specific drivers)
* **Custom Font Tables:**
  * `myUtilities/Fonts/` (Font lookup definitions utilized by the TFT display driver)

> **Note:** If generating a clean workspace via STM32CubeMX, copy the required BSP component drivers and the `myUtilities/Fonts/` directory into your project structure before building.

### 2. Include Paths (STM32CubeIDE)
Ensure the compiler knows where to resolve the header (`.h`) declarations. Add the following paths under:  
**Project Properties** $\rightarrow$ **C/C++ Build** $\rightarrow$ **Settings** $\rightarrow$ **Tool Settings** $\rightarrow$ **MCU GCC Compiler** $\rightarrow$ **Include paths**:

```text
../myUtilities/Fonts
../Drivers/BSP/Components/Common
../Drivers/BSP/Components/hx8347d
../Drivers/BSP/Components/mfxstm32l152
../Drivers/BSP/STM32L073Z_EVAL
```
## PyQt Industrial HMI Terminal

A custom desktop telemetry panel built with PyQt5 and PyQtGraph for real-time sensor monitoring, dynamic threshold configuration, hardware LED status tracking, and EEPROM history export.

### Features
* **Live Telemetry & Graphing:** Real-time plotting of STLM75 I2C temperature and ADC pressure data at 115200 baud.
* **Bi-directional Threshold Control:** Dynamic calibration of temperature and pressure limits synchronized with the STM32 firmware.
* **Watchdog Supervision:** UI connection timeout detection (turns off indicator LEDs if packet stream halts).
* **EEPROM Memory Dump & Export:** Retrieves non-volatile ring-buffer logs from the microcontroller and exports them to structured CSV files.

---

### UART Communication Protocol

The application communicates with the STM32 microcontroller using a structured framed binary protocol and standard ASCII commands:

#### 1. Inbound Telemetry Packet (STM32 $\rightarrow$ Host)
Fixed-length 13-byte frame emitted periodically:
* **Header:** `0xAA 0x55` (2 bytes)
* **Command ID:** `0x01` (Telemetry, 1 byte)
* **Temperature:** `pkt[3] + (pkt[4] / 10.0)` (°C)
* **Pressure:** 32-bit big-endian unsigned integer scaled by 100.0 (`(pkt[5]<<24 | pkt[6]<<16 | pkt[7]<<8 | pkt[8]) / 100.0` hPa)
* **Active Thresholds:** Scaled temperature (`pkt[9] / 2.0`) and pressure offset (`pkt[10] + 900.0`)
* **Emergency Status:** `pkt[11]` (`0x01`: Emergency Active, `0x00`: Normal)
* **Footer:** `0x0D` (`\r` delimiter for frame validation)

#### 2. Outbound Threshold Command (Host $\rightarrow$ STM32)
6-byte binary payload:
`[ 0xAA, 0x55, 0x04, Temp_Max_Scaled, Press_Max_Offset, 0x0D ]`

#### 3. Log Dump Command
ASCII string command `eeprom\r\n` requests raw historical CSV rows formatted as:  
`ID, Timestamp (RTC), Temperature (°C), Pressure (hPa)`

---

### Installation & Quick Start

Install dependencies:

``` Bash
pip install -r requirements.txt
```

Launch the terminal:

``` Bash
python main.py
```


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

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

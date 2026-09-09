#ifndef INC_STLM75_H_
#define INC_STLM75_H_
#include "stm32l0xx_hal.h"
#include <stdio.h>
#include <string.h>

#define STLM75_ADDR            (0x92) // 7-bit adres 0x48, HAL için 1-bit sola kaydırılır (0x90)
#define STLM75_REG_TEMP        0x00        // Sıcaklık register adresi
typedef struct {
   I2C_HandleTypeDef *hi2c;   // Kullanılacak I2C Handleri
   UART_HandleTypeDef *huart; // Kullanılacak UART Handleri
   uint8_t txBuffer[64];      // DMA ile gönderilecek string tamponu
   float temperature;         // Okunan sıcaklık değeri (Celsius)
} STLM75_HandleTypeDef;

HAL_StatusTypeDef STLM75_Init(STLM75_HandleTypeDef *dev, I2C_HandleTypeDef *hi2c, UART_HandleTypeDef *huart);
HAL_StatusTypeDef STLM75_ReadTemperature(STLM75_HandleTypeDef *dev);
HAL_StatusTypeDef STLM75_SendTempViaUART_DMA(STLM75_HandleTypeDef *dev);
#endif /* INC_STLM75_H_ */

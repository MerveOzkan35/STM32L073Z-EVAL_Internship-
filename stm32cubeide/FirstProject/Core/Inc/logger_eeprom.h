#ifndef __LOGGER_EEPROM_H
#define __LOGGER_EEPROM_H
#include "stm32l0xx_hal.h"
#define EEPROM_I2C_ADDR      0xA6
#define RECORD_SIZE         12      // 6 bayt tarih + 4 bayt temp + 2 bayt press
#define EEPROM_MAX_RECORDS  400     // EEPROM 8 KB, = 8192 byte. Yazma indeksi 4 byte -> 8188
									// her paket 12 byte
									// 8022 / 16 = 682 paket
									//Belleği zorlamamak için 400-500 max records 
static HAL_StatusTypeDef EEPROM_WriteBytes(uint16_t mem_addr, uint8_t *pData, uint16_t size);
void Logger_Init(I2C_HandleTypeDef *hi2c, RTC_HandleTypeDef *hrtc);
void Logger_GetFormattedTimeString(char *buf_time, char *buf_date);
void Logger_SaveData(float temp, uint16_t press);
void Logger_ExportToUART(UART_HandleTypeDef *huart);
#endif /* __LOGGER_EEPROM_H */

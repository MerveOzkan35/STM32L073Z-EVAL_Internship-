#include "logger_eeprom.h"
#include <stdio.h>
#include <string.h>
static I2C_HandleTypeDef *g_hi2c;
static RTC_HandleTypeDef *g_hrtc;
static uint32_t g_write_index = 0;
// M24LR64 için 4-byte sayfa hizalamalı kayıt boyutu (12 byte veri + 4 byte padding)
#define ALIGNED_RECORD_SIZE 16
#define M24LR_PAGE_SIZE     4
/* M24LR64 Güvenli Yazma (Sayfa Taşkınını / Roll-over Önler) */
static HAL_StatusTypeDef EEPROM_WriteBytes(uint16_t mem_addr, uint8_t *pData, uint16_t size)
{
   HAL_StatusTypeDef status = HAL_OK;
   uint16_t bytes_written = 0;
   while (bytes_written < size)
   {
       uint16_t current_addr = mem_addr + bytes_written;
       uint16_t page_offset = current_addr % M24LR_PAGE_SIZE;
       uint16_t chunk_size = M24LR_PAGE_SIZE - page_offset;
       if (chunk_size > (size - bytes_written))
       {
           chunk_size = size - bytes_written;
       }
       status = HAL_I2C_Mem_Write(g_hi2c, EEPROM_I2C_ADDR, current_addr, I2C_MEMADD_SIZE_16BIT, &pData[bytes_written], chunk_size, 100);
       if (status != HAL_OK) return status;
       HAL_Delay(6); // M24LR Write Cycle Time (tW = 5ms)
       bytes_written += chunk_size;
   }
   return HAL_OK;
}
void Logger_Init(I2C_HandleTypeDef *hi2c, RTC_HandleTypeDef *hrtc)
{
   g_hi2c = hi2c;
   g_hrtc = hrtc;
   uint8_t idx_buf[4];
   if (HAL_I2C_Mem_Read(g_hi2c, EEPROM_I2C_ADDR, 0x0000, I2C_MEMADD_SIZE_16BIT, idx_buf, 4, 100) == HAL_OK)
   {
       g_write_index = (idx_buf[0] << 24) | (idx_buf[1] << 16) | (idx_buf[2] << 8) | idx_buf[3];
       if (g_write_index >= 500) // Sınır kontrolü (8192/16 = 512)
       {
           g_write_index = 0;
           memset(idx_buf, 0, 4);
           EEPROM_WriteBytes(0x0000, idx_buf, 4);
       }
   }
}
void Logger_GetFormattedTimeString(char *buf_time, char *buf_date)
{
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  /* STM32 RTC Kuralı: Önce Time sonra Date okunmalıdır! */
  HAL_RTC_GetTime(g_hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(g_hrtc, &sDate, RTC_FORMAT_BIN);
  sprintf(buf_time, "%02d:%02d", sTime.Hours, sTime.Minutes);
  sprintf(buf_date, "%02d/%02d/20%02d", sDate.Date, sDate.Month, sDate.Year);
}
void Logger_SaveData(float temp, uint16_t press)
{
   RTC_TimeTypeDef sTime;
   RTC_DateTypeDef sDate;
   HAL_RTC_GetTime(g_hrtc, &sTime, RTC_FORMAT_BIN);
   HAL_RTC_GetDate(g_hrtc, &sDate, RTC_FORMAT_BIN);
   uint8_t buffer[ALIGNED_RECORD_SIZE];
   memset(buffer, 0x00, ALIGNED_RECORD_SIZE);
   buffer[0] = sDate.Year;
   buffer[1] = sDate.Month;
   buffer[2] = sDate.Date;
   buffer[3] = sTime.Hours;
   buffer[4] = sTime.Minutes;
   buffer[5] = sTime.Seconds;
   // Float Sıcaklık (4 bayt)
   uint32_t temp_raw;
   memcpy(&temp_raw, &temp, sizeof(float));
   buffer[6] = (temp_raw >> 24) & 0xFF;
   buffer[7] = (temp_raw >> 16) & 0xFF;
   buffer[8] = (temp_raw >> 8) & 0xFF;
   buffer[9] = temp_raw & 0xFF;
   // Basınç (2 bayt)
   buffer[10] = (press >> 8) & 0xFF;
   buffer[11] = press & 0xFF;
   // 16-byte hizalanmış adres hesaplama (İlk 16 byte indeks ve header için ayrılır)
   uint16_t eeprom_mem_addr = 16 + (g_write_index * ALIGNED_RECORD_SIZE);
   // Güvenli sayfa yazımı
   if (EEPROM_WriteBytes(eeprom_mem_addr, buffer, ALIGNED_RECORD_SIZE) == HAL_OK)
   {
       g_write_index = (g_write_index + 1) % 500;
       uint8_t idx_buf[4];
       idx_buf[0] = (g_write_index >> 24) & 0xFF;
       idx_buf[1] = (g_write_index >> 16) & 0xFF;
       idx_buf[2] = (g_write_index >> 8) & 0xFF;
       idx_buf[3] = g_write_index & 0xFF;
       EEPROM_WriteBytes(0x0000, idx_buf, 4);
   }
}
void Logger_ExportToUART(UART_HandleTypeDef *huart)
{
   char tx_buf[128];
   while (huart->gState != HAL_UART_STATE_READY) { HAL_Delay(1); }
   sprintf(tx_buf, "\r\n--- EEPROM KAYITLI DATA LOG LISTESI (CSV) ---\r\nTarih,Saat,Sicaklik(C),Basinc(hPa)\r\n");
   HAL_UART_Transmit(huart, (uint8_t*)tx_buf, strlen(tx_buf), 200);
   uint16_t valid_records = 0;
   for (int i = 0; i < 500; i++)
   {
       uint16_t mem_addr = 16 + (i * ALIGNED_RECORD_SIZE);
       uint8_t buf[ALIGNED_RECORD_SIZE];
       if (HAL_I2C_Mem_Read(g_hi2c, EEPROM_I2C_ADDR, mem_addr, I2C_MEMADD_SIZE_16BIT, buf, ALIGNED_RECORD_SIZE, 100) == HAL_OK)
       {
           // Yıl, Ay, Gün veya Saat geçersizse (0xFF) henüz yazılmamış veya boş alandır
           if (buf[0] == 0xFF || buf[1] == 0xFF || buf[1] > 12 || buf[3] > 23)
           {
               continue;
           }
           uint32_t temp_raw = ((uint32_t)buf[6] << 24) | ((uint32_t)buf[7] << 16) | ((uint32_t)buf[8] << 8) | buf[9];
           float temp;
           memcpy(&temp, &temp_raw, sizeof(float));
           uint16_t press = ((uint16_t)buf[10] << 8) | buf[11];
           // Makul aralık denetimi (Sıcaklık -50 ile +100 C arası, Basınç 300-1200 hPa arası)
           if (temp < -50.0f || temp > 100.0f) continue;
           sprintf(tx_buf, "20%02d-%02d-%02d,%02d:%02d:%02d,%.2f,%d\r\n",
                   buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], temp, press);
           HAL_UART_Transmit(huart, (uint8_t*)tx_buf, strlen(tx_buf), 200);
           valid_records++;
           HAL_Delay(2);
       }
   }
   if (valid_records == 0)
   {
       sprintf(tx_buf, "[BILGI]: EEPROM icinde henüz gecerli kayitli veri bulunamadi!\r\n");
       HAL_UART_Transmit(huart, (uint8_t*)tx_buf, strlen(tx_buf), 100);
   }
   sprintf(tx_buf, "--- AKTARIM TAMAMLANDI ---\r\n");
   HAL_UART_Transmit(huart, (uint8_t*)tx_buf, strlen(tx_buf), 100);
}

#include <stlm75.h>
/**
 * @brief  STLM75 Sensörünü başlatır ve UART/I2C yapısını baglar.
 */
HAL_StatusTypeDef STLM75_Init(STLM75_HandleTypeDef *dev, I2C_HandleTypeDef *hi2c, UART_HandleTypeDef *huart) {
   if (dev == NULL || hi2c == NULL || huart == NULL) {
       return HAL_ERROR;
   }
   dev->hi2c = hi2c;
   dev->huart = huart;
   dev->temperature = 0.0f;
   memset(dev->txBuffer, 0, sizeof(dev->txBuffer));
   // Sensörün I2C üzerinde hatta olup olmadığını kontrol et
   return HAL_I2C_IsDeviceReady(dev->hi2c, STLM75_ADDR, 2, 100);
}
/**
 * @brief  STLM75'ten 2 byte okuyarak sıcaklık değerini hesaplar.
 */
HAL_StatusTypeDef STLM75_ReadTemperature(STLM75_HandleTypeDef *dev) {
   uint8_t rawData[2] = {0};
   HAL_StatusTypeDef status;
   // Register seçimi ve 2 byte okuma
   status = HAL_I2C_Mem_Read(dev->hi2c, STLM75_ADDR, STLM75_REG_TEMP, I2C_MEMADD_SIZE_8BIT, rawData, 2, HAL_MAX_DELAY);
   if (status == HAL_OK) {
       // STLM75 9-bit çözünürlük kullanır (MSB tam derece, LSB en yüksek biti 0.5 derece)
       int16_t tempRaw = (rawData[0] << 8) | rawData[1];
       dev->temperature = (float)(tempRaw >> 7) * 0.5f;
   }
   return status;
}
/**
 * @brief  Hesaplanan sıcaklık değerini formatlayıp UART4 + DMA ile gönderir.
 */
HAL_StatusTypeDef STLM75_SendTempViaUART_DMA(STLM75_HandleTypeDef *dev) {
   // UART DMA meşgulse beklemeden çakışmayı önle
   if (dev->huart->gState == HAL_UART_STATE_BUSY_TX) {
       return HAL_BUSY;
   }
   // String formatlama
   int len = snprintf((char*)dev->txBuffer, sizeof(dev->txBuffer), "Sicaklik: %.2f C\r\n", dev->temperature);
   // Non-blocking DMA ile gönderim
   return HAL_UART_Transmit_DMA(dev->huart, dev->txBuffer, len);
}

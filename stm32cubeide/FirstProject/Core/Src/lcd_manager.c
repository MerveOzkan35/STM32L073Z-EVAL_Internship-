#include "lcd_manager.h"
/**
 * @brief  BSP LCD Donanımını başlatır ve ekranı temizler.
 */
HAL_StatusTypeDef LCD_Manager_Init(void) {
   /* BSP Sürücüsünü Başlat (Fonksiyon void döner) */
   BSP_LCD_GLASS_Init();
   /* Ekranı Temizle */
   BSP_LCD_GLASS_Clear();
   return HAL_OK;
}
/**
 * @brief  Sıcaklık Değerini BSP Kütüphanesi İle Ekrana Yazar
 * @param  temp: Sensörden gelen float sıcaklık verisi (Örn: 26.5)
 */
void LCD_Manager_ShowTemperature(float temp) {
   char lcdBuffer[8];
   // BSP LCD Camı maksimum 4-6 karakter gösterdiği için sade format: "26.5C" veya "26 C"
   snprintf(lcdBuffer, sizeof(lcdBuffer), "%.1fC", temp);
   BSP_LCD_GLASS_Clear();
   BSP_LCD_GLASS_DisplayString((uint8_t *)lcdBuffer);
}
/**
 * @brief  İleride ekleyeceğiniz Basınç Değerini Ekrana Yazar
 * @param  pressure: Basınç verisi (Örn: 1013 hPa)
 */
void LCD_Manager_ShowPressure(float pressure) {
   char lcdBuffer[8];
   snprintf(lcdBuffer, sizeof(lcdBuffer), "%.0fP", pressure);
   BSP_LCD_GLASS_Clear();
   BSP_LCD_GLASS_DisplayString((uint8_t *)lcdBuffer);
}
/**
 * @brief  Ekrana İstediğiniz Özel Metni Yazar
 * @param  text: Gönderilecek string (Örn: "TEST", "WARN")
 */
void LCD_Manager_ShowText(const char *text) {
   BSP_LCD_GLASS_Clear();
   BSP_LCD_GLASS_DisplayString((uint8_t *)text);
}

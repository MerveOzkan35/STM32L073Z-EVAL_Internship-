#ifndef TFT_MANAGER_H
#define TFT_MANAGER_H
#include "stm32l0xx_hal.h"
#include "stm32l073z_eval_lcd.h" 
#include <stdio.h>
/* UI ve Menü Tip Tanımları */
typedef enum {
   PAGE_MAIN_MENU = 0,
   PAGE_TEMP,
   PAGE_PRESSURE,
   PAGE_SET_RTC,
   PAGE_SET_THRESHOLDS
} PageState_t;
typedef enum {
   BTN_TEMP = 0,
   BTN_PRESS,
   BTN_SET_RTC,
   BTN_SET_THRESH,
   MENU_ITEM_COUNT
} MenuSelection_t;
// Fonksiyon Prototipleri
uint8_t TFT_Manager_Init(void);
void TFT_Manager_ClearContentArea(void);
void TFT_Manager_DrawHeaderWithTime(char *time_str, char *date_str, const char *title);
void TFT_Manager_DrawMainMenu(MenuSelection_t selection, uint8_t scroll_offset);
void TFT_Manager_ShowTemperaturePage(float temp);
void TFT_Manager_ShowPressurePage(float press);
void TFT_Manager_ShowSetRTCPage(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime, uint8_t active_field);
void TFT_Manager_ShowSetThresholdsPage(float temp_max, float press_max, uint8_t active_field);
#endif

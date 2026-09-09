#ifndef UI_MANAGER_H
#define UI_MANAGER_H
#include <stlm75.h>
#include "stm32l0xx_hal.h"
#include "tft_manager.h"
#include "rtc_app.h"
#include "pressure_sensor.h"
#include "stm32l073z_eval.h"
// Dışarıdan veya Konfigürasyondan Erişim Sağlanacak Sistem Verileri
typedef struct {
   float max_temp_threshold;
   float max_press_threshold;
   PageState_t current_page;
   MenuSelection_t menu_selection;
   uint8_t scroll_offset;
   uint8_t rtc_edit_field;      // 0:Date, 1:Month, 2:Year, 3:Hour, 4:Min
   uint8_t thresh_edit_field;   // 0: Temp, 1: Press
   RTC_TimeTypeDef edit_Time;
   RTC_DateTypeDef edit_Date;
} UI_SystemConfig_t;
extern UI_SystemConfig_t g_ui_config;
/* Fonksiyon Protokolleri */
void UI_Manager_Init(void);
void UI_Manager_ProcessInput(JOYState_TypeDef joy_state, RTC_TimeTypeDef *curr_time, RTC_DateTypeDef *curr_date);
void UI_Manager_UpdateHeaderIfNeeded(RTC_TimeTypeDef *curr_time, RTC_DateTypeDef *curr_date);
void UI_Manager_RenderPeriodicPages(float temp, float press);
#endif /* UI_MANAGER_H */

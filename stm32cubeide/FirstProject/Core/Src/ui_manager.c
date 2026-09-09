#include "ui_manager.h"
// Başlangıç değerlerini doğrudan tanımda veriyoruz
UI_SystemConfig_t g_ui_config = {
   .max_temp_threshold = 35.0f,
   .max_press_threshold = 1050.0f,
   .current_page = PAGE_MAIN_MENU,
   .menu_selection = BTN_TEMP,
   .scroll_offset = 0,
   .rtc_edit_field = 0,
   .thresh_edit_field = 0
};
static uint8_t s_last_drawn_minute = 255;
static PageState_t s_last_drawn_page = (PageState_t)255;
static JOYState_TypeDef s_prev_joy_state = JOY_NONE; // Sayfa geçişinde JOY_SEL yakalamak için
void UI_Manager_Init(void)
{
   // Değerlerin sıfırlanmadığından emin olma
   if (g_ui_config.max_temp_threshold == 0.0f) {
       g_ui_config.max_temp_threshold = 35.0f;
   }
   if (g_ui_config.max_press_threshold == 0.0f) {
       g_ui_config.max_press_threshold = 1050.0f;
   }
   g_ui_config.current_page = PAGE_MAIN_MENU;
   g_ui_config.menu_selection = BTN_TEMP;
   g_ui_config.scroll_offset = 0;
   g_ui_config.rtc_edit_field = 0;
   g_ui_config.thresh_edit_field = 0;
   // Önce ekranın altını temizle, ardından menüyü çiz
   TFT_Manager_ClearContentArea();
   TFT_Manager_DrawMainMenu(g_ui_config.menu_selection, g_ui_config.scroll_offset);
}
void UI_Manager_UpdateHeaderIfNeeded(RTC_TimeTypeDef *curr_time, RTC_DateTypeDef *curr_date)
{
   if (curr_time->Minutes != s_last_drawn_minute || g_ui_config.current_page != s_last_drawn_page)
   {
       s_last_drawn_minute = curr_time->Minutes;
       s_last_drawn_page = g_ui_config.current_page;
       char time_tft[8], time_full[10], date_str[12];
       RTC_App_GetFormattedStrings(time_tft, time_full, date_str);
       const char *page_title = "SISTEM MENU";
       switch (g_ui_config.current_page)
       {
           case PAGE_TEMP:           page_title = "SICAKLIK"; break;
           case PAGE_PRESSURE:       page_title = "BASINC"; break;
           case PAGE_SET_RTC:        page_title = "RTC AYAR"; break;
           case PAGE_SET_THRESHOLDS: page_title = "ESIK AYAR"; break;
           default: break;
       }
       TFT_Manager_DrawHeaderWithTime(time_tft, date_str, page_title);
   }
}
void UI_Manager_ProcessInput(JOYState_TypeDef joy_state, RTC_TimeTypeDef *curr_time, RTC_DateTypeDef *curr_date)
{
   // JOY_SEL tuşuna menüde basıldığında yeni sayfaya geçip aynı basışta alttaki seçeneği değiştirmemek için edge kontrolü:
   uint8_t is_new_press = (joy_state != s_prev_joy_state);
   s_prev_joy_state = joy_state;
   if (joy_state == JOY_NONE) {
       return; // Tuşa basılmıyorsa çık
   }
   switch (g_ui_config.current_page)
   {
       case PAGE_MAIN_MENU:
           if (joy_state == JOY_DOWN && is_new_press) {
               if (g_ui_config.menu_selection < MENU_ITEM_COUNT - 1) {
                   g_ui_config.menu_selection++;
                   if (g_ui_config.menu_selection >= g_ui_config.scroll_offset + 2) {
                       g_ui_config.scroll_offset++;
                   }
                   TFT_Manager_DrawMainMenu(g_ui_config.menu_selection, g_ui_config.scroll_offset);
               }
           } else if (joy_state == JOY_UP && is_new_press) {
               if (g_ui_config.menu_selection > 0) {
                   g_ui_config.menu_selection--;
                   if (g_ui_config.menu_selection < g_ui_config.scroll_offset) {
                       g_ui_config.scroll_offset--;
                   }
                   TFT_Manager_DrawMainMenu(g_ui_config.menu_selection, g_ui_config.scroll_offset);
               }
           } else if (joy_state == JOY_SEL && is_new_press) {
               TFT_Manager_ClearContentArea();
               switch (g_ui_config.menu_selection) {
                   case BTN_TEMP:
                       g_ui_config.current_page = PAGE_TEMP;
                       break;
                   case BTN_PRESS:
                       g_ui_config.current_page = PAGE_PRESSURE;
                       break;
                   case BTN_SET_RTC:
                       g_ui_config.current_page = PAGE_SET_RTC;
                       g_ui_config.edit_Time = *curr_time;
                       g_ui_config.edit_Date = *curr_date;
                       g_ui_config.rtc_edit_field = 0; // İlk alan (Gün) seçili
                       TFT_Manager_ShowSetRTCPage(&g_ui_config.edit_Date, &g_ui_config.edit_Time, g_ui_config.rtc_edit_field);
                       break;
                   case BTN_SET_THRESH:
                       g_ui_config.current_page = PAGE_SET_THRESHOLDS;
                       g_ui_config.thresh_edit_field = 0; // İlk alan (Sıcaklık) seçili
                       
                       TFT_Manager_ShowSetThresholdsPage(g_ui_config.max_temp_threshold, g_ui_config.max_press_threshold, g_ui_config.thresh_edit_field);
                       break;
                   default: break;
               }
           }
           break;
       case PAGE_SET_RTC:
           // JOY_SEL alan değiştirme sadece YENİ BİR BASIŞTA tetiklenir 
           if (joy_state == JOY_SEL && is_new_press) {
               g_ui_config.rtc_edit_field = (g_ui_config.rtc_edit_field + 1) % 5;
               TFT_Manager_ShowSetRTCPage(&g_ui_config.edit_Date, &g_ui_config.edit_Time, g_ui_config.rtc_edit_field);
           }
           // UP / DOWN tuşlarında is_new_press şartı YOKTUR, böylece UZUN BASMADA hızlı artış/azalış çalışmaya devam eder
           else if (joy_state == JOY_UP) {
               switch (g_ui_config.rtc_edit_field) {
                   case 0: g_ui_config.edit_Date.Date = (g_ui_config.edit_Date.Date % 31) + 1; break;
                   case 1: g_ui_config.edit_Date.Month = (g_ui_config.edit_Date.Month % 12) + 1; break;
                   case 2: g_ui_config.edit_Date.Year = (g_ui_config.edit_Date.Year + 1) % 100; break;
                   case 3: g_ui_config.edit_Time.Hours = (g_ui_config.edit_Time.Hours + 1) % 24; break;
                   case 4: g_ui_config.edit_Time.Minutes = (g_ui_config.edit_Time.Minutes + 1) % 60; break;
               }
               TFT_Manager_ShowSetRTCPage(&g_ui_config.edit_Date, &g_ui_config.edit_Time, g_ui_config.rtc_edit_field);
           } else if (joy_state == JOY_DOWN) {
               switch (g_ui_config.rtc_edit_field) {
                   case 0: if (g_ui_config.edit_Date.Date > 1) g_ui_config.edit_Date.Date--; break;
                   case 1: if (g_ui_config.edit_Date.Month > 1) g_ui_config.edit_Date.Month--; break;
                   case 2: if (g_ui_config.edit_Date.Year > 0) g_ui_config.edit_Date.Year--; break;
                   case 3: if (g_ui_config.edit_Time.Hours > 0) g_ui_config.edit_Time.Hours--; break;
                   case 4: if (g_ui_config.edit_Time.Minutes > 0) g_ui_config.edit_Time.Minutes--; break;
               }
               TFT_Manager_ShowSetRTCPage(&g_ui_config.edit_Date, &g_ui_config.edit_Time, g_ui_config.rtc_edit_field);
           } else if (joy_state == JOY_LEFT && is_new_press) { // Kaydet ve çık
               RTC_App_SetTimeAndDate(g_ui_config.edit_Time.Hours, g_ui_config.edit_Time.Minutes,
                                      g_ui_config.edit_Date.Date, g_ui_config.edit_Date.Month,
                                      g_ui_config.edit_Date.Year);
               TFT_Manager_ClearContentArea();
               g_ui_config.current_page = PAGE_MAIN_MENU;
               TFT_Manager_DrawMainMenu(g_ui_config.menu_selection, g_ui_config.scroll_offset);
           }
           break;
       case PAGE_SET_THRESHOLDS:
           if (joy_state == JOY_SEL && is_new_press) {
               g_ui_config.thresh_edit_field = (g_ui_config.thresh_edit_field == 0) ? 1 : 0;
               TFT_Manager_ShowSetThresholdsPage(g_ui_config.max_temp_threshold, g_ui_config.max_press_threshold, g_ui_config.thresh_edit_field);
           }
           // UP ve DOWN için is_new_press YOK -> UZUN BASMADA SAYILAR HIZLI ARTAR
           else if (joy_state == JOY_UP) {
               if (g_ui_config.thresh_edit_field == 0) g_ui_config.max_temp_threshold += 0.5f;
               else g_ui_config.max_press_threshold += 5.0f;
               TFT_Manager_ShowSetThresholdsPage(g_ui_config.max_temp_threshold, g_ui_config.max_press_threshold, g_ui_config.thresh_edit_field);
           } else if (joy_state == JOY_DOWN) {
               if (g_ui_config.thresh_edit_field == 0) g_ui_config.max_temp_threshold -= 0.5f;
               else g_ui_config.max_press_threshold -= 5.0f;
               TFT_Manager_ShowSetThresholdsPage(g_ui_config.max_temp_threshold, g_ui_config.max_press_threshold, g_ui_config.thresh_edit_field);
           } else if (joy_state == JOY_LEFT && is_new_press) {
               TFT_Manager_ClearContentArea();
               g_ui_config.current_page = PAGE_MAIN_MENU;
               TFT_Manager_DrawMainMenu(g_ui_config.menu_selection, g_ui_config.scroll_offset);
           }
           break;
       default: // PAGE_TEMP, PAGE_PRESSURE
           if (joy_state == JOY_LEFT && is_new_press) {
               TFT_Manager_ClearContentArea();
               g_ui_config.current_page = PAGE_MAIN_MENU;
               TFT_Manager_DrawMainMenu(g_ui_config.menu_selection, g_ui_config.scroll_offset);
           }
           break;
   }
}
void UI_Manager_RenderPeriodicPages(float temp, float press)
{
   if (g_ui_config.current_page == PAGE_TEMP) {
       TFT_Manager_ShowTemperaturePage(temp);
   } else if (g_ui_config.current_page == PAGE_PRESSURE) {
       TFT_Manager_ShowPressurePage(press);
   }else if (g_ui_config.current_page == PAGE_SET_THRESHOLDS)
   {
       // Python'dan gelen yeni g_ui_config değerleriyle ekranı anlık yenile
       TFT_Manager_ShowSetThresholdsPage(g_ui_config.max_temp_threshold,
                                          g_ui_config.max_press_threshold,
                                          g_ui_config.thresh_edit_field);
   }
}

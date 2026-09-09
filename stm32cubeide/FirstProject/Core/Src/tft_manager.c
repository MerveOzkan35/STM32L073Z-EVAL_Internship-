#include "tft_manager.h"
#include <string.h>
#define GET_BTN_TEXT_X(btn_x, btn_w, text_len) \
   ((btn_x) + (((btn_w) - ((text_len) * 11)) / 2))
static float last_displayed_temp = -999.0f;
static float last_displayed_press = -999.0f;
static const char* menu_labels[MENU_ITEM_COUNT] = {
   "1. SICAKLIK",
   "2. BASINC",
   "3. RTC AYARLA",
   "4. ESIK AYARLA"
};
uint8_t TFT_Manager_Init(void)
{
   if (BSP_LCD_Init() != LCD_OK) return 0;
   BSP_LCD_Clear(LCD_COLOR_WHITE);
   return 1;
}
void TFT_Manager_ClearContentArea(void)
{
   BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
   BSP_LCD_FillRect(0, 41, BSP_LCD_GetXSize(), BSP_LCD_GetYSize() - 41);
   last_displayed_temp = -999.0f;
   last_displayed_press = -999.0f;
}
void TFT_Manager_DrawHeaderWithTime(char *time_str, char *date_str, const char *title)
{
   BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
   BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), 40);
   BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
   BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
   BSP_LCD_SetFont(&Font12);
   BSP_LCD_DisplayStringAt(5, 5, (uint8_t*)date_str, LEFT_MODE);
   BSP_LCD_DisplayStringAt(280, 5, (uint8_t*)time_str, LEFT_MODE);
   BSP_LCD_SetFont(&Font16);
   BSP_LCD_DisplayStringAt(0, 20, (uint8_t*)title, CENTER_MODE);
}
/* 2 EKRANLIK SCROLL DESTEKLİ ANA MENÜ */
void TFT_Manager_DrawMainMenu(MenuSelection_t selection, uint8_t scroll_offset)
{
   uint16_t btn_width  = 220;
   uint16_t btn_height = 45;
   uint16_t x_pos      = (BSP_LCD_GetXSize() - btn_width) / 2;
   uint16_t y_positions[2] = {60, 125}; // Ekranda aynı anda görünen 2 buton konumu
   TFT_Manager_ClearContentArea();
   for (uint8_t i = 0; i < 2; i++)
   {
       uint8_t item_index = scroll_offset + i;
       if (item_index >= MENU_ITEM_COUNT) break;
       uint16_t y_pos = y_positions[i];
       if (item_index == (uint8_t)selection)
       {
           BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
           BSP_LCD_FillRect(x_pos, y_pos, btn_width, btn_height);
           BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
           BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
       }
       else
       {
           BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
           BSP_LCD_FillRect(x_pos, y_pos, btn_width, btn_height);
           BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
           BSP_LCD_DrawRect(x_pos, y_pos, btn_width, btn_height);
           BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
           BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
       }
       BSP_LCD_SetFont(&Font16);
       uint16_t x_text = GET_BTN_TEXT_X(x_pos, btn_width, strlen(menu_labels[item_index]));
       BSP_LCD_DisplayStringAt(x_text, y_pos + 14, (uint8_t*)menu_labels[item_index], LEFT_MODE);
   }
   // Scroll Göstergesi (Sağ tarafa küçük oklar)
   BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
   BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
   BSP_LCD_SetFont(&Font12);
   if (scroll_offset > 0) BSP_LCD_DisplayStringAt(300, 50, (uint8_t*)"^", LEFT_MODE);
   if (scroll_offset + 2 < MENU_ITEM_COUNT) BSP_LCD_DisplayStringAt(300, 180, (uint8_t*)"v", LEFT_MODE);
   BSP_LCD_DisplayStringAt(0, 215, (uint8_t*)"[Joy/Slider] Sec | [OK] Giris", CENTER_MODE);
}
void TFT_Manager_ShowTemperaturePage(float temp)
{
   if (temp == last_displayed_temp) return;
   last_displayed_temp = temp;
   char buf[20];
   snprintf(buf, sizeof(buf), "%.1f C", temp);
   BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
   BSP_LCD_SetTextColor((temp > 35.0f) ? LCD_COLOR_RED : LCD_COLOR_GREEN);
   BSP_LCD_SetFont(&Font24);
   BSP_LCD_DisplayStringAt(0, 100, (uint8_t*)buf, CENTER_MODE);
   BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
   BSP_LCD_SetFont(&Font12);
   BSP_LCD_DisplayStringAt(0, 210, (uint8_t*)"[Sol] Ana Menu", CENTER_MODE);
}
void TFT_Manager_ShowPressurePage(float press)
{
   if (press == last_displayed_press) return;
   last_displayed_press = press;
   char buf[20];
   snprintf(buf, sizeof(buf), "%.2f hPa", press);
   BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
   BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
   BSP_LCD_SetFont(&Font24);
   BSP_LCD_DisplayStringAt(0, 100, (uint8_t*)buf, CENTER_MODE);
   BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
   BSP_LCD_SetFont(&Font12);
   BSP_LCD_DisplayStringAt(0, 210, (uint8_t*)"[Sol] Ana Menu", CENTER_MODE);
}
/* RTC SET EKRANI */
void TFT_Manager_ShowSetRTCPage(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime, uint8_t active_field)
{
   char buf[10];
   uint16_t x_start = 50; // Sol başlangıç X offset'i (320x240 ekranda ortalamak için)
   uint16_t char_w = 11;  // Font16 karakter genişliği (piksel)
   BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
   BSP_LCD_SetFont(&Font16);
   /* --- TARİH SATIRI (Y = 80) --- */
   // "Tarih: "
   BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
   BSP_LCD_DisplayStringAt(x_start, 80, (uint8_t*)"Tarih: ", LEFT_MODE);
   uint16_t x_cursor = x_start + (7 * char_w);
   // Gün (active_field == 0)
   snprintf(buf, sizeof(buf), "%02d", sDate->Date);
   BSP_LCD_SetTextColor((active_field == 0) ? LCD_COLOR_RED : LCD_COLOR_BLACK);
   BSP_LCD_DisplayStringAt(x_cursor, 80, (uint8_t*)buf, LEFT_MODE);
   x_cursor += (2 * char_w);
   // Ayrac "/"
   BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
   BSP_LCD_DisplayStringAt(x_cursor, 80, (uint8_t*)"/", LEFT_MODE);
   x_cursor += char_w;
   // Ay (active_field == 1)
   snprintf(buf, sizeof(buf), "%02d", sDate->Month);
   BSP_LCD_SetTextColor((active_field == 1) ? LCD_COLOR_RED : LCD_COLOR_BLACK);
   BSP_LCD_DisplayStringAt(x_cursor, 80, (uint8_t*)buf, LEFT_MODE);
   x_cursor += (2 * char_w);
   // Ayrac "/20"
   BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
   BSP_LCD_DisplayStringAt(x_cursor, 80, (uint8_t*)"/20", LEFT_MODE);
   x_cursor += (3 * char_w);
   // Yıl (active_field == 2)
   snprintf(buf, sizeof(buf), "%02d", sDate->Year);
   BSP_LCD_SetTextColor((active_field == 2) ? LCD_COLOR_RED : LCD_COLOR_BLACK);
   BSP_LCD_DisplayStringAt(x_cursor, 80, (uint8_t*)buf, LEFT_MODE);
   /* --- SAAT SATIRI (Y = 120) --- */
   x_cursor = x_start;
   // "Saat : "
   BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
   BSP_LCD_DisplayStringAt(x_cursor, 120, (uint8_t*)"Saat : ", LEFT_MODE);
   x_cursor += (7 * char_w);
   // Saat (active_field == 3)
   snprintf(buf, sizeof(buf), "%02d", sTime->Hours);
   BSP_LCD_SetTextColor((active_field == 3) ? LCD_COLOR_RED : LCD_COLOR_BLACK);
   BSP_LCD_DisplayStringAt(x_cursor, 120, (uint8_t*)buf, LEFT_MODE);
   x_cursor += (2 * char_w);
   // Ayrac ":"
   BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
   BSP_LCD_DisplayStringAt(x_cursor, 120, (uint8_t*)":", LEFT_MODE);
   x_cursor += char_w;
   // Dakika (active_field == 4)
   snprintf(buf, sizeof(buf), "%02d", sTime->Minutes);
   BSP_LCD_SetTextColor((active_field == 4) ? LCD_COLOR_RED : LCD_COLOR_BLACK);
   BSP_LCD_DisplayStringAt(x_cursor, 120, (uint8_t*)buf, LEFT_MODE);
   /* --- ALT BİLGİ METİNLERİ --- */
   BSP_LCD_SetTextColor(LCD_COLOR_DARKGRAY);
   BSP_LCD_SetFont(&Font12);
   BSP_LCD_DisplayStringAt(0, 180, (uint8_t*)"[Up/Down] Degistir | [OK] Alan Sec", CENTER_MODE);
   BSP_LCD_DisplayStringAt(0, 200, (uint8_t*)"[Sol] Kaydet ve Cik", CENTER_MODE);
}
/* EŞİK DEĞER SET EKRANI */
void TFT_Manager_ShowSetThresholdsPage(float temp_max, float press_max, uint8_t active_field)
{
   char buf[30];
   BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
   BSP_LCD_SetFont(&Font16);
   snprintf(buf, sizeof(buf), "Max Sicaklik: %.1f C", temp_max);
   BSP_LCD_SetTextColor((active_field == 0) ? LCD_COLOR_RED : LCD_COLOR_BLACK);
   BSP_LCD_DisplayStringAt(0, 80, (uint8_t*)buf, CENTER_MODE);
   snprintf(buf, sizeof(buf), "Max Basinc  : %.0f hPa", press_max);
   BSP_LCD_SetTextColor((active_field == 1) ? LCD_COLOR_RED : LCD_COLOR_BLACK);
   BSP_LCD_DisplayStringAt(0, 120, (uint8_t*)buf, CENTER_MODE);
   BSP_LCD_SetTextColor(LCD_COLOR_DARKGRAY);
   BSP_LCD_SetFont(&Font12);
   BSP_LCD_DisplayStringAt(0, 180, (uint8_t*)"[Up/Down] Degistir | [OK] Alan Sec", CENTER_MODE);
   BSP_LCD_DisplayStringAt(0, 200, (uint8_t*)"[Sol] Kaydet ve Cik", CENTER_MODE);
}

#include "rtc_app.h"
extern RTC_HandleTypeDef hrtc;
#define RTC_BKP_MAGIC_NUMBER 0x32F2
/**
* @brief  Derleyicinin __DATE__ ve __TIME__ makrolarını güvenle ayrıştırır.
*/
static void RTC_ParseCompileDateTime(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate)
{
  char month_str[4];
  int day = 1, year = 2026, hour = 0, min = 0, sec = 0;
  /* __DATE__ Örnek: "Aug  5 2026" */
  sscanf(__DATE__, "%s %d %d", month_str, &day, &year);
  /* __TIME__ Örnek: "13:15:30" */
  sscanf(__TIME__, "%d:%d:%d", &hour, &min, &sec);
  sTime->Hours   = (uint8_t)hour;
  sTime->Minutes = (uint8_t)min;
  sTime->Seconds = (uint8_t)sec;
  sTime->TimeFormat = RTC_HOURFORMAT12_AM;
  sTime->DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime->StoreOperation = RTC_STOREOPERATION_RESET;
  sDate->Date = (uint8_t)day;
  sDate->Year = (uint8_t)(year % 100); // 2026 -> 26
  const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  sDate->Month = RTC_MONTH_JANUARY;
  for (uint8_t i = 0; i < 12; i++)
  {
      if (strncmp(month_str, months[i], 3) == 0)
      {
          sDate->Month = i + 1;
          break;
      }
  }
}
void RTC_App_Init(void)
{
  /* 1. Güç birimi saatini aktif et ve Backup Domain erişim kilidini aç */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();
  /* 2. Backup Register Kontrolü */
  if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != RTC_BKP_MAGIC_NUMBER)
  {
      /* --- İLK YÜKLEME VEYA GÜÇ KESİNTİSİ DURUMU --- */
      RTC_TimeTypeDef sTime = {0};
      RTC_DateTypeDef sDate = {0};
      /* Derleme anındaki bilgisayar saatini çöz */
      RTC_ParseCompileDateTime(&sTime, &sDate);
      /* RTC Donanımına Yaz (BIN formatında) */
      HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
      HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
      /* İşaretleyiciyi yaz (Reset atılınca anlaşılsın) */
      HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, RTC_BKP_MAGIC_NUMBER);
  }
  else
  {
      /* --- RESET BUTONUNA BASILDIĞI DURUM (WARM RESET) --- */
      /* RTC arka planda saymaya devam ediyor, sadece gölge register'ları eşitliyoruz */
      HAL_RTC_WaitForSynchro(&hrtc);
  }
}
void RTC_App_SetTimeAndDate(uint8_t hours, uint8_t minutes, uint8_t date, uint8_t month, uint8_t year)
{
   RTC_TimeTypeDef sTime = {0};
   RTC_DateTypeDef sDate = {0};
   sTime.Hours = hours;
   sTime.Minutes = minutes;
   sTime.Seconds = 0;
   HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
   sDate.Date = date;
   sDate.Month = month;
   sDate.Year = year;
   HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}
void RTC_App_GetTimeAndDate(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate)
{
  // STM32 RTC Okuma Kuralı: Önce Time sonra Date okunmalıdır
  HAL_RTC_GetTime(&hrtc, sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, sDate, RTC_FORMAT_BIN);
}
void RTC_App_GetFormattedStrings(char *buf_time_tft, char *buf_time_full, char *buf_date)
{
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  RTC_App_GetTimeAndDate(&sTime, &sDate);
  if (buf_time_tft != NULL)
  {
      sprintf(buf_time_tft, "%02d:%02d", sTime.Hours, sTime.Minutes);
  }
  if (buf_time_full != NULL)
  {
      sprintf(buf_time_full, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);
  }
  if (buf_date != NULL)
  {
      sprintf(buf_date, "%02d/%02d/20%02d", sDate.Date, sDate.Month, sDate.Year);
  }
}

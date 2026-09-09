#ifndef __RTC_APP_H
#define __RTC_APP_H
#include "stm32l0xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
void RTC_App_Init(void);
void RTC_App_SetTimeAndDate(uint8_t hours, uint8_t minutes, uint8_t date, uint8_t month, uint8_t year);
void RTC_App_GetTimeAndDate(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate);
void RTC_App_GetFormattedStrings(char *buf_time_tft, char *buf_time_full, char *buf_date);
#endif

#ifndef INC_LCD_MANAGER_H_
#define INC_LCD_MANAGER_H_
#include "stm32l073z_eval_glass_lcd.h"
#include <stdio.h>
#include <string.h>

HAL_StatusTypeDef LCD_Manager_Init(void);
void LCD_Manager_ShowTemperature(float temp);
void LCD_Manager_ShowPressure(float pressure);
void LCD_Manager_ShowText(const char *text);
#endif /* INC_LCD_MANAGER_H_ */

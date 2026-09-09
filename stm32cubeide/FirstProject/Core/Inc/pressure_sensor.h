#ifndef __PRESSURE_SENSOR_H
#define __PRESSURE_SENSOR_H
#include "stm32l0xx_hal.h"
#include <stdio.h>
typedef struct {
   ADC_HandleTypeDef *hadc;
   uint32_t adc_channel;
   float raw_mv;
   float pressure_hpa;
} PressureSensor_t;
HAL_StatusTypeDef PressureSensor_Init(PressureSensor_t *psens, ADC_HandleTypeDef *hadc);
float PressureSensor_Read(PressureSensor_t *psens);
void PressureSensor_SendViaUART_DMA(PressureSensor_t *psens, UART_HandleTypeDef *huart);
#endif

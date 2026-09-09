#include "pressure_sensor.h"
#include <string.h>
static uint8_t pressure_uart_tx_buf[64];
HAL_StatusTypeDef PressureSensor_Init(PressureSensor_t *psens, ADC_HandleTypeDef *hadc)
{
   if (psens == NULL || hadc == NULL) return HAL_ERROR;
   psens->hadc = hadc;
   psens->pressure_hpa = 0.0f;
   return HAL_OK;
}
float PressureSensor_Read(PressureSensor_t *psens)
{
   HAL_ADC_Start(psens->hadc);
   if (HAL_ADC_PollForConversion(psens->hadc, 10) == HAL_OK)
   {
       uint32_t raw_val = HAL_ADC_GetValue(psens->hadc);
       /* 1. 12-Bit ADC (0-4095) -> Voltaja (mV) Dönüşümü (VDD = 3.3V / 3300 mV) */
       float voltage_mv = ((float)raw_val / 4095.0f) * 3300.0f;
       /* 2. Donanımsal mV Kaçağının Temizlenmesi (Analog Ofset Kalibrasyonu)
        * Op-amp ve devre elemanlarının oluşturduğu sabit +178.4 mV kaçak voltaj elenir.
        */
       float calibrated_voltage_mv = voltage_mv - 178.4f;
       /*
        * --- DÖKÜMAN LINEER GRAFIK DENKLEMI HESABI (y = ax + b) ---
        * Referans Noktaları:
        * Nokta 1: 1000 hPa -> 1.65 V  = 1650 mV
        * Nokta 2: 1200 hPa -> 2.88 V  = 2880 mV
        *
        * 1. Eğim (a) Hesabı:
        * a = (P2 - P1) / (V2 - V1)
        * a = (1200 - 1000) / (2880 - 1650) = 200 / 1230 = 0.162601626 hPa/mV
        *
        * 2. Kayma/Offset (b) Hesabı:
        * b = P1 - (a * V1)
        * b = 1000 - (0.162601626 * 1650) = 1000 - 268.29268 = 731.707317 hPa
        *
        * Final Döküman Formülü: P = (V * 0.1626016f) + 731.7073f
        */
       psens->pressure_hpa = calibrated_voltage_mv * 0.1626016f + 731.7073f ;
   }
   HAL_ADC_Stop(psens->hadc);
   return psens->pressure_hpa;
}
void PressureSensor_SendViaUART_DMA(PressureSensor_t *psens, UART_HandleTypeDef *huart)
{
   snprintf((char*)pressure_uart_tx_buf, sizeof(pressure_uart_tx_buf),
            "[BASINC]: %.2f hPa\r\n", psens->pressure_hpa);
   HAL_UART_Transmit_DMA(huart, pressure_uart_tx_buf, strlen((char*)pressure_uart_tx_buf));
}

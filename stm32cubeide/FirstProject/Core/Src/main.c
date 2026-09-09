/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stlm75.h>
#include "dma_handler.h"
#include "tft_manager.h"
#include "pressure_sensor.h"
#include "stm32l073z_eval.h"
#include "logger_eeprom.h"
#include "rtc_app.h"
#include "ui_manager.h"
/* USER CODE END Includes */
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */
/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */
/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */
/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;
COMP_HandleTypeDef hcomp2;
CRC_HandleTypeDef hcrc;
DAC_HandleTypeDef hdac;
I2C_HandleTypeDef hi2c1;
LCD_HandleTypeDef hlcd;
LPTIM_HandleTypeDef hlptim1;
RTC_HandleTypeDef hrtc;
SPI_HandleTypeDef hspi1;
TIM_HandleTypeDef htim2;
TSC_HandleTypeDef htsc;
UART_HandleTypeDef huart4;
DMA_HandleTypeDef hdma_usart4_rx;
DMA_HandleTypeDef hdma_usart4_tx;
PCD_HandleTypeDef hpcd_USB_FS;
/* USER CODE BEGIN PV */
extern CRC_HandleTypeDef hcrc; // CubeMX'in oluşturduğu CRC yapısı
#define RX_BUFFER_SIZE 64
uint8_t rx_dma_buffer[RX_BUFFER_SIZE]; // DMA'nın veriyi doğrudan dolduracağı dizi
char command_buffer[RX_BUFFER_SIZE];
uint8_t rx_data;
STLM75_HandleTypeDef hstlm75;
PressureSensor_t g_press_sensor;
// Acil Durum (Tamper) Bayrağı
volatile uint8_t g_emergency_active = 0;
// Zamanlayıcı Ticker Değişkenleri
uint32_t last_ui_tick     = 0;
uint32_t last_sensor_tick = 0;
uint32_t last_eeprom_tick = 0;
uint8_t last_drawn_minute = 255;
PageState_t last_drawn_page = (PageState_t)255;
// Donanım LED Pin Tanımları (STM32L073Z Discovery)
#define LED1_CONN_PIN   GPIO_PIN_4  // Yeşil (LD1)
#define LED2_TEMP_PIN   GPIO_PIN_5  // Turuncu (LD2)
#define LED3_EMERG_PIN   GPIO_PIN_1  // Kırmızı (LD3)
#define LED4_PRESS_PIN  GPIO_PIN_7  // Mavi (LD4)
#define LED_PORTE       GPIOE
#define LED_PORTD       GPIOD
/* USER CODE END PV */
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC_Init(void);
static void MX_DAC_Init(void);
static void MX_USART4_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_USB_PCD_Init(void);
static void MX_TIM2_Init(void);
static void MX_CRC_Init(void);
static void MX_I2C1_Init(void);
static void MX_LCD_Init(void);
static void MX_COMP2_Init(void);
static void MX_LPTIM1_Init(void);
static void MX_RTC_Init(void);
static void MX_TSC_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void Update_Board_LEDs(float current_temp, float current_press, uint8_t is_emergency, uint8_t is_connected);
static JOYState_TypeDef Get_Debounced_Joystick(void);
/* USER CODE END 0 */
/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
 /* USER CODE BEGIN 1 */
 /* USER CODE END 1 */
 /* MCU Configuration--------------------------------------------------------*/
 /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
 HAL_Init();
 /* USER CODE BEGIN Init */
 /* USER CODE END Init */
 /* Configure the system clock */
 SystemClock_Config();
 /* USER CODE BEGIN SysInit */
 /* USER CODE END SysInit */
 /* Initialize all configured peripherals */
 MX_GPIO_Init();
 MX_DMA_Init();
 MX_ADC_Init();
 MX_DAC_Init();
 MX_USART4_UART_Init();
 MX_SPI1_Init();
 MX_USB_PCD_Init();
 MX_TIM2_Init();
 MX_CRC_Init();
 MX_I2C1_Init();
 MX_LCD_Init();
 MX_COMP2_Init();
 MX_LPTIM1_Init();
 MX_RTC_Init();
 MX_TSC_Init();
 /* USER CODE BEGIN 2 */
 DMA_Handler_Init(&huart4);
 // Sensör başlatma (CubeMX'in oluşturduğu &hi2c1 ve &huart4 kullanılır)
 STLM75_Init(&hstlm75, &hi2c1, &huart4);
 PressureSensor_Init(&g_press_sensor, &hadc);
 BSP_JOY_Init(JOY_MODE_GPIO);
 /* Derleme anındaki saati yükler (sadece ilk açılışta),
		 Reset atıldığında mevcut saati korur */
 RTC_App_Init();
 Logger_Init(&hi2c1, &hrtc);
 // TFT ve UI Yöneticisini en son başlatın!
   TFT_Manager_Init();
   UI_Manager_Init(); // Menü en son çizilmeli
 //UART Interrupt Dinlemesini Başlat (Hercules Dışa Aktarımı İçin)
 HAL_UART_Receive_IT(&huart4, &rx_data, 1);
 /* USER CODE END 2 */
 /* Infinite loop */
 /* USER CODE BEGIN WHILE */
 while (1)
     {
         uint32_t current_tick = HAL_GetTick();
         /* Task 1: UART DMA İşleme (Her zaman aktif) */
         DMA_Process_In_Main(&huart4);
         /* Task 2: Yüksek Hızlı Input/UI İşleme (Her 20 ms'de bir - 50 FPS) */
         if (current_tick - last_ui_tick >= 20)
         {
             last_ui_tick = current_tick;
             JOYState_TypeDef joy_event = Get_Debounced_Joystick();
             RTC_TimeTypeDef sTime_Current;
             RTC_DateTypeDef sDate_Current;
             RTC_App_GetTimeAndDate(&sTime_Current, &sDate_Current);
             UI_Manager_UpdateHeaderIfNeeded(&sTime_Current, &sDate_Current);
             UI_Manager_ProcessInput(joy_event, &sTime_Current, &sDate_Current);
         }
         /* Task 3: Sensör Okuma, Telemetri ve Sayfa Güncelleme (Her 1000 ms'de bir) */
         if (current_tick - last_sensor_tick >= 1000)
         {
             last_sensor_tick = current_tick;
             STLM75_ReadTemperature(&hstlm75);
             float current_press = PressureSensor_Read(&g_press_sensor);
             // LED'leri güncelle (Acil durum bayrağı dahil)
				Update_Board_LEDs(hstlm75.temperature, current_press, g_emergency_active, 1);
             // Telemetri Paketi (13 Bayt)
             uint8_t telemetry_pkt[13];
             telemetry_pkt[0] = 0xAA;
             telemetry_pkt[1] = 0x55;
             telemetry_pkt[2] = 0x01;
             telemetry_pkt[3] = (uint8_t)hstlm75.temperature;
             telemetry_pkt[4] = (uint8_t)((hstlm75.temperature - (int)hstlm75.temperature) * 10.0f);
             uint32_t press_scaled = (uint32_t)(current_press * 100.0f);
             telemetry_pkt[5] = (press_scaled >> 24) & 0xFF;
             telemetry_pkt[6] = (press_scaled >> 16) & 0xFF;
             telemetry_pkt[7] = (press_scaled >> 8)  & 0xFF;
             telemetry_pkt[8] = press_scaled & 0xFF;
             telemetry_pkt[9]  = (uint8_t)(g_ui_config.max_temp_threshold * 2.0f);
             telemetry_pkt[10] = (uint8_t)(g_ui_config.max_press_threshold - 900.0f);
             // Bayt 11: Acil Durum Durumu (0x00: Normal, 0x01: Acil Durum)
			  telemetry_pkt[11] = g_emergency_active ? 0x01 : 0x00;
			  telemetry_pkt[12]  = 0x0D; // Footer (Sonlandırma)
             HAL_UART_Transmit(&huart4, telemetry_pkt, sizeof(telemetry_pkt), 50);
             UI_Manager_RenderPeriodicPages(hstlm75.temperature, g_press_sensor.pressure_hpa);
         }
         /* Task 4: EEPROM Log Kaydı (Her 5000 ms'de bir) */
         if (current_tick - last_eeprom_tick >= 5000)
         {
             last_eeprom_tick = current_tick;
             Logger_SaveData(hstlm75.temperature, (uint16_t)g_press_sensor.pressure_hpa);
         }
     }
 /* USER CODE END 3 */
}
/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
 RCC_OscInitTypeDef RCC_OscInitStruct = {0};
 RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
 RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
 /** Configure the main internal regulator output voltage
 */
 __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
 /** Configure LSE Drive Capability
 */
 HAL_PWR_EnableBkUpAccess();
 __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
 /** Initializes the RCC Oscillators according to the specified parameters
 * in the RCC_OscInitTypeDef structure.
 */
 RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
 RCC_OscInitStruct.HSEState = RCC_HSE_ON;
 RCC_OscInitStruct.LSEState = RCC_LSE_ON;
 RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
 RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
 RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_12;
 RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_3;
 if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
 {
   Error_Handler();
 }
 /** Initializes the CPU, AHB and APB buses clocks
 */
 RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                             |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
 RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
 RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
 RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
 RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
 if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
 {
   Error_Handler();
 }
 PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_RTC
                             |RCC_PERIPHCLK_USB|RCC_PERIPHCLK_LPTIM1;
 PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
 PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
 PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
 PeriphClkInit.LptimClockSelection = RCC_LPTIM1CLKSOURCE_PCLK;
 if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
 {
   Error_Handler();
 }
}
/**
 * @brief ADC Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC_Init(void)
{
 /* USER CODE BEGIN ADC_Init 0 */
 /* USER CODE END ADC_Init 0 */
 ADC_ChannelConfTypeDef sConfig = {0};
 /* USER CODE BEGIN ADC_Init 1 */
 /* USER CODE END ADC_Init 1 */
 /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
 */
 hadc.Instance = ADC1;
 hadc.Init.OversamplingMode = DISABLE;
 hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
 hadc.Init.Resolution = ADC_RESOLUTION_12B;
 hadc.Init.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
 hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
 hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
 hadc.Init.ContinuousConvMode = DISABLE;
 hadc.Init.DiscontinuousConvMode = DISABLE;
 hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
 hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
 hadc.Init.DMAContinuousRequests = DISABLE;
 hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
 hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
 hadc.Init.LowPowerAutoWait = DISABLE;
 hadc.Init.LowPowerFrequencyMode = DISABLE;
 hadc.Init.LowPowerAutoPowerOff = DISABLE;
 if (HAL_ADC_Init(&hadc) != HAL_OK)
 {
   Error_Handler();
 }
 /** Configure for the selected ADC regular channel to be converted.
 */
 sConfig.Channel = ADC_CHANNEL_0;
 sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
 if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
 {
   Error_Handler();
 }
 /** Configure for the selected ADC regular channel to be converted.
 */
 sConfig.Channel = ADC_CHANNEL_5;
 if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN ADC_Init 2 */
 /* USER CODE END ADC_Init 2 */
}
/**
 * @brief COMP2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_COMP2_Init(void)
{
 /* USER CODE BEGIN COMP2_Init 0 */
 /* USER CODE END COMP2_Init 0 */
 /* USER CODE BEGIN COMP2_Init 1 */
 /* USER CODE END COMP2_Init 1 */
 hcomp2.Instance = COMP2;
 hcomp2.Init.InvertingInput = COMP_INPUT_MINUS_DAC1_CH1;
 hcomp2.Init.NonInvertingInput = COMP_INPUT_PLUS_IO2;
 hcomp2.Init.LPTIMConnection = COMP_LPTIMCONNECTION_IN1_ENABLED;
 hcomp2.Init.OutputPol = COMP_OUTPUTPOL_NONINVERTED;
 hcomp2.Init.Mode = COMP_POWERMODE_MEDIUMSPEED;
 hcomp2.Init.WindowMode = COMP_WINDOWMODE_DISABLE;
 hcomp2.Init.TriggerMode = COMP_TRIGGERMODE_NONE;
 if (HAL_COMP_Init(&hcomp2) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN COMP2_Init 2 */
 /* USER CODE END COMP2_Init 2 */
}
/**
 * @brief CRC Initialization Function
 * @param None
 * @retval None
 */
static void MX_CRC_Init(void)
{
 /* USER CODE BEGIN CRC_Init 0 */
 /* USER CODE END CRC_Init 0 */
 /* USER CODE BEGIN CRC_Init 1 */
 /* USER CODE END CRC_Init 1 */
 hcrc.Instance = CRC;
 hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
 hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
 hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
 hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
 hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
 if (HAL_CRC_Init(&hcrc) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN CRC_Init 2 */
 /* USER CODE END CRC_Init 2 */
}
/**
 * @brief DAC Initialization Function
 * @param None
 * @retval None
 */
static void MX_DAC_Init(void)
{
 /* USER CODE BEGIN DAC_Init 0 */
 /* USER CODE END DAC_Init 0 */
 DAC_ChannelConfTypeDef sConfig = {0};
 /* USER CODE BEGIN DAC_Init 1 */
 /* USER CODE END DAC_Init 1 */
 /** DAC Initialization
 */
 hdac.Instance = DAC;
 if (HAL_DAC_Init(&hdac) != HAL_OK)
 {
   Error_Handler();
 }
 /** DAC channel OUT1 config
 */
 sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
 sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
 if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN DAC_Init 2 */
 /* USER CODE END DAC_Init 2 */
}
/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void)
{
 /* USER CODE BEGIN I2C1_Init 0 */
 /* USER CODE END I2C1_Init 0 */
 /* USER CODE BEGIN I2C1_Init 1 */
 /* USER CODE END I2C1_Init 1 */
 hi2c1.Instance = I2C1;
 hi2c1.Init.Timing = 0x00B07CB4;
 hi2c1.Init.OwnAddress1 = 0;
 hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
 hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
 hi2c1.Init.OwnAddress2 = 0;
 hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
 hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
 hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
 if (HAL_I2C_Init(&hi2c1) != HAL_OK)
 {
   Error_Handler();
 }
 /** Configure Analogue filter
 */
 if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
 {
   Error_Handler();
 }
 /** Configure Digital filter
 */
 if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN I2C1_Init 2 */
 /* USER CODE END I2C1_Init 2 */
}
/**
 * @brief LCD Initialization Function
 * @param None
 * @retval None
 */
static void MX_LCD_Init(void)
{
 /* USER CODE BEGIN LCD_Init 0 */
//
 /* USER CODE END LCD_Init 0 */
 /* USER CODE BEGIN LCD_Init 1 */
//
 /* USER CODE END LCD_Init 1 */
 hlcd.Instance = LCD;
 hlcd.Init.Prescaler = LCD_PRESCALER_1;
 hlcd.Init.Divider = LCD_DIVIDER_16;
 hlcd.Init.Duty = LCD_DUTY_1_4;
 hlcd.Init.Bias = LCD_BIAS_1_3;
 hlcd.Init.VoltageSource = LCD_VOLTAGESOURCE_INTERNAL;
 hlcd.Init.Contrast = LCD_CONTRASTLEVEL_6;
 hlcd.Init.DeadTime = LCD_DEADTIME_0;
 hlcd.Init.PulseOnDuration = LCD_PULSEONDURATION_0;
 hlcd.Init.HighDrive = LCD_HIGHDRIVE_0;
 hlcd.Init.BlinkMode = LCD_BLINKMODE_OFF;
 hlcd.Init.BlinkFrequency = LCD_BLINKFREQUENCY_DIV8;
 hlcd.Init.MuxSegment = LCD_MUXSEGMENT_DISABLE;
 if (HAL_LCD_Init(&hlcd) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN LCD_Init 2 */
//
 /* USER CODE END LCD_Init 2 */
}
/**
 * @brief LPTIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_LPTIM1_Init(void)
{
 /* USER CODE BEGIN LPTIM1_Init 0 */
 /* USER CODE END LPTIM1_Init 0 */
 /* USER CODE BEGIN LPTIM1_Init 1 */
 /* USER CODE END LPTIM1_Init 1 */
 hlptim1.Instance = LPTIM1;
 hlptim1.Init.Clock.Source = LPTIM_CLOCKSOURCE_ULPTIM;
 hlptim1.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV1;
 hlptim1.Init.UltraLowPowerClock.Polarity = LPTIM_CLOCKPOLARITY_RISING;
 hlptim1.Init.UltraLowPowerClock.SampleTime = LPTIM_CLOCKSAMPLETIME_DIRECTTRANSITION;
 hlptim1.Init.Trigger.Source = LPTIM_TRIGSOURCE_7;
 hlptim1.Init.Trigger.ActiveEdge = LPTIM_ACTIVEEDGE_RISING;
 hlptim1.Init.Trigger.SampleTime = LPTIM_TRIGSAMPLETIME_DIRECTTRANSITION;
 hlptim1.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_HIGH;
 hlptim1.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
 hlptim1.Init.CounterSource = LPTIM_COUNTERSOURCE_EXTERNAL;
 if (HAL_LPTIM_Init(&hlptim1) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN LPTIM1_Init 2 */
 /* USER CODE END LPTIM1_Init 2 */
}
/**
 * @brief RTC Initialization Function
 * @param None
 * @retval None
 */
static void MX_RTC_Init(void)
{
 /* USER CODE BEGIN RTC_Init 0 */
 /* USER CODE END RTC_Init 0 */
 RTC_TimeTypeDef sTime = {0};
 RTC_DateTypeDef sDate = {0};
 /* USER CODE BEGIN RTC_Init 1 */
 /* USER CODE END RTC_Init 1 */
 /** Initialize RTC Only
 */
 hrtc.Instance = RTC;
 hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
 hrtc.Init.AsynchPrediv = 127;
 hrtc.Init.SynchPrediv = 255;
 hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
 hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
 hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
 hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
 if (HAL_RTC_Init(&hrtc) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN Check_RTC_BKUP */
 /* USER CODE END Check_RTC_BKUP */
 /** Initialize RTC and set the Time and Date
 */
 sTime.Hours = 0x0;
 sTime.Minutes = 0x0;
 sTime.Seconds = 0x0;
 sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
 sTime.StoreOperation = RTC_STOREOPERATION_RESET;
 if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
 {
   Error_Handler();
 }
 sDate.WeekDay = RTC_WEEKDAY_SATURDAY;
 sDate.Month = RTC_MONTH_APRIL;
 sDate.Date = 0x1;
 sDate.Year = 0x26;
 if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN RTC_Init 2 */
 /* USER CODE END RTC_Init 2 */
}
/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void)
{
 /* USER CODE BEGIN SPI1_Init 0 */
 /* USER CODE END SPI1_Init 0 */
 /* USER CODE BEGIN SPI1_Init 1 */
 /* USER CODE END SPI1_Init 1 */
 /* SPI1 parameter configuration*/
 hspi1.Instance = SPI1;
 hspi1.Init.Mode = SPI_MODE_MASTER;
 hspi1.Init.Direction = SPI_DIRECTION_2LINES;
 hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
 hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
 hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
 hspi1.Init.NSS = SPI_NSS_SOFT;
 hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
 hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
 hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
 hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
 hspi1.Init.CRCPolynomial = 7;
 if (HAL_SPI_Init(&hspi1) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN SPI1_Init 2 */
 /* USER CODE END SPI1_Init 2 */
}
/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void)
{
 /* USER CODE BEGIN TIM2_Init 0 */
 /* USER CODE END TIM2_Init 0 */
 TIM_ClockConfigTypeDef sClockSourceConfig = {0};
 TIM_MasterConfigTypeDef sMasterConfig = {0};
 /* USER CODE BEGIN TIM2_Init 1 */
 /* USER CODE END TIM2_Init 1 */
 htim2.Instance = TIM2;
 htim2.Init.Prescaler = 31999;
 htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
 htim2.Init.Period = 999;
 htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
 htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
 if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
 {
   Error_Handler();
 }
 sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
 if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
 {
   Error_Handler();
 }
 sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
 sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
 if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN TIM2_Init 2 */
 /* USER CODE END TIM2_Init 2 */
}
/**
 * @brief TSC Initialization Function
 * @param None
 * @retval None
 */
static void MX_TSC_Init(void)
{
 /* USER CODE BEGIN TSC_Init 0 */
 /* USER CODE END TSC_Init 0 */
 /* USER CODE BEGIN TSC_Init 1 */
 /* USER CODE END TSC_Init 1 */
 /** Configure the TSC peripheral
 */
 htsc.Instance = TSC;
 htsc.Init.CTPulseHighLength = TSC_CTPH_2CYCLES;
 htsc.Init.CTPulseLowLength = TSC_CTPL_2CYCLES;
 htsc.Init.SpreadSpectrum = DISABLE;
 htsc.Init.SpreadSpectrumDeviation = 1;
 htsc.Init.SpreadSpectrumPrescaler = TSC_SS_PRESC_DIV1;
 htsc.Init.PulseGeneratorPrescaler = TSC_PG_PRESC_DIV64;
 htsc.Init.MaxCountValue = TSC_MCV_16383;
 htsc.Init.IODefaultMode = TSC_IODEF_OUT_PP_LOW;
 htsc.Init.SynchroPinPolarity = TSC_SYNC_POLARITY_FALLING;
 htsc.Init.AcquisitionMode = TSC_ACQ_MODE_NORMAL;
 htsc.Init.MaxCountInterrupt = DISABLE;
 htsc.Init.ChannelIOs = TSC_GROUP1_IO2|TSC_GROUP3_IO2|TSC_GROUP6_IO2;
 htsc.Init.ShieldIOs = 0;
 htsc.Init.SamplingIOs = TSC_GROUP1_IO3|TSC_GROUP3_IO3|TSC_GROUP6_IO3;
 if (HAL_TSC_Init(&htsc) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN TSC_Init 2 */
 /* USER CODE END TSC_Init 2 */
}
/**
 * @brief USART4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART4_UART_Init(void)
{
 /* USER CODE BEGIN USART4_Init 0 */
 /* USER CODE END USART4_Init 0 */
 /* USER CODE BEGIN USART4_Init 1 */
 /* USER CODE END USART4_Init 1 */
 huart4.Instance = USART4;
 huart4.Init.BaudRate = 115200;
 huart4.Init.WordLength = UART_WORDLENGTH_8B;
 huart4.Init.StopBits = UART_STOPBITS_1;
 huart4.Init.Parity = UART_PARITY_NONE;
 huart4.Init.Mode = UART_MODE_TX_RX;
 huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
 huart4.Init.OverSampling = UART_OVERSAMPLING_16;
 huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
 huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
 if (HAL_UART_Init(&huart4) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN USART4_Init 2 */
 /* USER CODE END USART4_Init 2 */
}
/**
 * @brief USB Initialization Function
 * @param None
 * @retval None
 */
static void MX_USB_PCD_Init(void)
{
 /* USER CODE BEGIN USB_Init 0 */
 /* USER CODE END USB_Init 0 */
 /* USER CODE BEGIN USB_Init 1 */
 /* USER CODE END USB_Init 1 */
 hpcd_USB_FS.Instance = USB;
 hpcd_USB_FS.Init.dev_endpoints = 8;
 hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
 hpcd_USB_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
 hpcd_USB_FS.Init.low_power_enable = DISABLE;
 hpcd_USB_FS.Init.lpm_enable = DISABLE;
 hpcd_USB_FS.Init.battery_charging_enable = DISABLE;
 if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN USB_Init 2 */
 /* USER CODE END USB_Init 2 */
}
/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void)
{
 /* DMA controller clock enable */
 __HAL_RCC_DMA1_CLK_ENABLE();
 /* DMA interrupt init */
 /* DMA1_Channel2_3_IRQn interrupt configuration */
 HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
 HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
}
/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};
 /* USER CODE BEGIN MX_GPIO_Init_1 */
 /* USER CODE END MX_GPIO_Init_1 */
 /* GPIO Ports Clock Enable */
 __HAL_RCC_GPIOE_CLK_ENABLE();
 __HAL_RCC_GPIOC_CLK_ENABLE();
 __HAL_RCC_GPIOH_CLK_ENABLE();
 __HAL_RCC_GPIOA_CLK_ENABLE();
 __HAL_RCC_GPIOB_CLK_ENABLE();
 __HAL_RCC_GPIOD_CLK_ENABLE();
 /*Configure GPIO pin Output Level */
 HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7, GPIO_PIN_RESET);
 /*Configure GPIO pin Output Level */
 HAL_GPIO_WritePin(GPIOH, GPIO_PIN_9|MFX_WKUP_Pin, GPIO_PIN_RESET);
 /*Configure GPIO pin Output Level */
 HAL_GPIO_WritePin(GPIOD, MicroSD_CS_Pin|GPIO_PIN_1|GPIO_PIN_7, GPIO_PIN_RESET);
 /*Configure GPIO pins : PE4 PE5 PE7 */
 GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
 /*Configure GPIO pin : MFX_IRQOUT_Pin */
 GPIO_InitStruct.Pin = MFX_IRQOUT_Pin;
 GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 HAL_GPIO_Init(MFX_IRQOUT_GPIO_Port, &GPIO_InitStruct);
 /*Configure GPIO pin : PC13 */
 GPIO_InitStruct.Pin = GPIO_PIN_13;
 GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
 GPIO_InitStruct.Pull = GPIO_PULLDOWN;
 HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
 /*Configure GPIO pins : PH9 MFX_WKUP_Pin */
 GPIO_InitStruct.Pin = GPIO_PIN_9|MFX_WKUP_Pin;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
 /*Configure GPIO pins : MicroSD_CS_Pin PD1 PD7 */
 GPIO_InitStruct.Pin = MicroSD_CS_Pin|GPIO_PIN_1|GPIO_PIN_7;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
 /* EXTI interrupt init*/
 HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
 HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
 /* USER CODE BEGIN MX_GPIO_Init_2 */
 /* USER CODE END MX_GPIO_Init_2 */
}
/* USER CODE BEGIN 4 */
/*
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_13)
	    {
	        static uint32_t last_interrupt_time = 0;
	        uint32_t current_time = HAL_GetTick(); // Sistem süresini ms olarak alırsın
	        // Eğer son kesmeden bu yana 75 ms'den fazla zaman geçtiyse bu GERÇEK bir basıştır!
	        // (75 ms içinde gelen diğer kesmeler mekanik sıçramadır, yoksayılır)
	        //NOT: Mekanik sıçramalar yaklaşık 5-20 ms arası sürer
	        if (current_time - last_interrupt_time > 100)
	        {
	        	g_emergency_active = !g_emergency_active;
	        }
	        last_interrupt_time = current_time; // Son basılma zamanını güncelle
	    }
}
/**
 * @brief Joystick girdilerini kaçırmayan ve basılı tutmayı algılayan filtreleyici
 */
static JOYState_TypeDef Get_Debounced_Joystick(void)
{
   static JOYState_TypeDef prev_state = JOY_NONE;
   static uint32_t press_duration_ticks = 0;
   JOYState_TypeDef raw_state = BSP_JOY_GetState();
   if (raw_state != JOY_NONE)
   {
       if (raw_state != prev_state)
       {
           // Yeni bir yön tuşuna tıklandı
           prev_state = raw_state;
           press_duration_ticks = 0;
           return raw_state;
       }
       else
       {
           // Aynı tuşa basılmaya devam ediliyor (Uzun basma takibi)
           press_duration_ticks++;
           // 20ms * 15 = 300 ms basılı tutulduysa otomatik tekrarlat (Auto-repeat)
           if (press_duration_ticks >= 15)
           {
               press_duration_ticks = 10; // Hızlı kaydırma için eşik
               return raw_state;
           }
       }
   }
   else
   {
       prev_state = JOY_NONE;
       press_duration_ticks = 0;
   }
   return JOY_NONE;
}
static void Update_Board_LEDs(float current_temp, float current_press, uint8_t is_emergency, uint8_t is_connected)
{
   // LD1 (Yeşil - PE4): Sistem Açık / COM Bağlantısı Aktif (Sürekli Sabit Yanar)
   HAL_GPIO_WritePin(LED_PORTE, LED1_CONN_PIN, is_connected ? GPIO_PIN_RESET : GPIO_PIN_SET);
   // LD2 (Turuncu - PE5): Sıcaklık Eşik Üstünde Olduğu Sürece Sabit Yanar
   if (current_temp >= g_ui_config.max_temp_threshold) {
       HAL_GPIO_WritePin(LED_PORTE, LED2_TEMP_PIN, GPIO_PIN_RESET); // Aktif LOW (Yanar)
   } else {
       HAL_GPIO_WritePin(LED_PORTE, LED2_TEMP_PIN, GPIO_PIN_SET);   // Söner
   }
   // LD3 (Kırmızı - PD1): Tamper Acil Durum Butonuna Basıldığında Sabit Yanar
   if (is_emergency) {
       HAL_GPIO_WritePin(LED_PORTD, LED3_EMERG_PIN, GPIO_PIN_RESET); // Aktif LOW (Yanar)
   } else {
       HAL_GPIO_WritePin(LED_PORTD, LED3_EMERG_PIN, GPIO_PIN_SET);   // Söner
   }
   // LD4 (Mavi - PD7): Basınç Eşik Üstünde Olduğu Sürece Sabit Yanar
   if (current_press >= g_ui_config.max_press_threshold) {
       HAL_GPIO_WritePin(LED_PORTD, LED4_PRESS_PIN, GPIO_PIN_RESET); // Aktif LOW (Yanar)
   } else {
       HAL_GPIO_WritePin(LED_PORTD, LED4_PRESS_PIN, GPIO_PIN_SET);   // Söner
   }
}
/*
*/
/*
*/
int __io_putchar(int ch)
{
	uint8_t che = (uint8_t)ch;
	 HAL_UART_Transmit(&huart4, &che, 1,1000);
	 return ch;
}
int __io_getchar(void)
{
	char c = 0;
	HAL_UART_Receive(&huart4, (uint8_t*)&c, 1, HAL_MAX_DELAY);
	return c;
}
/* USER CODE END 4 */
/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
 /* USER CODE BEGIN Error_Handler_Debug */
 /* User can add his own implementation to report the HAL error return state */
 __disable_irq();
 while (1)
 {
 }
 /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
 /* USER CODE BEGIN 6 */
 /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
 /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

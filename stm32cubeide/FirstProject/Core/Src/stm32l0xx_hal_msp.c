/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file         stm32l0xx_hal_msp.c
 * @brief        This file provides code for the MSP Initialization
 *               and de-Initialization codes.
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
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */
extern DMA_HandleTypeDef hdma_usart4_rx;
extern DMA_HandleTypeDef hdma_usart4_tx;
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
/* USER CODE END TD */
/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */
/* USER CODE END Define */
/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */
/* USER CODE END Macro */
/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* USER CODE END PV */
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */
/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */
/* USER CODE END ExternalFunctions */
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */
/**
 * Initializes the Global MSP.
 */
void HAL_MspInit(void)
{
 /* USER CODE BEGIN MspInit 0 */
 /* USER CODE END MspInit 0 */
 __HAL_RCC_SYSCFG_CLK_ENABLE();
 __HAL_RCC_PWR_CLK_ENABLE();
 /* System interrupt init*/
 /* USER CODE BEGIN MspInit 1 */
 /* USER CODE END MspInit 1 */
}
/**
 * @brief ADC MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hadc: ADC handle pointer
 * @retval None
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};
 if(hadc->Instance==ADC1)
 {
   /* USER CODE BEGIN ADC1_MspInit 0 */
   /* USER CODE END ADC1_MspInit 0 */
   /* Peripheral clock enable */
   __HAL_RCC_ADC1_CLK_ENABLE();
   __HAL_RCC_GPIOA_CLK_ENABLE();
   /**ADC GPIO Configuration
   PA0     ------> ADC_IN0
   PA5     ------> ADC_IN5
   */
   GPIO_InitStruct.Pin = GPIO_PIN_0|ADC_IN5_Pin;
   GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
   /* USER CODE BEGIN ADC1_MspInit 1 */
   /* USER CODE END ADC1_MspInit 1 */
 }
}
/**
 * @brief ADC MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hadc: ADC handle pointer
 * @retval None
 */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
 if(hadc->Instance==ADC1)
 {
   /* USER CODE BEGIN ADC1_MspDeInit 0 */
   /* USER CODE END ADC1_MspDeInit 0 */
   /* Peripheral clock disable */
   __HAL_RCC_ADC1_CLK_DISABLE();
   /**ADC GPIO Configuration
   PA0     ------> ADC_IN0
   PA5     ------> ADC_IN5
   */
   HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|ADC_IN5_Pin);
   /* USER CODE BEGIN ADC1_MspDeInit 1 */
   /* USER CODE END ADC1_MspDeInit 1 */
 }
}
/**
 * @brief COMP MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hcomp: COMP handle pointer
 * @retval None
 */
void HAL_COMP_MspInit(COMP_HandleTypeDef* hcomp)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};
 if(hcomp->Instance==COMP2)
 {
   /* USER CODE BEGIN COMP2_MspInit 0 */
   /* USER CODE END COMP2_MspInit 0 */
   __HAL_RCC_GPIOA_CLK_ENABLE();
   __HAL_RCC_GPIOB_CLK_ENABLE();
   /**COMP2 GPIO Configuration
   PA7     ------> COMP2_OUT
   PB4     ------> COMP2_INP
   */
   GPIO_InitStruct.Pin = GPIO_PIN_7;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Alternate = GPIO_AF7_COMP2;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
   GPIO_InitStruct.Pin = GPIO_PIN_4;
   GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
   /* USER CODE BEGIN COMP2_MspInit 1 */
   /* USER CODE END COMP2_MspInit 1 */
 }
}
/**
 * @brief COMP MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hcomp: COMP handle pointer
 * @retval None
 */
void HAL_COMP_MspDeInit(COMP_HandleTypeDef* hcomp)
{
 if(hcomp->Instance==COMP2)
 {
   /* USER CODE BEGIN COMP2_MspDeInit 0 */
   /* USER CODE END COMP2_MspDeInit 0 */
   /**COMP2 GPIO Configuration
   PA7     ------> COMP2_OUT
   PB4     ------> COMP2_INP
   */
   HAL_GPIO_DeInit(GPIOA, GPIO_PIN_7);
   HAL_GPIO_DeInit(GPIOB, GPIO_PIN_4);
   /* USER CODE BEGIN COMP2_MspDeInit 1 */
   /* USER CODE END COMP2_MspDeInit 1 */
 }
}
/**
 * @brief CRC MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hcrc: CRC handle pointer
 * @retval None
 */
void HAL_CRC_MspInit(CRC_HandleTypeDef* hcrc)
{
 if(hcrc->Instance==CRC)
 {
   /* USER CODE BEGIN CRC_MspInit 0 */
   /* USER CODE END CRC_MspInit 0 */
   /* Peripheral clock enable */
   __HAL_RCC_CRC_CLK_ENABLE();
   /* USER CODE BEGIN CRC_MspInit 1 */
   /* USER CODE END CRC_MspInit 1 */
 }
}
/**
 * @brief CRC MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hcrc: CRC handle pointer
 * @retval None
 */
void HAL_CRC_MspDeInit(CRC_HandleTypeDef* hcrc)
{
 if(hcrc->Instance==CRC)
 {
   /* USER CODE BEGIN CRC_MspDeInit 0 */
   /* USER CODE END CRC_MspDeInit 0 */
   /* Peripheral clock disable */
   __HAL_RCC_CRC_CLK_DISABLE();
   /* USER CODE BEGIN CRC_MspDeInit 1 */
   /* USER CODE END CRC_MspDeInit 1 */
 }
}
/**
 * @brief DAC MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hdac: DAC handle pointer
 * @retval None
 */
void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};
 if(hdac->Instance==DAC)
 {
   /* USER CODE BEGIN DAC_MspInit 0 */
   /* USER CODE END DAC_MspInit 0 */
   /* Peripheral clock enable */
   __HAL_RCC_DAC_CLK_ENABLE();
   __HAL_RCC_GPIOA_CLK_ENABLE();
   /**DAC GPIO Configuration
   PA4     ------> DAC_OUT1
   */
   GPIO_InitStruct.Pin = GPIO_PIN_4;
   GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
   /* USER CODE BEGIN DAC_MspInit 1 */
   /* USER CODE END DAC_MspInit 1 */
 }
}
/**
 * @brief DAC MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hdac: DAC handle pointer
 * @retval None
 */
void HAL_DAC_MspDeInit(DAC_HandleTypeDef* hdac)
{
 if(hdac->Instance==DAC)
 {
   /* USER CODE BEGIN DAC_MspDeInit 0 */
   /* USER CODE END DAC_MspDeInit 0 */
   /* Peripheral clock disable */
   __HAL_RCC_DAC_CLK_DISABLE();
   /**DAC GPIO Configuration
   PA4     ------> DAC_OUT1
   */
   HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);
   /* USER CODE BEGIN DAC_MspDeInit 1 */
   /* USER CODE END DAC_MspDeInit 1 */
 }
}
/**
 * @brief I2C MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hi2c: I2C handle pointer
 * @retval None
 */
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};
 if(hi2c->Instance==I2C1)
 {
   /* USER CODE BEGIN I2C1_MspInit 0 */
   /* USER CODE END I2C1_MspInit 0 */
   __HAL_RCC_GPIOB_CLK_ENABLE();
   /**I2C1 GPIO Configuration
   PB6     ------> I2C1_SCL
   PB7     ------> I2C1_SDA
   */
   GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
   GPIO_InitStruct.Alternate = GPIO_AF1_I2C1;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
   /* Peripheral clock enable */
   __HAL_RCC_I2C1_CLK_ENABLE();
   /* USER CODE BEGIN I2C1_MspInit 1 */
   /* USER CODE END I2C1_MspInit 1 */
 }
}
/**
 * @brief I2C MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hi2c: I2C handle pointer
 * @retval None
 */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
 if(hi2c->Instance==I2C1)
 {
   /* USER CODE BEGIN I2C1_MspDeInit 0 */
   /* USER CODE END I2C1_MspDeInit 0 */
   /* Peripheral clock disable */
   __HAL_RCC_I2C1_CLK_DISABLE();
   /**I2C1 GPIO Configuration
   PB6     ------> I2C1_SCL
   PB7     ------> I2C1_SDA
   */
   HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);
   HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);
   /* USER CODE BEGIN I2C1_MspDeInit 1 */
   /* USER CODE END I2C1_MspDeInit 1 */
 }
}
/**
 * @brief LCD MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hlcd: LCD handle pointer
 * @retval None
 */
void HAL_LCD_MspInit(LCD_HandleTypeDef* hlcd)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};
 if(hlcd->Instance==LCD)
 {
   /* USER CODE BEGIN LCD_MspInit 0 */
   /* USER CODE END LCD_MspInit 0 */
   /* Peripheral clock enable */
   __HAL_RCC_LCD_CLK_ENABLE();
   __HAL_RCC_GPIOE_CLK_ENABLE();
   __HAL_RCC_GPIOC_CLK_ENABLE();
   __HAL_RCC_GPIOD_CLK_ENABLE();
   __HAL_RCC_GPIOA_CLK_ENABLE();
   __HAL_RCC_GPIOB_CLK_ENABLE();
   /**LCD GPIO Configuration
   PE2     ------> LCD_SEG38
   PC4     ------> LCD_SEG22
   PD8     ------> LCD_SEG28
   PD9     ------> LCD_SEG29
   PD10     ------> LCD_SEG30
   PD11     ------> LCD_SEG31
   PD12     ------> LCD_SEG32
   PA8     ------> LCD_COM0
   PA9     ------> LCD_COM1
   PA10     ------> LCD_COM2
   PB3     ------> LCD_SEG7
   PB9     ------> LCD_COM3
   */
   GPIO_InitStruct.Pin = GPIO_PIN_2;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Alternate = GPIO_AF1_LCD;
   HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
   GPIO_InitStruct.Pin = GPIO_PIN_4;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Alternate = GPIO_AF1_LCD;
   HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
   GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                         |GPIO_PIN_12;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Alternate = GPIO_AF1_LCD;
   HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
   GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Alternate = GPIO_AF1_LCD;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
   GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_9;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Alternate = GPIO_AF1_LCD;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
   /* USER CODE BEGIN LCD_MspInit 1 */
   /* USER CODE END LCD_MspInit 1 */
 }
}
/**
 * @brief LCD MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hlcd: LCD handle pointer
 * @retval None
 */
void HAL_LCD_MspDeInit(LCD_HandleTypeDef* hlcd)
{
 if(hlcd->Instance==LCD)
 {
   /* USER CODE BEGIN LCD_MspDeInit 0 */
//
   /* USER CODE END LCD_MspDeInit 0 */
   /* Peripheral clock disable */
   __HAL_RCC_LCD_CLK_DISABLE();
   /**LCD GPIO Configuration
   PE2     ------> LCD_SEG38
   PC4     ------> LCD_SEG22
   PD8     ------> LCD_SEG28
   PD9     ------> LCD_SEG29
   PD10     ------> LCD_SEG30
   PD11     ------> LCD_SEG31
   PD12     ------> LCD_SEG32
   PA8     ------> LCD_COM0
   PA9     ------> LCD_COM1
   PA10     ------> LCD_COM2
   PB3     ------> LCD_SEG7
   PB9     ------> LCD_COM3
   */
   HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2);
   HAL_GPIO_DeInit(GPIOC, GPIO_PIN_4);
   HAL_GPIO_DeInit(GPIOD, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                         |GPIO_PIN_12);
   HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10);
   HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_9);
   /* USER CODE BEGIN LCD_MspDeInit 1 */
//
   /* USER CODE END LCD_MspDeInit 1 */
 }
}
/**
 * @brief LPTIM MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hlptim: LPTIM handle pointer
 * @retval None
 */
void HAL_LPTIM_MspInit(LPTIM_HandleTypeDef* hlptim)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};
 if(hlptim->Instance==LPTIM1)
 {
   /* USER CODE BEGIN LPTIM1_MspInit 0 */
   /* USER CODE END LPTIM1_MspInit 0 */
   /* Peripheral clock enable */
   __HAL_RCC_LPTIM1_CLK_ENABLE();
   __HAL_RCC_GPIOC_CLK_ENABLE();
   /**LPTIM1 GPIO Configuration
   PC0     ------> LPTIM1_IN1
   */
   GPIO_InitStruct.Pin = GPIO_PIN_0;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Alternate = GPIO_AF0_LPTIM1;
   HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
   /* USER CODE BEGIN LPTIM1_MspInit 1 */
   /* USER CODE END LPTIM1_MspInit 1 */
 }
}
/**
 * @brief LPTIM MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hlptim: LPTIM handle pointer
 * @retval None
 */
void HAL_LPTIM_MspDeInit(LPTIM_HandleTypeDef* hlptim)
{
 if(hlptim->Instance==LPTIM1)
 {
   /* USER CODE BEGIN LPTIM1_MspDeInit 0 */
   /* USER CODE END LPTIM1_MspDeInit 0 */
   /* Peripheral clock disable */
   __HAL_RCC_LPTIM1_CLK_DISABLE();
   /**LPTIM1 GPIO Configuration
   PC0     ------> LPTIM1_IN1
   */
   HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0);
   /* USER CODE BEGIN LPTIM1_MspDeInit 1 */
   /* USER CODE END LPTIM1_MspDeInit 1 */
 }
}
/**
 * @brief RTC MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hrtc: RTC handle pointer
 * @retval None
 */
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
 if(hrtc->Instance==RTC)
 {
   /* USER CODE BEGIN RTC_MspInit 0 */
   /* USER CODE END RTC_MspInit 0 */
   /* Peripheral clock enable */
   __HAL_RCC_RTC_ENABLE();
   /* USER CODE BEGIN RTC_MspInit 1 */
   /* USER CODE END RTC_MspInit 1 */
 }
}
/**
 * @brief RTC MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hrtc: RTC handle pointer
 * @retval None
 */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{
 if(hrtc->Instance==RTC)
 {
   /* USER CODE BEGIN RTC_MspDeInit 0 */
   /* USER CODE END RTC_MspDeInit 0 */
   /* Peripheral clock disable */
   __HAL_RCC_RTC_DISABLE();
   /* USER CODE BEGIN RTC_MspDeInit 1 */
   /* USER CODE END RTC_MspDeInit 1 */
 }
}
/**
 * @brief SPI MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hspi: SPI handle pointer
 * @retval None
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};
 if(hspi->Instance==SPI1)
 {
   /* USER CODE BEGIN SPI1_MspInit 0 */
   /* USER CODE END SPI1_MspInit 0 */
   /* Peripheral clock enable */
   __HAL_RCC_SPI1_CLK_ENABLE();
   __HAL_RCC_GPIOE_CLK_ENABLE();
   /**SPI1 GPIO Configuration
   PE13     ------> SPI1_SCK
   PE14     ------> SPI1_MISO
   PE15     ------> SPI1_MOSI
   */
   GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
   GPIO_InitStruct.Alternate = GPIO_AF2_SPI1;
   HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
   /* USER CODE BEGIN SPI1_MspInit 1 */
   /* USER CODE END SPI1_MspInit 1 */
 }
}
/**
 * @brief SPI MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hspi: SPI handle pointer
 * @retval None
 */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
 if(hspi->Instance==SPI1)
 {
   /* USER CODE BEGIN SPI1_MspDeInit 0 */
   /* USER CODE END SPI1_MspDeInit 0 */
   /* Peripheral clock disable */
   __HAL_RCC_SPI1_CLK_DISABLE();
   /**SPI1 GPIO Configuration
   PE13     ------> SPI1_SCK
   PE14     ------> SPI1_MISO
   PE15     ------> SPI1_MOSI
   */
   HAL_GPIO_DeInit(GPIOE, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
   /* USER CODE BEGIN SPI1_MspDeInit 1 */
   /* USER CODE END SPI1_MspDeInit 1 */
 }
}
/**
 * @brief TIM_Base MSP Initialization
 * This function configures the hardware resources used in this example
 * @param htim_base: TIM_Base handle pointer
 * @retval None
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
 if(htim_base->Instance==TIM2)
 {
   /* USER CODE BEGIN TIM2_MspInit 0 */
   /* USER CODE END TIM2_MspInit 0 */
   /* Peripheral clock enable */
   __HAL_RCC_TIM2_CLK_ENABLE();
   /* TIM2 interrupt Init */
   HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
   HAL_NVIC_EnableIRQ(TIM2_IRQn);
   /* USER CODE BEGIN TIM2_MspInit 1 */
   /* USER CODE END TIM2_MspInit 1 */
 }
}
/**
 * @brief TIM_Base MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param htim_base: TIM_Base handle pointer
 * @retval None
 */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
 if(htim_base->Instance==TIM2)
 {
   /* USER CODE BEGIN TIM2_MspDeInit 0 */
   /* USER CODE END TIM2_MspDeInit 0 */
   /* Peripheral clock disable */
   __HAL_RCC_TIM2_CLK_DISABLE();
   /* TIM2 interrupt DeInit */
   HAL_NVIC_DisableIRQ(TIM2_IRQn);
   /* USER CODE BEGIN TIM2_MspDeInit 1 */
   /* USER CODE END TIM2_MspDeInit 1 */
 }
}
/**
 * @brief TSC MSP Initialization
 * This function configures the hardware resources used in this example
 * @param htsc: TSC handle pointer
 * @retval None
 */
void HAL_TSC_MspInit(TSC_HandleTypeDef* htsc)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};
 if(htsc->Instance==TSC)
 {
   /* USER CODE BEGIN TSC_MspInit 0 */
   /* USER CODE END TSC_MspInit 0 */
   /* Peripheral clock enable */
   __HAL_RCC_TSC_CLK_ENABLE();
   __HAL_RCC_GPIOA_CLK_ENABLE();
   __HAL_RCC_GPIOB_CLK_ENABLE();
   /**TSC GPIO Configuration
   PA1     ------> TSC_G1_IO2
   PA2     ------> TSC_G1_IO3
   PB0     ------> TSC_G3_IO2
   PB1     ------> TSC_G3_IO3
   PB12     ------> TSC_G6_IO2
   PB13     ------> TSC_G6_IO3
   */
   GPIO_InitStruct.Pin = GPIO_PIN_1;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Alternate = GPIO_AF3_TSC;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
   GPIO_InitStruct.Pin = GPIO_PIN_2;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Alternate = GPIO_AF3_TSC;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
   GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_12;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Alternate = GPIO_AF3_TSC;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
   GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_13;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Alternate = GPIO_AF3_TSC;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
   /* USER CODE BEGIN TSC_MspInit 1 */
   /* USER CODE END TSC_MspInit 1 */
 }
}
/**
 * @brief TSC MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param htsc: TSC handle pointer
 * @retval None
 */
void HAL_TSC_MspDeInit(TSC_HandleTypeDef* htsc)
{
 if(htsc->Instance==TSC)
 {
   /* USER CODE BEGIN TSC_MspDeInit 0 */
   /* USER CODE END TSC_MspDeInit 0 */
   /* Peripheral clock disable */
   __HAL_RCC_TSC_CLK_DISABLE();
   /**TSC GPIO Configuration
   PA1     ------> TSC_G1_IO2
   PA2     ------> TSC_G1_IO3
   PB0     ------> TSC_G3_IO2
   PB1     ------> TSC_G3_IO3
   PB12     ------> TSC_G6_IO2
   PB13     ------> TSC_G6_IO3
   */
   HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1|GPIO_PIN_2);
   HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_12|GPIO_PIN_13);
   /* USER CODE BEGIN TSC_MspDeInit 1 */
   /* USER CODE END TSC_MspDeInit 1 */
 }
}
/**
 * @brief UART MSP Initialization
 * This function configures the hardware resources used in this example
 * @param huart: UART handle pointer
 * @retval None
 */
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};
 if(huart->Instance==USART4)
 {
   /* USER CODE BEGIN USART4_MspInit 0 */
   /* USER CODE END USART4_MspInit 0 */
   /* Peripheral clock enable */
   __HAL_RCC_USART4_CLK_ENABLE();
   __HAL_RCC_GPIOE_CLK_ENABLE();
   /**USART4 GPIO Configuration
   PE8     ------> USART4_TX
   PE9     ------> USART4_RX
   */
   GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
   GPIO_InitStruct.Alternate = GPIO_AF6_USART4;
   HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
   /* USART4 DMA Init */
   /* USART4_RX Init */
   hdma_usart4_rx.Instance = DMA1_Channel2;
   hdma_usart4_rx.Init.Request = DMA_REQUEST_12;
   hdma_usart4_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
   hdma_usart4_rx.Init.PeriphInc = DMA_PINC_DISABLE;
   hdma_usart4_rx.Init.MemInc = DMA_MINC_ENABLE;
   hdma_usart4_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
   hdma_usart4_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
   hdma_usart4_rx.Init.Mode = DMA_NORMAL;
   hdma_usart4_rx.Init.Priority = DMA_PRIORITY_LOW;
   if (HAL_DMA_Init(&hdma_usart4_rx) != HAL_OK)
   {
     Error_Handler();
   }
   __HAL_LINKDMA(huart,hdmarx,hdma_usart4_rx);
   /* USART4_TX Init */
   hdma_usart4_tx.Instance = DMA1_Channel3;
   hdma_usart4_tx.Init.Request = DMA_REQUEST_12;
   hdma_usart4_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
   hdma_usart4_tx.Init.PeriphInc = DMA_PINC_DISABLE;
   hdma_usart4_tx.Init.MemInc = DMA_MINC_ENABLE;
   hdma_usart4_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
   hdma_usart4_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
   hdma_usart4_tx.Init.Mode = DMA_NORMAL;
   hdma_usart4_tx.Init.Priority = DMA_PRIORITY_LOW;
   if (HAL_DMA_Init(&hdma_usart4_tx) != HAL_OK)
   {
     Error_Handler();
   }
   __HAL_LINKDMA(huart,hdmatx,hdma_usart4_tx);
   /* USART4 interrupt Init */
   HAL_NVIC_SetPriority(USART4_5_IRQn, 0, 0);
   HAL_NVIC_EnableIRQ(USART4_5_IRQn);
   /* USER CODE BEGIN USART4_MspInit 1 */
   /* USER CODE END USART4_MspInit 1 */
 }
}
/**
 * @brief UART MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param huart: UART handle pointer
 * @retval None
 */
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
 if(huart->Instance==USART4)
 {
   /* USER CODE BEGIN USART4_MspDeInit 0 */
   /* USER CODE END USART4_MspDeInit 0 */
   /* Peripheral clock disable */
   __HAL_RCC_USART4_CLK_DISABLE();
   /**USART4 GPIO Configuration
   PE8     ------> USART4_TX
   PE9     ------> USART4_RX
   */
   HAL_GPIO_DeInit(GPIOE, GPIO_PIN_8|GPIO_PIN_9);
   /* USART4 DMA DeInit */
   HAL_DMA_DeInit(huart->hdmarx);
   HAL_DMA_DeInit(huart->hdmatx);
   /* USART4 interrupt DeInit */
   HAL_NVIC_DisableIRQ(USART4_5_IRQn);
   /* USER CODE BEGIN USART4_MspDeInit 1 */
   /* USER CODE END USART4_MspDeInit 1 */
 }
}
/**
 * @brief PCD MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hpcd: PCD handle pointer
 * @retval None
 */
void HAL_PCD_MspInit(PCD_HandleTypeDef* hpcd)
{
 if(hpcd->Instance==USB)
 {
   /* USER CODE BEGIN USB_MspInit 0 */
   /* USER CODE END USB_MspInit 0 */
   /* Peripheral clock enable */
   __HAL_RCC_USB_CLK_ENABLE();
   /* USER CODE BEGIN USB_MspInit 1 */
   /* USER CODE END USB_MspInit 1 */
 }
}
/**
 * @brief PCD MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hpcd: PCD handle pointer
 * @retval None
 */
void HAL_PCD_MspDeInit(PCD_HandleTypeDef* hpcd)
{
 if(hpcd->Instance==USB)
 {
   /* USER CODE BEGIN USB_MspDeInit 0 */
   /* USER CODE END USB_MspDeInit 0 */
   /* Peripheral clock disable */
   __HAL_RCC_USB_CLK_DISABLE();
   /* USER CODE BEGIN USB_MspDeInit 1 */
   /* USER CODE END USB_MspDeInit 1 */
 }
}
/* USER CODE BEGIN 1 */
/* USER CODE END 1 */

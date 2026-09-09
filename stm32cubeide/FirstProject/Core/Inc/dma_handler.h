#ifndef DMA_HANDLER_H_
#define DMA_HANDLER_H_
#include "stm32l0xx_hal.h"
#include <string.h>
#include <stdio.h>
#include "ui_manager.h"
// --- BUFFER PARAMETRELERİ ---
#define CMD_BUFFER_SIZE  64   // 1 Paket Max 64 Byte
#define RING_BUFFER_SLOTS 10  // 10 Adet Dairesel Yuva (Slot)
                             // Toplam RAM Maliyeti: 64 * 10 = 640 Byte
// Fonksiyon Prototipleri
void DMA_Handler_Init(UART_HandleTypeDef *huart);
void DMA_Process_In_Main(UART_HandleTypeDef *huart);
#endif /* DMA_HANDLER_H_ */

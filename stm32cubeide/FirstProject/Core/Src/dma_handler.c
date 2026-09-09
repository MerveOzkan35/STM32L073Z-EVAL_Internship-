#include <dma_handler.h>
#include "logger_eeprom.h"
// 1. Dairesel Matris (10 Satır x 64 Sütun)
static uint8_t ring_buffer[RING_BUFFER_SLOTS][CMD_BUFFER_SIZE];
// 2. Ring Buffer İndeksleri
static volatile uint8_t rx_head = 0; // DMA'nın yeni veriyi yazacağı slot
static volatile uint8_t rx_tail = 0; // main()'in okuyacağı slot
// 3. Yanıt Mesajı (DMA Transmit için static olması şarttır)
static char tx_msg[64];
/**
 * @brief DMA Dinlemesini Bir Sonraki Slota Yönlendirir (Yazılımsal Circular)
 */
static void DMA_Start_Listening_Next_Slot(UART_HandleTypeDef *huart)
{
   // DMA'yı doğrudan matrisin bir sonraki 'head' satırının adresine yönlendiriyoruz
   HAL_UARTEx_ReceiveToIdle_DMA(huart, ring_buffer[rx_head], CMD_BUFFER_SIZE);
   // Half-Transfer kesmesini kapatıyoruz (Gerekmedikçe CPU'yu meşgul etmesin)
   __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
}
/**
 * @brief DMA İlk Kurulumu
 */
void DMA_Handler_Init(UART_HandleTypeDef *huart)
{
   memset(ring_buffer, 0, sizeof(ring_buffer));
   rx_head = 0;
   rx_tail = 0;
   DMA_Start_Listening_Next_Slot(huart);
}
/**
 * @brief UART IDLE Kesmesi - KESME İÇİNDE ÇALIŞIR (Çok Hızlı!)
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
   if (huart->Instance == USART4) 
   {
       // 1. O anki slota gelen string'in sonuna NULL ekle (Kopyalama YAPILMIYOR)
       ring_buffer[rx_head][Size] = '\0';
       // 2. Dairesel İndeks Kaydırma: Head bir sonraki slota kaydırılır (0..9 arası döner)
       uint8_t next_head = (rx_head + 1) % RING_BUFFER_SLOTS;
       // Taşma (Overflow) Kontrolü:
       // Eğer yeni head, tail'a yetiştiyse RAM dolmuştur! Veri ezilmesin diye ilerletmiyoruz.
       if (next_head != rx_tail)
       {
           rx_head = next_head;
       }
       // 3. DMA'yı kaydırılan yeni slota dinlemeye alıyoruz!
       DMA_Start_Listening_Next_Slot(huart);
   }
}
/**
 * @brief Main Döngüsünde Sürekli Çağrılan İşlemci Fonksiyonu
 */
void DMA_Process_In_Main(UART_HandleTypeDef *huart)
{
   if (rx_head == rx_tail)
   {
       return;
   }
   uint8_t *curr_buf = ring_buffer[rx_tail];
	// =========================================================================
   // 1. KONTROL: Python'dan Gelen Binary Paket mi? [0xAA, 0x55, 0x04, T, P, 0x0D]
   // =========================================================================
   if (curr_buf[0] == 0xAA && curr_buf[1] == 0x55)
   {
   	uint8_t cmd = curr_buf[2];
   	if (cmd == 0x04 && curr_buf[5] == 0x0D) // Eşik Değiştirme Komutu
   	{
   		// Sıcaklık Eşiği: (x2) hassasiyeti ile geliyor. Örn: 26.5C -> 53 -> 53/2 = 26.5C
   		g_ui_config.max_temp_threshold = ((float)curr_buf[3]) / 2.0f;
   		// Basınç Eşiği: 900 offset ile geliyor. Örn: 50 -> 950 hPa
   		g_ui_config.max_press_threshold = ((float)curr_buf[4]) + 900.0f;
   		snprintf(tx_msg, sizeof(tx_msg), "[OK]: Esikler Guncellendi T:%.1f P:%.1f\r\n",
   				g_ui_config.max_temp_threshold, g_ui_config.max_press_threshold);
			HAL_UART_Transmit_DMA(huart, (uint8_t*)tx_msg, strlen(tx_msg));
		}
	}
   // =========================================================================
   // 2. KONTROL: Hercules veya Terminalden Gelen String Komutu mu?
   // =========================================================================
   else
   {
   	char *current_cmd = (char*)ring_buffer[rx_tail];
   	// \r, \n ve boşluk temizliği
   	char *pos;
   	if ((pos = strchr(current_cmd, '\r')) != NULL) *pos = '\0';
   	if ((pos = strchr(current_cmd, '\n')) != NULL) *pos = '\0';
   	if (strcmp(current_cmd, "led1_on") == 0)
   	{
   		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET);
   		snprintf(tx_msg, sizeof(tx_msg), "[OK]: LED1 Yandi\r\n");
   		HAL_UART_Transmit_DMA(huart, (uint8_t*)tx_msg, strlen(tx_msg));
   	}
   	else if (strcmp(current_cmd, "led1_off") == 0)
   	{
   		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET);
   		snprintf(tx_msg, sizeof(tx_msg), "[OK]: LED1 Sondu\r\n");
   		HAL_UART_Transmit_DMA(huart, (uint8_t*)tx_msg, strlen(tx_msg));
   	}
   	else if (strcmp(current_cmd, "led2_on") == 0)
   	{
   		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
   		snprintf(tx_msg, sizeof(tx_msg), "[OK]: LED2 Yandi\r\n");
   		HAL_UART_Transmit_DMA(huart, (uint8_t*)tx_msg, strlen(tx_msg));
   	}
   	else if (strcmp(current_cmd, "led2_off") == 0)
   	{
   		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);
   		snprintf(tx_msg, sizeof(tx_msg), "[OK]: LED2 Sondu\r\n");
   		HAL_UART_Transmit_DMA(huart, (uint8_t*)tx_msg, strlen(tx_msg));
   	}
   	/* EEPROM Komutları: 'e', 'E' veya 'eeprom' geldiyse döküm yap */
   	else if (strcmp(current_cmd, "e") == 0 || strcmp(current_cmd, "E") == 0 || strcmp(current_cmd, "eeprom") == 0)
   	{
   		Logger_ExportToUART(huart);
   	}
   	else if (strlen(current_cmd) > 0)
   	{
   		snprintf(tx_msg, sizeof(tx_msg), "[ERR]: Bilinmeyen Komut: %s\r\n", current_cmd);
   		HAL_UART_Transmit_DMA(huart, (uint8_t*)tx_msg, strlen(tx_msg));
   	}
   }
   rx_tail = (rx_tail + 1) % RING_BUFFER_SLOTS;
}

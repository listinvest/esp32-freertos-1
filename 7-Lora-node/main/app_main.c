#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "lora.h"

SemaphoreHandle_t xMutex;

EventGroupHandle_t lora_flags;
const uint32_t CTS_BIT = BIT0;
const uint32_t RTS_BIT = BIT1;
const uint32_t PYD_BIT = BIT2;
const uint32_t ACK_BIT = BIT3;
const uint32_t ALL_BITS = (BIT3|BIT2|BIT1|BIT0);

const uint8_t CTS_FLAG[32] = "CTS";
const uint8_t ACK_FLAG[8] = "ACK";

uint8_t payload[8] = "PAYLOAD";

void lora_nd_receive(void* p)
{
   uint8_t buf[32];
   int x = 0;
   for(;;){
      if ( xSemaphoreTake(xMutex, (TickType_t)0xFFFFFFFF) == 1 ){
         lora_enable_invertiq();
         lora_receive();
         while(lora_received()) {
            x = lora_receive_packet(buf, sizeof(buf));
            buf[x] = 0;
            printf("Received: %s\n", buf);
            lora_receive();
         }
         xSemaphoreGive(xMutex);
      }
      if( buf == CTS_FLAG ) xEventGroupSetBits(lora_flags, CTS_BIT);
      else if ( buf == ACK_FLAG ) xEventGroupSetBits(lora_flags, ACK_BIT);

      vTaskDelay(pdMS_TO_TICKS(100));
   }
}

void lora_nd_send(void *tx_msg)
{
   if ( xSemaphoreTake(xMutex, (TickType_t)0xFFFFFFFF) == 1 ){
      lora_idle();
      lora_disable_invertiq();

      lora_send_packet((uint8_t*) tx_msg, sizeof(tx_msg));

      lora_enable_invertiq();
      lora_receive();

      xSemaphoreGive(xMutex);
   }
}

void app_main()
{
   lora_init();
   lora_set_frequency(915e6);
   lora_enable_crc();

   xMutex = xSemaphoreCreateMutex();

   xTaskCreate(&lora_nd_send, "lora_comm", 2048, "RTS", 10, NULL);
   xTaskCreate(&lora_nd_receive, "lora_rece", 2048, NULL, 5, NULL);
}
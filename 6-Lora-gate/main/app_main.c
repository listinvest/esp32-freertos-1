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

void lora_gw_receive(void *p)
{
   uint8_t buf[32];
   int x;
   for(;;) {
      if ( xSemaphoreTake(xMutex, (TickType_t) 0xFFFFFFFF) == 1 ){
         lora_disable_invertiq();
         lora_receive();    // put into receive mode
         while(lora_received()) {
            x = lora_receive_packet(buf, sizeof(buf));
            buf[x] = 0;
            printf("Received: %s\n", buf);
            lora_receive();
         }
         xSemaphoreGive(xMutex);
      }


      vTaskDelay(pdMS_TO_TICKS(100));
   }

}

void lora_gw_send(void *p)
{
   if ( xSemaphoreTake(xMutex, (TickType_t) 0xFFFFFFFF) == 1 ){
      lora_idle();
      lora_enable_invertiq();

      lora_send_packet((uint8_t*) p, sizeof(p));

      lora_disable_invertiq();
      lora_receive();

      xSemaphoreGive(xMutex);
   }
}

void comm_task( void *p )
{
   while(1){
      uint32_t comm_bits = xEventGroupWaitBits(   //ESPERA RTS
         lora_flags,
         RTS_BIT,
         false,
         true,
         pdMS_TO_TICKS(1000)
      );

      if(comm_bits == RTS_BIT){
         lora_set_frequency(920e6);
         lora_gw_send( "CTS" );
         lora_set_frequency(915e6);
         xEventGroupSetBits(lora_flags, CTS_BIT);  //ENVIAR CTS
      }

      comm_bits = xEventGroupWaitBits(
         lora_flags,
         (PYD_BIT|CTS_BIT|RTS_BIT),
         false,
         true,
         pdMS_TO_TICKS(2000);
      );
   }
}

void app_main()
{
   lora_init();
   lora_set_frequency(915e6);
   lora_enable_crc();

   xMutex = xSemaphoreCreateMutex();

   xTaskCreatePinnedToCore(&lora_gw_receive,"lora_gw_receive", 2048, NULL, 5, NULL, 0);

   xTaskCreatePinnedToCore(&lora_gw_send, "lora_gw_send", 2048, "gate", 5, NULL, 0);
}
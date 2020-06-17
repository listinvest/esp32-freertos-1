#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lora.h"

uint8_t buf[32];



void lora_gw_receive(void *p)
{
   int x;
   for(;;) {
      lora_disable_invertiq();
      lora_receive();    // put into receive mode
      while(lora_received()) {
         x = lora_receive_packet(buf, sizeof(buf));
         buf[x] = 0;
         printf("Received: %s\n", buf);
         lora_receive();
      }
      vTaskDelay(pdMS_TO_TICKS(10));
   }
}

void lora_gw_send(void *p)
{
   while(1){
      lora_idle();
      lora_enable_invertiq();

      lora_send_packet((uint8_t*) p, sizeof(p));

      lora_disable_invertiq();
      lora_receive();

      printf("packet sent...\n");
      vTaskDelay(pdMS_TO_TICKS(1000));
   }
}

void app_main()
{
   lora_init();
   lora_set_frequency(915e6);
   lora_enable_crc();

   xTaskCreatePinnedToCore(
      &lora_gw_send,
      "lora_gw_send",
      configMINIMAL_STACK_SIZE * 3,
      "gate message",
      tskIDLE_PRIORITY + 5,
      NULL,
      0
   );

   xTaskCreatePinnedToCore(
      &lora_gw_receive,
      "lora_gw_receive",
      configMINIMAL_STACK_SIZE * 3,
      NULL,
      tskIDLE_PRIORITY + 5,
      NULL,
      0
   );
}
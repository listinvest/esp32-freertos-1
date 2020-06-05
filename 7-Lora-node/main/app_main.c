#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lora.h"

uint8_t buf[32];



void lora_nd_receive(void *p)
{
   int x;
   for(;;) {
      lora_enable_invertiq();
      lora_receive();    // put into receive mode
      while(lora_received()) {
         x = lora_receive_packet(buf, sizeof(buf));
         buf[x] = 0;
         printf("Received: %s\n", buf);
         lora_receive();
      }
      vTaskDelay(pdMS_TO_TICKS(100));
   }
}

void lora_nd_send(void *p)
{
   while(1){
      lora_idle();
      lora_disable_invertiq();

      lora_send_packet((uint8_t*) p, sizeof(p));

      lora_enable_invertiq();
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
      &lora_nd_send,
      "lora_nd_send",
      configMINIMAL_STACK_SIZE * 3,
      "node message",
      tskIDLE_PRIORITY + 5,
      NULL,
      0
   );

   xTaskCreatePinnedToCore(
      &lora_nd_receive,
      "lora_nd_receive",
      configMINIMAL_STACK_SIZE * 3,
      NULL,
      tskIDLE_PRIORITY + 5,
      NULL,
      0
   );
}
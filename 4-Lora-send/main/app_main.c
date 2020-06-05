#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lora.h"

void task_tx(void *p)
{
   uint8_t* message = p;
   for(;;) {
      lora_send_packet(message, sizeof(message));
      printf("packet sent...\n");
      vTaskDelay(pdMS_TO_TICKS(1000));
   }
}

void app_main()
{
   lora_init();
   lora_set_frequency(915e6);
   lora_enable_crc();
   xTaskCreate(&task_tx, "task_tx", 2048, "lol", configMAX_PRIORITIES-1, NULL);
}
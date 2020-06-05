/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "adcfix.h"

TaskHandle_t myTaskHandle = NULL;

void app_main()
{
    xTaskCreatePinnedToCore(
        &getAdcValue_Task,
        "getAdcValue_Task",
        configMINIMAL_STACK_SIZE * 3,
        NULL,
        configMAX_PRIORITIES - 1,
        myTaskHandle,
        portNUM_PROCESSORS - 2
    );
}

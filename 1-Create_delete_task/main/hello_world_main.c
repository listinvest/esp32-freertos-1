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

TaskHandle_t myTask1Handle1 = NULL;

void myTask1 ( void *p ){
    int count = (int *) p;
    while(1){
        printf( "count : %d \r\n", count ++);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        if(count == 30){
            vTaskDelete(myTask1Handle1);
        }
    }
}

void app_main()
{
    int pass = 25;

    xTaskCreatePinnedToCore(
        myTask1,
        "task1",
        configMINIMAL_STACK_SIZE * 4, 
        (void*) pass,
        tskIDLE_PRIORITY ,
        &myTask1Handle1,
        1
    );
}

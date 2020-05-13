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

TaskHandle_t myTaskHandle = NULL;

void myTask1 ( void *p ){
    while(1){
        printf( "MIN \r\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void myTask2 ( void *p ){
    while(1){
        printf( "MAX \r\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main()
{

    xTaskCreatePinnedToCore(
        myTask1,
        "task1",
        configMINIMAL_STACK_SIZE * 4, 
        (void*) 0,
        tskIDLE_PRIORITY ,
        &myTaskHandle,
        1
    );

    xTaskCreatePinnedToCore(
        myTask2,
        "task2",
        configMINIMAL_STACK_SIZE * 4, 
        (void*) 0,
        configMAX_PRIORITIES -1 ,
        &myTaskHandle,
        1
    );
}

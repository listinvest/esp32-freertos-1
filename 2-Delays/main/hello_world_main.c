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
#include "driver/gpio.h"
#include "esp_spi_flash.h"

void blink_task1(void *pvParameter)
{
    while(1) {
        printf("LIGA\n");
	    gpio_set_level(GPIO_NUM_21, 1);
        vTaskDelay(5000/portTICK_PERIOD_MS);
    }
}
 
void blink_task2(void *pvParameter)
{
    while(1) {
        printf("DESLIGA\n");
	    gpio_set_level(GPIO_NUM_21, 0);
        vTaskDelay(4000/portTICK_PERIOD_MS);
    }
}

void app_main()
{
    gpio_pad_select_gpio(GPIO_NUM_21); 
    gpio_set_direction(GPIO_NUM_21,GPIO_MODE_OUTPUT);

    xTaskCreatePinnedToCore( blink_task1, "blink_task1", 1024, NULL, 5, NULL, 0 );
    xTaskCreatePinnedToCore( blink_task2, "blink_task2", 1024, NULL, 5, NULL, 0 );
    printf("blink task started\n");
}

#include <header.h>

EventGroupHandle_t esp32_event_group = NULL;

void mqtt_tsk( void* p )
{
    while(1)
    {
        xEventGroupWaitBits(esp32_event_group, MQTT_PUBLISHED_BIT       , true, true, portMAX_DELAY);
        xEventGroupWaitBits(esp32_event_group, MQTT_INITIATE_PUBLISH_BIT, true, true, portMAX_DELAY);
        subscribe_cb(gb_mqttClient, NULL);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_main()
{
    nvs_flash_init();
    wifi_conn_init();

    xTaskCreate( mqtt_tsk, "mqtt_task",  2048, NULL, tskIDLE_PRIORITY + 4, NULL);
}

#include <header.h>

EventGroupHandle_t esp32_event_group = NULL;
const int WIFI_CONNECTED_BIT = BIT0;
const int MQTT_PUBLISHED_BIT = BIT1;
const int MQTT_INITIATE_PUBLISH_BIT = BIT2;

esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupSetBits(esp32_event_group, WIFI_CONNECTED_BIT);
            gb_mqttClient = mqtt_start(&settings);
            //init app here
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            // This is a workaround as ESP32 WiFi libs don't currently auto-reassociate.
            esp_wifi_connect();
            mqtt_stop();
            gb_mqttClient = NULL;
            xEventGroupClearBits(esp32_event_group, WIFI_CONNECTED_BIT);
            break;
        default:
            break;
    }
    return ESP_OK;
}

void GpioInit()
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = 1 << PIN_LED;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}

void app_main()
{
    nvs_flash_init();
    wifi_conn_init();
    GpioInit();
    
    xTaskCreate(&GetAdcValue_Task, "GetAdcValue_Task", 2048, NULL,  8, NULL);

    while(1)
    {
        xEventGroupWaitBits(esp32_event_group, MQTT_PUBLISHED_BIT, true, true, portMAX_DELAY);
        xEventGroupWaitBits(esp32_event_group, MQTT_INITIATE_PUBLISH_BIT, true, true, portMAX_DELAY);
        subscribe_cb(gb_mqttClient, NULL);
    }
}

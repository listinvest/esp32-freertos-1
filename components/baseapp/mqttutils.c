#include "mqttutils.h"

const char *MQTT_TAG = "MQTT_SAMPLE";

mqtt_client * gb_mqttClient = NULL;

void connected_cb(void *self, void *params)
{
    mqtt_client *client = (mqtt_client *)self;
    const char topic_subscribe[] = "sensors/"SENSOR_ID"/configuration";
    mqtt_subscribe(client, topic_subscribe, 1);
    /*
    const char topic_publish[] = "sensors/"SENSOR_ID"/values";
    const char topic_subscribe[] = "sensors/"SENSOR_ID"/configuration";
    const char body[] = "{\"Voltage\":3.3}";
    ESP_LOGI("connected_cb", "topic_publish: %s   body: %s", topic_publish, body);
    mqtt_publish(client, topic_publish, body, sizeof(body)-1, 1, 0);                            // sizeof()-1 to compensate for the trailing '\0' in the string
    */
}

void disconnected_cb(void *self, void *params)
{

}

void reconnect_cb(void *self, void *params)
{

}

void subscribe_cb(void *self, void *params)
{
    ESP_LOGI(MQTT_TAG, "[APP] Subscribe ok, test publish msg");
    const char topic_publish[] = "sensors/"SENSOR_ID"/values";
    char body[25];
    sprintf(body, "{\"Voltage\":%1.2f}", gb_voltage/1000.0);
    mqtt_client *client = (mqtt_client *)self;
    mqtt_publish(client, topic_publish, body, strlen(body), 1, 0);                            // sizeof()-1 to compensate for the trailing '\0' in the string
}

void publish_cb(void *self, void *params)
{
    xEventGroupSetBits(esp32_event_group, MQTT_PUBLISHED_BIT);
}

void data_cb(void *self, void *params)
{
    mqtt_client *client = (mqtt_client *)self;
    mqtt_event_data_t *event_data = (mqtt_event_data_t *)params;
    char *topic = NULL, *data = NULL;

    if(event_data->data_offset == 0) {

        topic = malloc(event_data->topic_length + 1);
        memcpy(topic, event_data->topic, event_data->topic_length);
        topic[event_data->topic_length] = 0;
        ESP_LOGI(MQTT_TAG, "[APP] Publish topic: %s", topic);
    }

    data = malloc(event_data->data_length + 1);
    memcpy(data, event_data->data, event_data->data_length);
    data[event_data->data_length] = 0;
    ESP_LOGI(MQTT_TAG, "[APP] Publish data[%d/%d bytes]",
             event_data->data_length + event_data->data_offset,
             event_data->data_total_length);
    ESP_LOGI(MQTT_TAG, "Publish Data: %s", data);

    DecodePublishResponse(topic, data);

    free(topic);
    free(data);
}

mqtt_settings settings = {
    .host = WEB_SERVER,
/*
#if defined(CONFIG_MQTT_SECURITY_ON)
    .port = 8883, // encrypted
#else
    .port = 1883, // unencrypted
#endif
*/
    //.port = WEB_PORT,
    .port = WEB_PORT,
    .client_id = "mqtt_" SENSOR_ID,
    .username = "sensor_" SENSOR_ID,
    .password = SENSOR_KEY,
    .clean_session = 0,
    .keepalive = 120,
    .lwt_topic = "",
    .lwt_msg = "",
    .lwt_qos = 0,
    .lwt_retain = 0,
    .connected_cb = connected_cb,
    .disconnected_cb = disconnected_cb,
    //.reconnect_cb = reconnect_cb,
    .subscribe_cb = subscribe_cb,
    .publish_cb = publish_cb,
    .data_cb = data_cb
};

void wifi_conn_init(void)
{
    tcpip_adapter_init();
    esp32_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_LOGI(MQTT_TAG, "start the WIFI SSID:[%s] password:[%s]", CONFIG_WIFI_SSID, "******");
    ESP_ERROR_CHECK(esp_wifi_start());
}
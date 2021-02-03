#ifndef __HEADER_H__
#define __HEADER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt.h"

#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "adcutils.h"
#include "stringutils.h"
#include "mqttutils.h"

#define WIFI_SSID                   "Neo"
#define WIFI_PASSWORD               "6138777771"
#define SOFTAP_SSID                 "ESP32AP"
#define SOFTAP_PASSWORD             "abcdefpassword"

#define WEB_SERVER                  "192.168.0.115"
#define WEB_PORT                    1883                                // should be an integer and not a string

#define SENSOR_ID                   "c6f900db58e9"
#define SENSOR_KEY                  "2d9a97f168db68ae6f94b1b547581c1f"

#define PIN_LED                     GPIO_NUM_22
#define POT_ADC_CHANNEL             (ADC1_CHANNEL_6)      //GPIO 34

#define LED_ON                      gpio_set_level(PIN_LED, 0)
#define LED_OFF                     gpio_set_level(PIN_LED, 1)

#define ADC_ACCURACY                4                                  //in percentage

extern EventGroupHandle_t esp32_event_group;
extern const int WIFI_CONNECTED_BIT;
extern const int MQTT_PUBLISHED_BIT;
extern const int MQTT_INITIATE_PUBLISH_BIT;

esp_err_t wifi_event_handler(void *ctx, system_event_t *event);

#endif //__HEADER_H__
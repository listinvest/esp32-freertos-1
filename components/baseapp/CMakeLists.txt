idf_component_register(SRCS "adcutils.c" "mqttutils.c" "stringutils.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver
                    REQUIRES esp_adc_cal
                    REQUIRES nvs_flash
                    REQUIRES espmqtt)

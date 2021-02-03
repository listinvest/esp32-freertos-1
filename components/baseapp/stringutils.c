#include "stringutils.h"

//char

bool DecodePublishResponse(char *topic, char *data)
{
    if(strstr(topic, "sensors/"SENSOR_ID"/values") != NULL)
        return false;

    if(strstr(data, "switch") != NULL)
    {
        if(strstr(data, "false"))
        {
            ESP_LOGI("DecodePublishResponse", "switch = false");
            LED_OFF;
        }
        else if(strstr(data, "true"))
        {
            ESP_LOGI("DecodePublishResponse", "switch = true");
            LED_ON;
        }
        return true;
    }
    return false;
}
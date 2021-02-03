#ifndef __ADCFIX_H__
#define __ADCFIX_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include <driver/adc.h>
#include <esp_adc_cal.h>


#define V_REF               1072
#define ADC1_TEST_CHANNEL   (ADC1_CHANNEL_6)      //GPIO 34

void getAdcValue_Task();

extern uint32_t gb_voltage;

#endif //__ADCFIX_H__
#ifndef __ADCUTILS_H__
#define __ADCUTILS_H__

#include "header.h"

#define V_REF               1100
#define ADC1_TEST_CHANNEL   (ADC1_CHANNEL_6)      //GPIO 34

void GetAdcValue_Task();

extern uint32_t gb_voltage;

#endif //__ADCUTILS_H__
#include <adcfix.h>

uint32_t gb_voltage;

esp_adc_cal_characteristics_t characteristics;

void config_adc1(){

	adc1_config_width ( ADC_WIDTH_BIT_12 );
	adc1_config_channel_atten ( ADC1_CHANNEL_0, ADC_ATTEN_DB_11 );
	
	esp_adc_cal_characterize ( ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, V_REF, &characteristics);

}

void GetAdcValue_Task(){

	config_adc1();
	while(1){
		uint32_t voltage = 0;
		for( int i = 0; i < 100; i++){
			voltage += adc1_get_raw( ADC1_CHANNEL_0 );
			ets_delay_us( 30 );
		}
		voltage /= 100;
		gb_voltage = esp_adc_cal_raw_to_voltage( voltage, &characteristics );
		ESP_LOGI("ADC CAL", "Read mV: %u", gb_voltage);
		vTaskDelay(5000 / portTICK_RATE_MS);
	}
}
/* DoorWay_LED_test18
 *
 * Test Ver.18
 * Use ADC to read analogue voltages of IR range sensors
 * Use ADC to read wifi enable button response
 * Use http server (lwip/netconn) as testing, socket server for ota image upload
 *
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "driver/adc.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_event_loop.h"

//General Setting
#include "sdkconfig.h"

//Custom Library
#include "nvsFunction.h"
#include "wifiFunction.h"
#include "httpServer.h"

//Sensor & Jumper settings
#define IRSensor_Long1 			(ADC1_CHANNEL_0)
#define IRSensor_Long2 			(ADC1_CHANNEL_3)
#define IRSensor_Short 			(ADC1_CHANNEL_6)
#define PhotoResistor 			(ADC1_CHANNEL_7)
#define WIFI_Trigger 			(ADC1_CHANNEL_4)
#define SENSOR_POWER_PIN 		GPIO_NUM_22
#define LED_POWER_PIN 			GPIO_NUM_21
#define ESP_INTR_FLAG_DEFAULT 	0

//LOG TAG
#define SYS_TAG 				"[SYS]"
#define IR_TAG 					"[IR]"
#define PR_TAG 					"[PR]"

EventGroupHandle_t wifi_event_group;

//Firmware Version
char VERSION[]="1.00";

//IR Setting
bool IR_Power=false;
char *key_name[] = {"PR_Trig", "IR_Long1_Trig", "IR_Long2_Trig", "IR_Short_Trig",  "CountDown"};
char *value_name[] = {"", "", "", "", ""};
char *display_name[] = {"PhotoResistor [0 - 4095] :", "IR_Long1 [0 - 4095] :", "IR_Long2 [0 - 4095] :", "IR(Short) [0 - 4095] :", "Light On Time [ms] :"};

int16_t PR_Trig=0, IR_Long1_Trig=0, IR_Long2_Trig=0, IR_Short_Trig=0,  CountDown=0;
int default_value[] = {3500, 2700, 2700, 1500, 1500};

char JSON_message[60];

//General
esp_err_t err;

int parameters_no;

void ADC_Init() {
	// initialize ADC
	adc1_config_width(ADC_WIDTH_12Bit);
	adc1_config_channel_atten(IRSensor_Long1, ADC_ATTEN_11db);	//Long Range IR sensor 1
	adc1_config_channel_atten(IRSensor_Long2, ADC_ATTEN_11db);	//Long Range IR sensor 2
	adc1_config_channel_atten(IRSensor_Short, ADC_ATTEN_11db);	//Short Range IR sensor
	adc1_config_channel_atten(PhotoResistor, ADC_ATTEN_11db);	//PhotoResistor
	adc1_config_channel_atten(WIFI_Trigger, ADC_ATTEN_11db);	//PhotoResistor
}

void GPIO_Init() {
	// initialize SENSOR POWER PIN GPIO
	gpio_pad_select_gpio(SENSOR_POWER_PIN);
	gpio_set_direction(SENSOR_POWER_PIN, GPIO_MODE_OUTPUT);

	// initialize LED POWER PIN GPIO
	gpio_pad_select_gpio(LED_POWER_PIN);
	gpio_set_direction(LED_POWER_PIN, GPIO_MODE_OUTPUT);

}

void adcTask(void *pvParameter)
{
	int IR_Long1 = 0;
	int IR_Long2 = 0;
	int IR_Short = 0;
	int PR = 0;
	int sample_limit=5;
	vTaskDelay(2000/portTICK_PERIOD_MS);

    while(1){

    	if (IR_Power) {

    		IR_Long1 = 0;
    		IR_Long2 = 0;
    		IR_Short = 0;
    		//ESP_LOGI(IR_TAG, "IR_Long: %d\tIR_Short: %d", adc1_get_voltage(IRSensor_Long), adc1_get_voltage(IRSensor_Short));
    		for (int i=0;i<sample_limit;i++) {
    			IR_Long1 = IR_Long1 + adc1_get_voltage(IRSensor_Long1);
    			IR_Long2 = IR_Long2 + adc1_get_voltage(IRSensor_Long2);
    			IR_Short = IR_Short + adc1_get_voltage(IRSensor_Short);
    		}
    		IR_Long1=IR_Long1/sample_limit;
    		IR_Long2=IR_Long2/sample_limit;
    		IR_Short=IR_Short/sample_limit;
    		PR = adc1_get_voltage(PhotoResistor);

    		sprintf(JSON_message, "%s%d%s%d%s%d%s%d%s", "{ \"PR\":\"", PR, "\", \"IR_Long1\":\"", IR_Long1, "\", \"IR_Long2\":\"", IR_Long2, "\", \"IR_Short\":\"", IR_Short, "\" }\r\n");
    		//ESP_LOGI(SYS_TAG, "JSON_message: %s", JSON_message);
    		//ESP_LOGI(IR_TAG, "IR_Long: %d\tIR_Short: %d", IR_Long, IR_Short);

    		if (IR_Long1 >= IR_Long1_Trig || IR_Long2 >= IR_Long2_Trig || IR_Short >= IR_Short_Trig) {
    			ESP_LOGI(IR_TAG, "Sensor(s) Triggered. Turn ON LED for %dms while Task delay", CountDown);

    			gpio_set_level(LED_POWER_PIN, true);
    			vTaskDelay(CountDown/portTICK_PERIOD_MS);
    		}
    		else {
    			gpio_set_level(LED_POWER_PIN, false);
    			vTaskDelay(50/portTICK_PERIOD_MS);
    		}
    	}
    	else {
    		gpio_set_level(LED_POWER_PIN, false);
    		vTaskDelay(300/portTICK_PERIOD_MS);
    	}
    }
    vTaskDelete(NULL);
}

void lightTask(void *pvParameter) {
	while(1) {
		int PR = adc1_get_voltage(PhotoResistor);

		if (PR<=PR_Trig) {
			IR_Power = true;
			gpio_set_level(SENSOR_POWER_PIN, true);
			ESP_LOGI(PR_TAG, "PhotoResistor: [%d], Turn ON IR SENSOR POWER", PR);
			vTaskDelay(60000/portTICK_RATE_MS);
		}
		else {
			IR_Power = false;
			gpio_set_level(SENSOR_POWER_PIN, false);
			ESP_LOGI(PR_TAG, "PhotoResistor: [%d], Turn OFF IR SENSOR POWER", PR);
			vTaskDelay(300/portTICK_RATE_MS);
		}
	}
	vTaskDelete(NULL);
}


void WIFI_task(void* arg) {

	bool WIFI_TRIG = false;
	int WIFI_button = 0;

	while(1) {
		WIFI_button = adc1_get_voltage(WIFI_Trigger);
		if (WIFI_button >=1800 && !WIFI_TRIG) {	//normally should be 4095 when button is triggered
			ESP_LOGI(WIFI_TAG, "WIFI Button ON");
			WIFI_TRIG = true;

			wifi_conn_init();

			if (AP_STA==0) {
				STA_mode();
			}
			else if (AP_STA==1) {
				AP_mode();
			}


			xEventGroupWaitBits(wifi_event_group, AP_STA_COMPLETED_BIT, false, true, portMAX_DELAY);

			esp_ota_init();
			xTaskCreatePinnedToCore(&HTTP_server_task, "HTTP_server_task", 10240, NULL, 4, NULL, 0);

		}
		if (WIFI_TRIG) {
			vTaskDelay(1000/portTICK_PERIOD_MS);
		} else {
			vTaskDelay(200/portTICK_PERIOD_MS);
		}
	}

	vTaskDelete(NULL);
}

void app_main(void) {
	nvs_init();
	parameters_no = sizeof(value_name)/sizeof(value_name[0]);
	nvs_read_batch();

	nvs_write_str("VERSION", VERSION);
	ESP_LOGI(SYS_TAG, "verion: %s", nvs_read_str("VERSION"));
	esp_ota_init();
	ADC_Init();
	GPIO_Init();

	xTaskCreatePinnedToCore(WIFI_task, "WIFI_task", 8192, NULL, 10, NULL, 0);
	xTaskCreatePinnedToCore(lightTask, "lightTask", 2048, NULL, 3, NULL, 1);
	xTaskCreatePinnedToCore(adcTask, "adcTask", 2048, NULL, 4, NULL, 1);
}


#include "nvsFunction.h"
#include "esp_log.h"


void nvs_init(void) {

	// Initialize NVS
	err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
		// NVS partition was truncated and needs to be erased
	    // Retry nvs_flash_init
	    ESP_ERROR_CHECK(nvs_flash_erase());
	    err = nvs_flash_init();
	}
	ESP_ERROR_CHECK( err );

	err = nvs_open("storage", NVS_READWRITE, &my_handle);
	if (err != ESP_OK) {
		ESP_LOGI(NVS_TAG, "Error (%d) opening NVS handle!\n", err);

	}
	else {

		//Write Value
		//for (int i=0; i<4; i++) {
			//nvs_write(key_name[i], value_name[i]);
		//}

		//Read Value, Write default value if cant read
		ESP_LOGI(NVS_TAG, "NVS handle opened");
		//nvs_read_batch();
	}

}

int16_t nvsReadI16(char *key ) {
	//Reading parameters
	int16_t value;
	err = nvs_get_i16(my_handle, key, &value);
	switch (err) {
		case ESP_OK:
			//printf("wifi_ssid = %s\n", value);
			ESP_LOGI(NVS_TAG, "%s = %d", key, value);
			return value;
	        break;
	    case ESP_ERR_NVS_NOT_FOUND:
	        //printf("%s is not set!\n", key);
	        ESP_LOGI(NVS_TAG, "%s is not set", key);
	        return -1;
	        break;
	    default :
	        //printf("Error (%d) reading!\n", err);
	        ESP_LOGI(NVS_TAG, "Error (%d) reading!\n", err);
	        return -1;
	}
}


void nvsWriteI16(char *key, int16_t value ) {
	//Writing parameters
	err = nvs_set_i16(my_handle, key, value);
	if (err != ESP_OK) {
		ESP_LOGI(NVS_TAG, "Set value failed for %s:%d", key, value);
	}
	else {
		ESP_LOGI(NVS_TAG, "Set value success for %s:%d", key, value);
	}

	//Commit update
	err = nvs_commit(my_handle);
	if (err != ESP_OK) {
		ESP_LOGI(NVS_TAG, "Commit update failed for %s:%d", key, value);
	}
	else {
		ESP_LOGI(NVS_TAG, "Commit update success for %s:%d", key, value);
	}
}

void nvs_read_batch() {
	for (int i=0; i<parameters_no; i++) {
		int16_t value = nvsReadI16(key_name[i]);
		if (value<0) {			//if error reading key name
			ESP_LOGI(NVS_TAG, "error opening %s, now write default value", key_name[i]);
			nvsWriteI16(key_name[i], default_value[i]);
		}
		else {
			switch(i) {
				case 0:
					PR_Trig = value;
					break;
				case 1:
					IR_Long1_Trig = value;
					break;
				case 2:
					IR_Long2_Trig = value;
					break;
				case 3:
					IR_Short_Trig = value;
					break;
				case 4:
					CountDown = value;
					break;
			}
		}
	}
}

char *nvs_read_str(char *key) {
	//Reading parameters
	size_t string_size;
	err = nvs_get_str(my_handle, key, NULL, &string_size);
	char *value = malloc(string_size);
	err = nvs_get_str(my_handle, key, value, &string_size);
	switch (err) {
		case ESP_OK:
			//printf("wifi_ssid = %s\n", value);
			ESP_LOGI(NVS_TAG, "%s = %s", key, value);
			return value;
	        break;
	    case ESP_ERR_NVS_NOT_FOUND:
	        ESP_LOGI(NVS_TAG, "%s is not set", key);
	        return NULL;
	        break;
	    default :
	        //printf("Error (%d) reading!\n", err);
	        ESP_LOGI(NVS_TAG, "Error (%d) reading!\n", err);
	        return NULL;
	}
}

void nvs_write_str(char* key, char *value) {
	//Writing parameters
	err = nvs_set_str(my_handle, key, value);
	if (err != ESP_OK) {
		ESP_LOGI(NVS_TAG, "Set value failed for %s:%s", key, value);
	}
	else {
		ESP_LOGI(NVS_TAG, "Set value success for %s:%s", key, value);
	}

	//Commit update
	err = nvs_commit(my_handle);
	if (err != ESP_OK) {
		ESP_LOGI(NVS_TAG, "Commit update failed for %s:%s", key, value);
	}
	else {
		ESP_LOGI(NVS_TAG, "Commit update success for %s:%s", key, value);
	}
}

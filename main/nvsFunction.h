#ifndef NVSFUNCTION_H_
#define NVSFUNCTION_H_

#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#define NVS_TAG "[NVS]"

esp_err_t err;
nvs_handle my_handle;

extern int32_t PR_Trig, IR_Long1_Trig, IR_Long2_Trig, IR_Short_Trig, CountDown;
extern int parameters_no;
extern int default_value[];
extern char *key_name[];

/* nvs init function */
void nvs_init(void);

/* Retrieve the value of key in int32_t format*/
int32_t nvsReadI32(char *key );

/* Write the value of key in int32_t format*/
void nvsWriteI32(char *key, int32_t value );

/* Batch Function to read nvs value to variables, write default value if fail to open nvs */
void nvs_read_batch();

/* Retrieve the value of key in char format */
char *nvs_read_str(char *key);

/* Write the value of key in char format*/
void nvs_write_str(char* key, char *value);

#endif /* NVSFUNCTION_H_ */

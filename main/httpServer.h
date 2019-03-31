/*
 * httpServer.h
 *
 *  Created on: 3 Feb, 2018
 *      Author: Administrator
 */

#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include "esp_system.h"

//HTTP
#define HTTP_TAG "[HTTP]"
#define OTA_TAG "[OTA]"
#define RECV_BUF_SIZE 1024

extern char VERSION[];
extern char *key_name[];
extern char *display_name[];
extern int16_t PR_Trig, IR_Long_Trig, IR_Short_Trig,  CountDown;
extern int parameters_no;
extern char JSON_message[60];

/* Functions*/
void esp_ota_init();
void HTTP_server_task(void *pvParameter);

#endif /* HTTPSERVER_H_ */

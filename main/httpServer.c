#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "httpServer.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_ota_ops.h"

//Socket Programming for HTTP Server
#include <sys/socket.h>
#include <lwip/sockets.h>

//Additional Library
#include "memmem.c"
#include "html.h"
#include "nvsFunction.h"

#include "sdkconfig.h"

int ret;
static char recv_buf[RECV_BUF_SIZE + 1] = { 0 };

//OTA
bool OTA_start = false;
bool packet_end = false;

char *delimitor;

int part_count = 0;

char *recv_buf_1;
char *recv_buf_2;

int content_length_read =0;
int content_length_write =0;

char ota_1st_data[RECV_BUF_SIZE*2] = { 0 };
char ota_write_data[RECV_BUF_SIZE] = { 0 };

esp_ota_handle_t update_handle = 0 ;
const esp_partition_t *update_partition = NULL;

const esp_partition_t *configured;
const esp_partition_t *running;

//General
esp_err_t err;

//HTTP
char server_address[20];
char server_port[] = "80";

void prepareHTML() {

	for (int i=0; i < parameters_no;i++) {
		memset(html_data[i], 0, sizeof(html_data[i]));
	}

	snprintf(html_data[0], sizeof(html_data[0]), "%s%s%s%s%s%d%s", t1, display_name[0], t2, key_name[0], t3, PR_Trig, t_a);
	snprintf(html_data[1], sizeof(html_data[1]), "%s%s%s%s%s%d%s", t1, display_name[1], t2, key_name[1], t3, IR_Long1_Trig, t_b);
	snprintf(html_data[2], sizeof(html_data[2]), "%s%s%s%s%s%d%s", t1, display_name[2], t2, key_name[2], t3, IR_Long2_Trig, t_c);
	snprintf(html_data[3], sizeof(html_data[2]), "%s%s%s%s%s%d%s", t1, display_name[3], t2, key_name[3], t3, IR_Short_Trig, t_d);
	snprintf(html_data[4], sizeof(html_data[3]), "%s%s%s%s%s%d%s", t1, display_name[4], t2, key_name[4], t3, CountDown, t_e);

	sprintf(ota_html, "%s%s%s%s%s", ota_html_top, server_address, ":", server_port, ota_html_bottom);

	//Prepare the header which shows firmware version
	sprintf(index_html_top, "%s%s%s", index_html_top1, VERSION, index_html_top2);
}

static int header_func(char *buffer,  int len) {
	//the 1st /r/n/r/n is the terminator for HTTP header
	//the 2nd /r/n/r/n is the terminator of the multipart/form-data;
	//with these 2 indexes we can locate the boundary text & the location of content
	//The total header thus count from 0->index[1], the size is thus index[1]+1

	//the additional "middle" is used to extract the boundary text

	int index[] ={0,0};
	int middle =0 ;
	int count =0;
	bool middle_found =false;
	for (int i=3; i<len; i++) {
		if (buffer[i]=='\n') {

			if  (buffer[i-1]=='\r' && count==1 && !middle_found) {
				middle = i;
				middle_found=true;
			}

			if  (buffer[i-1]=='\r' && buffer[i-2]=='\n' && buffer[i-3]=='\r') {
				index[count]=i;
				count++;
	        }
			if (count>1) {
				break;
			}
	    }
	}

	int start = index[0]+1;
	int end = middle-2;
	int size= end-start+1;
	char delimit[size+1];
	for (int j=start; j<=end; j++) {
		delimit[j-start]=buffer[j];
	}
	delimit[size]='\0';

	delimitor = (char *)malloc(size+1);
	strcpy(delimitor, (char*)delimit);
	//ESP_LOGI(OTA_TAG, "index[0]: %d", index[0]);
	//ESP_LOGI(OTA_TAG, "index[1]: %d", index[1]);
	//ESP_LOGI(OTA_TAG, "middle: %d", middle);
	ESP_LOGI(OTA_TAG, "delimitor: %s, [%d]", delimitor, size);

	return index[1];
}

static void print_ASCII(char *buffer, int len) {
	for (int i=0;i<len; i++) {
		printf("<%d> %c [%d]\n", i, buffer[i], buffer[i]);
	}
}

void esp_ota_init() {
	configured = esp_ota_get_boot_partition();
	running = esp_ota_get_running_partition();

	if (configured != running) {
		ESP_LOGW(OTA_TAG, "Configured OTA boot partition at offset 0x%08x, but running from offset 0x%08x", configured->address, running->address);
		ESP_LOGW(OTA_TAG, "(This can happen if either the OTA boot data or preferred boot image become corrupted somehow.)");
	}
	ESP_LOGI(OTA_TAG, "Running partition type %d subtype %d (offset 0x%08x)", running->type, running->subtype, running->address);

	update_partition = esp_ota_get_next_update_partition(NULL);
	ESP_LOGI(OTA_TAG, "Writing to partition subtype %d at offset 0x%x", update_partition->subtype, update_partition->address);
	assert(update_partition != NULL);

	err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);
	if (err != ESP_OK) {
		ESP_LOGE(OTA_TAG, "esp_ota_begin failed, error=%d", err);
	}
	ESP_LOGI(OTA_TAG, "esp_ota_begin succeeded");
}

static void espWriteData(char buffer[], int len) {
	err = esp_ota_write( update_handle, (const void *)buffer, len);
	if (err != ESP_OK) {
		ESP_LOGE(OTA_TAG, "Error: esp_ota_write failed! err=0x%x", err);

	} else {
	ESP_LOGI(OTA_TAG, "esp_ota_write %d", len);
		content_length_write += len;
	}
}

void HTTP_server_task(void *pvParameter) {

	prepareHTML();

	//create socket
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	//define address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(atoi(server_port));
	server_address.sin_addr.s_addr = INADDR_ANY;

	bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

	listen(server_socket, 5);
	ESP_LOGI(OTA_TAG, "HTTP Server started");

	int client_socket;
	while (1) {
		packet_end = false;
		OTA_start = false;
		client_socket = accept(server_socket, NULL, NULL);

		while (!packet_end) {
			memset(recv_buf, 0, RECV_BUF_SIZE); //clear the recv_buf first
			ret = recv(client_socket, recv_buf, RECV_BUF_SIZE, 0);

			if (ret <0) {
				break;
			}
			//else {
				//printf("%s\n", recv_buf);
			//}

			if (OTA_start) {
				// A precheck of ending "\r\n", it seems there is problem of just depending on memmem() to check for
				// the boundary. Problem occur when the fucntion is called many times in the loop.
				part_count++;

				if (recv_buf[ret-1]=='\n' && recv_buf[ret-2]=='\r') {
					char *pos =memmem(recv_buf, ret, delimitor, sizeof(delimitor)-1);
					if (pos != NULL) {	//if it contain the boundary, split the content-footer
						content_length_read = content_length_read + ret-(strlen(pos)+2);
						ESP_LOGI(OTA_TAG, "received OTA part %d(last) [content+footer=recv]: [%d+%d=%d], total: %d", part_count, ret-(strlen(pos)+2), (strlen(pos)+2), ret, content_length_read);

						memset(ota_write_data, 0, RECV_BUF_SIZE);
						memcpy(ota_write_data, &recv_buf, ret-(strlen(pos)+2));
						espWriteData(ota_write_data, ret-(strlen(pos)+2));

						//print_ASCII(recv_buf, ret);

						//reply to the client OTA completed

						packet_end = true;
						//send(client_socket, http_message, strlen(http_message), 0);
						send(client_socket, http_header, strlen(http_header), 0);
						send(client_socket, index_html_top, strlen(index_html_top), 0);
						send(client_socket, ota_feedback_html, strlen(ota_feedback_html), 0);
						send(client_socket, index_html_down, strlen(index_html_down), 0);
						close(client_socket);


						if (esp_ota_end(update_handle) != ESP_OK) {
							ESP_LOGE(OTA_TAG, "esp_ota_end failed!");
							//task_fatal_error();
						}
						else {
							ESP_LOGI(OTA_TAG, "Total Data written %d", content_length_write);
						}
						err = esp_ota_set_boot_partition(update_partition);
						if (err != ESP_OK) {
							ESP_LOGE(OTA_TAG, "esp_ota_set_boot_partition failed! err=0x%x", err);
							//task_fatal_error();
						}
						ESP_LOGI(OTA_TAG, "Prepare to restart system!");
						esp_restart();

						//break;
					}

					else {
						//if it didnt contain a boundary, the whole part is the content
						content_length_read=content_length_read + ret;
						ESP_LOGI(OTA_TAG, "received OTA part %d [recv]:[%d], total: %d", part_count, ret, content_length_read);

						memset(ota_write_data, 0, RECV_BUF_SIZE);
						memcpy(ota_write_data, &recv_buf, ret);
						espWriteData(ota_write_data, ret);
					}
				}
				else {
					//if it didnt contain a boundary, the whole part is the content
					content_length_read=content_length_read + ret;
					ESP_LOGI(OTA_TAG, "received OTA part %d [recv]:[%d], total: %d", part_count, ret, content_length_read);

					memset(ota_write_data, 0, RECV_BUF_SIZE);
					memcpy(ota_write_data, &recv_buf, ret);
					espWriteData(ota_write_data, ret);
				 }
			}

			else {
				if (strstr(recv_buf, "GET ")) {
					if (strstr(recv_buf, "GET /sensorReading_JSON HTTP/1.1") || strstr(recv_buf, "GET /sensorreading_json HTTP/1.1")) {
						send(client_socket, json_header, sizeof(json_header)-1, 0);
						//The JSON_message is a char array of fixed size 100
						//empty space at the end will cause problem for JSON parsing in browser
						//use strlen() instead of sizeof() to determine the length of char array which contain empty space
						//sizeof() shoulsd return the same result as strlen if the char array is const
						send(client_socket, JSON_message, strlen(JSON_message), 0);
						close(client_socket);
					}
					if (strstr(recv_buf, "GET / HTTP/1.1") || strstr(recv_buf, "GET /index.html HTTP/1.1") || strstr(recv_buf, "GET /index.htm HTTP/1.1")) {
						send(client_socket, http_header, strlen(http_header), 0);
						send(client_socket, index_html_top, strlen(index_html_top), 0);
						send(client_socket, index_html, strlen(index_html), 0);
						send(client_socket, index_html_down, strlen(index_html_down), 0);
						close(client_socket);
					}
					if (strstr(recv_buf, "GET /SETUP HTTP/1.1") || strstr(recv_buf, "GET /setup HTTP/1.1") || strstr(recv_buf, "GET /setup.html HTTP/1.1")) {

						send(client_socket, http_header, sizeof(http_header)-1, 0);
						send(client_socket, index_html_top, strlen(index_html_top)-1, 0);
						send(client_socket, setting_html_top, sizeof(setting_html_top)-1, 0);
						for (int i=0;i<parameters_no;i++) {
							send(client_socket, html_data[i], strlen(html_data[i]), 0);
						}
						send(client_socket, setting_html_down, sizeof(setting_html_down)-1, 0);
						send(client_socket, index_html_down, sizeof(index_html_down)-1, 0);
						close(client_socket);
					}
					if (strstr(recv_buf, "GET /OTA") || strstr(recv_buf, "GET /ota")) {
						//ESP_LOGI(SSL_TAG, "GET OTA request. sending ota_html");
						send(client_socket, http_header, sizeof(http_header)-1, 0);
						send(client_socket, index_html_top, strlen(index_html_top)-1, 0);
						send(client_socket, ota_html, strlen(ota_html), 0);
						send(client_socket, index_html_down, sizeof(index_html_down)-1, 0);
						close(client_socket);
					}

					if (strstr(recv_buf, "GET /favicon.ico")) {
						send(client_socket, http_nf, sizeof(http_nf)-1, 0);
						close(client_socket);
					}

				}
				if (strstr(recv_buf, "POST")) {
					if ( strstr(recv_buf, "POST / HTTP/1.1") ) {
						//** Handling the 1st and 2nd receive messages ***//
						//Firefox and MS Edge will make use of all buffer size for transmission
						//While Chrome will separate the HTTP header in the first transmission and transmit the remaining
						//To cope with the difference we combine the first 2 transmissions

						//make a copy of 1st received buffer
						char part1[ret+1];
						int part1_len = ret;
						memcpy(part1, recv_buf, ret);

						//read the 2nd buffer from client
						memset(recv_buf, 0, RECV_BUF_SIZE);
						ret = recv(client_socket, recv_buf, RECV_BUF_SIZE, 0);

						//print_ASCII(part1, part1_len);
						//print_ASCII(recv_buf, ret);

						//join the 2 buffers
						char joined_buf[part1_len+ret+1];
						int joined_buf_len=part1_len+ret;

						memcpy(joined_buf, part1, part1_len);
						memcpy(&joined_buf[part1_len], recv_buf, ret);
						//print_ASCII(joined_buf, part1_len+ret);


						//send the joined_buf for header-content split-up
						int boundary = header_func(joined_buf, joined_buf_len);
						content_length_read=content_length_read + (joined_buf_len)-(boundary+1);
						part_count++;

						ESP_LOGI(OTA_TAG, "received OTA part %d [header+content=recv]:[%d+%d=%d], total: %d", part_count, (boundary+1), (joined_buf_len)-(boundary+1), (joined_buf_len), content_length_read);
						OTA_start=true;

						//prepare the data fragment
						memset(ota_1st_data, 0, RECV_BUF_SIZE*2);	// we joined the buffer so we double the size
						/*copy first http packet body to write buffer*/
						memcpy(ota_1st_data, &(joined_buf[boundary+1]), joined_buf_len-(boundary+1));
						espWriteData(ota_1st_data, joined_buf_len-(boundary+1));
					}

					else if (strstr(recv_buf, "POST /SETUP HTTP/1.1") || strstr(recv_buf, "POST /setup HTTP/1.1")) {

						//Parse POST data
						char message [1500];
						char temp [200];
						char temp2 [parameters_no][50];
						char data [parameters_no][2][25];
						int i =0, j=0;

						strcpy(message, recv_buf);
						char *ptr = strtok(message, "\n"); //parse at \n
						while(ptr) { //while token was found
							strcpy(temp, ptr);
							ptr = strtok(NULL, "\n");
						}
						//ESP_LOGI(HTTP_TAG, "Post Message 1st break: %s", temp);
						ESP_LOGI(HTTP_TAG, "Post Message: %s", temp);

						ptr = strtok(temp, "&"); //parse at &
						while(ptr) { //while token was found
							//printf(">> %s, %d\n", ptr, i);
							//ESP_LOGI(HTTP_TAG, "Post Message 2nd break[%d]: %s", i, ptr);
							strcpy(temp2[i], ptr);
							ptr = strtok(NULL, "&");
							i++;
						}

						for (i=0;i<parameters_no;i++) {
							j=0;
							ptr = strtok(temp2[i], "="); //parse at =
							while(ptr) { //while token was found
								//printf(">>> %s, %d, %d\n", ptr, i, j);
								//ESP_LOGI(HTTP_TAG, "Post Message 3rd break[%d,%d]: %s", i, j, ptr);
								strcpy(data[i][j], ptr);
								ptr = strtok(NULL, "=");
								j++;
							}
						}

						//Update setting
						for (int i=0; i<parameters_no; i++) {
							nvsWriteI32(data[i][0], (int32_t)atoi(data[i][1]));
						}
						nvs_read_batch();//read the value into variable
						prepareHTML();
						ESP_LOGI(HTTP_TAG, "Setting Update done");
						send(client_socket, json_header, sizeof(json_header)-1, 0);
						send(client_socket, JSON_message2, strlen(JSON_message2), 0);
						close(client_socket);
					}
				}
				else {
					send(client_socket, http_nf, strlen(http_nf), 0);
					close(client_socket);
				}
			}
		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}

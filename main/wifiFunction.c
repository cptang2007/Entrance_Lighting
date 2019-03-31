#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "wifiFunction.h"
#include "sdkconfig.h"

//AP
//char AP_IP[] = "192.168.10.1";

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {

    case SYSTEM_EVENT_AP_START:
		printf("Access point started\n");
		break;

	case SYSTEM_EVENT_AP_STACONNECTED:
		xEventGroupSetBits(wifi_event_group, STA_CONNECTED_BIT);
		break;

	case SYSTEM_EVENT_AP_STADISCONNECTED:
		xEventGroupSetBits(wifi_event_group, STA_DISCONNECTED_BIT);
		break;

	case SYSTEM_EVENT_STA_START:
	    esp_wifi_connect();
	    break;

	case SYSTEM_EVENT_STA_GOT_IP:
	    xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
	    break;


	case SYSTEM_EVENT_STA_DISCONNECTED:
		/* This is a workaround as ESP32 WiFi libs don't currently
	   auto-reassociate. */
		esp_wifi_connect();
		xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
		break;

	default:
		break;
	}

	return ESP_OK;
}

void wifi_conn_init() {

	// disable the default wifi logging
	//esp_log_level_set("wifi", ESP_LOG_NONE);

	// initialize the tcp stack
	tcpip_adapter_init();

	// create the event group to handle wifi events
	wifi_event_group = xEventGroupCreate();

	// initialize the wifi event handler
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
}

void AP_mode() {

	ESP_LOGI(WIFI_TAG, "Starting Access Point, SSID=%s, IP=%s", CONFIG_AP_SSID, AP_IP);

	//resolve IP address
	int IP[4];
	int i=0;
	char temp[strlen(AP_IP)+1];
	memcpy(temp, AP_IP, strlen(AP_IP));
	char *ptr = strtok(temp, "."); //parse at \n
	while(ptr) { //while token was found
		//ESP_LOGI(WIFI_TAG, "%s", ptr);
		IP[i]=atoi(ptr);
		ptr = strtok(NULL, ".");
		i++;
	}

	// stop DHCP server
	ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));

	// assign a static IP to the network interface
	tcpip_adapter_ip_info_t ip_info;
	memset(&ip_info, 0, sizeof(ip_info));
	IP4_ADDR(&ip_info.ip, IP[0], IP[1], IP[2], IP[3]);
	IP4_ADDR(&ip_info.gw, IP[0], IP[1], IP[2], IP[3]);
	IP4_ADDR(&ip_info.netmask, 255, 255, 255, 0);
	ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &ip_info));

	// start the DHCP server
	ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));


	// initialize the wifi stack in AccessPoint mode with config in RAM
	wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

	// Switch to AP Mode
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

	// configure the wifi connection and start the interface
	wifi_config_t ap_config = {
			.ap = {
					.ssid=				CONFIG_AP_SSID,
					.ssid_len=	 		strlen(CONFIG_AP_SSID),
					.password=			CONFIG_AP_PASSWORD,
					.channel=			CONFIG_AP_CHANNEL,
					.authmode=			CONFIG_AP_AUTHMODE,
					.ssid_hidden=		CONFIG_AP_SSID_HIDDEN,
					.max_connection=	CONFIG_AP_MAX_CONNECTIONS,
					.beacon_interval=	CONFIG_AP_BEACON_INTERVAL
			},
	};
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));

	// start the wifi interface
	ESP_ERROR_CHECK(esp_wifi_start());


	memset(server_address, 0, strlen(server_address));
	memcpy(server_address, AP_IP, strlen(AP_IP));
	//set AP_STA_COMPLETED_BIT to allow server tasks started
	xEventGroupSetBits(wifi_event_group, AP_STA_COMPLETED_BIT);
}

void STA_mode() {
	// initialize the wifi stack in STAtion mode with config in RAM
	wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

	// configure the wifi connection and start the interface
	wifi_config_t wifi_config = {
		.sta = {
				.ssid = WIFI_SSID,
				.password = WIFI_PASS,
	    },
	};
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_LOGI(WIFI_TAG, "Connecting to %s\n", WIFI_SSID);


	//Wait for getting IP address
	xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
	// print the local IP address
	tcpip_adapter_ip_info_t ip_info;
	ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info));
	ESP_LOGI(WIFI_TAG, "IP Address:  %s", ip4addr_ntoa(&ip_info.ip));
	ESP_LOGI(WIFI_TAG, "Subnet mask: %s", ip4addr_ntoa(&ip_info.netmask));
	ESP_LOGI(WIFI_TAG, "Gateway:     %s", ip4addr_ntoa(&ip_info.gw));

	memset(server_address, 0, strlen(server_address));
	memcpy(server_address, ip4addr_ntoa(&ip_info.ip), strlen(ip4addr_ntoa(&ip_info.ip)));

	//set AP_STA_COMPLETED_BIT to allow server tasks started
	xEventGroupSetBits(wifi_event_group, AP_STA_COMPLETED_BIT);
}

#ifndef WIFIFUNCTION_H_
#define WIFIFUNCTION_H_

#define WIFI_TAG 					"[WIFI]"
#define CONNECTED_BIT				BIT0
#define STA_CONNECTED_BIT 			BIT1
#define STA_DISCONNECTED_BIT 		BIT2
#define AP_STA_COMPLETED_BIT 		BIT3

//WiFi
#define AP_STA						1	//1 for AP mode, 0 for STA Mode

//AP CONFIG
#define CONFIG_AP_SSID 				"ESP32_Entrance_Lighting"
#define CONFIG_AP_PASSWORD 			"11111111"					//Password need 8 digits
#define CONFIG_AP_CHANNEL 			8
#define CONFIG_AP_AUTHMODE 			WIFI_AUTH_WPA2_PSK
#define CONFIG_AP_MAX_CONNECTIONS 	1
#define CONFIG_AP_BEACON_INTERVAL 	100
#define CONFIG_AP_SSID_HIDDEN 		0
#define AP_IP 						"192.168.10.1"

//STA CONFIG
#define WIFI_SSID 					"My_Router"
#define WIFI_PASS 					"password"

extern EventGroupHandle_t wifi_event_group;
extern char server_address[20];


/* Functions */
void wifi_conn_init();
void AP_mode();
void STA_mode();

#endif /* WIFIFUNCTION_H_ */

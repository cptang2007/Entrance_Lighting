# Automatic Entrance Lighting

This is the firmware for the Instructable [Automatic Entrance Lighting](https://www.instructables.com/member/CPTang/instructables/drafts/).  

## Features
- OTA firmware update via browser upload.
- Parameters configuration via a web browser:
  * Infra-Red Sensors Detection Ranges.
  * Light-On Duration.
  * Light Intensity Trigger Level.
   
## Requirement
 * ESP32 Development Board, in this project I use **NodeMCU-32s**. You may need to change the PCB layout if other development board is used. 
 * **[ESP-IDF v2.1](https://github.com/espressif/esp-idf/tree/release/v2.1)** - I use an older version of Espressif IoT Development Framework. You probably need to fix the codes if you use a newer version.   

## Pins Assignment 
 * IR Sensor Long_Range 1 	(ADC1_CHANNEL_0)
 * IR Sensor Long_Range 2 	(ADC1_CHANNEL_3)
 * IR Sensor Short_Range 	(ADC1_CHANNEL_6)
 * PhotoResistor 			(ADC1_CHANNEL_7)
 * WIFI_Trigger 			(ADC1_CHANNEL_4)
 * Sensor Power Pin 		GPIO_NUM_22
 * LED Lighting Power Pin	GPIO_NUM_21
 
 ![Reference](https://www.taiwaniot.com.tw/wp-content/uploads/2017/06/TB2416uotFopuFjSZFHXXbSlXXa_2922621297.png)
 
## How to use
  1. Setup ESP-IDF.
  2. The MCU will boot in **SoftAP** mode by default, make the following changes if you want to boot in **STA mode** (e.g. connect to your wireless router):
     * Change **AP_STA** to **0** in **wifiFunction.h**
     * Modify the WiFi credentials **WIFI_SSID** & **WIFI_PASS** in **wifiFunction.h**.
  3. Save the changes, Build and Flash the Firmware.
  4. Install the development board to the PCB.
  5. Make sure the WiFi toggle switch is **ON before Power up**.  
  6. Connect to the MCU
     * SoftAP mode - SSID: ESP32_Entrance_Lighting, Password: 11111111, go to http://192.168.10.1
     * STA mode - go to the IP address of the MCU. You may need to check it in your wireless router administration page.
  7. Change the Parameters.
  8. Turn off the WiFi toggle button and restart the system, if all settings are done and You don't need WiFi setting anymore.      

## Note
  1. In SoftAP mode, the connection will be unstable if the channel is already occupied by other 2.4GHz WiFi devices. Use another channel by modifying the value of **CONFIG_AP_CHANNEL** in **wifiFunction.h**. The default is channel 8. 
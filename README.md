# Automatic Entrance Lighting

This is the firmware for the Instructable [Automatic Entrance Lighting](https://www.instructables.com/member/CPTang/instructables/drafts/).  

## Features
- OTA firmware update via browser upload.
- Paratmeters configuration via web browser:
  * Infra-Red Sesnors Detection Ranges.
  * Light-On Duration.
  * Light Intensity Trigger Level.
   
## Requirement
 * ESP32 Development Board, in this project I use **NodeMCU-32s**. You may need to change the PCB layout if you use other development board or chips 
 * **[ESP-IDF v2.1](https://github.com/espressif/esp-idf/tree/release/v2.1)** - I use an older version of Espressif IoT Development Framework. You probably need to fix the codes if you use a newer version.   

## Pins Used 
 * IR Sensor Long_Range 1 	(ADC1_CHANNEL_0)
 * IR Sensor Long_Range 2 	(ADC1_CHANNEL_3)
 * IR Sensor Short_Range 	(ADC1_CHANNEL_6)
 * PhotoResistor 			(ADC1_CHANNEL_7)
 * WIFI_Trigger 			(ADC1_CHANNEL_4)
 * Sensor Power Pin 		GPIO_NUM_22
 * LED Lighting Power Pin	GPIO_NUM_21
 
 ![Reference](https://wiki.ai-thinker.com/_media/esp32/boards/nodemcu/nodemcu_32s_pin.png)
 
 ## How to use
  1. Setup ESP-IDF.
  2. Flash the Firmware.
  3. Install the development board to the PCB.
  4. Make sure the WiFi toggle switch is ON. Power on the PCB. **WiFi cannot be switched on AFTER Power On**.  
  5. Connect to the MCU via browser - http://192.168.10.1 in case AP mode, or goto its IP address in case STA mode.
  6. Config the Parameters.    

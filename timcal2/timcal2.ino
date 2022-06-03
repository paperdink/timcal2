/* The MIT License (MIT)
 * Copyright (c) 2021 Rohit Gujarathi

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "FS.h"
#include "SPIFFS.h"
#include "SD.h"
#include "SPI.h"
#include <sys/time.h>
#include "WiFi.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "driver/adc.h"
#include <esp_wifi.h>
#include <esp_bt.h>
#include <DNSServer.h>
#include <WebServer.h>

// #########  Configuration ##########
#include "config.h"
// ###################################

#include "PCF8574.h"
#include "GUI.h"
#include "date_time.h"

// E-paper
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, GxEPD2_DRIVER_CLASS::HEIGHT> display(GxEPD2_DRIVER_CLASS(/*CS=*/ 22, /*DC=*/ 15, /*RST=*/ 13, /*BUSY=*/ 34));

// PCF8574 GPIO extender
PCF8574 pcf8574(PCF_I2C_ADDR, SDA, SCL);

const char* weather = NULL;
char buf[2048];
uint8_t wifi_update = 0;

RTC_DATA_ATTR uint8_t wifi_connected = 0; // keep track if wifi was connected and according update the symbol
RTC_DATA_ATTR uint8_t config_done = 0; // keep track of config done
RTC_DATA_ATTR uint8_t first_boot = 1; // check if it is first boot

esp_sleep_wakeup_cause_t wakeup_reason;

void setup(void)
{
	Serial.begin(115200);
	DEBUG.println();
	DEBUG.println("paperd.ink");

	pinMode(EPD_EN, OUTPUT);
	pinMode(EPD_RES, OUTPUT);
	pinMode(SD_EN, OUTPUT);
	pinMode(BATT_EN, OUTPUT);
	pinMode(PCF_INT, INPUT);
	//pcf8574.begin();

	// Power up EPD
	digitalWrite(EPD_EN, LOW);
	digitalWrite(EPD_RES, LOW);
	delay(50);
	digitalWrite(EPD_RES, HIGH);
	delay(50);
	display.init(115200); // enable diagnostic output on Serial
	//display.init();

	DEBUG.print("Initializing SPIFFS...");
	if(!SPIFFS.begin(true)){
		DEBUG.println("failed!");
		//return;
	}else{
		DEBUG.println("OK!");
	}
	delay(100);
 
	wifi_update = (now.hour % UPDATE_HOUR_INTERVAL == 0 && now.min % UPDATE_MIN_INTERVAL == 0);

	// clear the display
	display.fillScreen(GxEPD_WHITE);
	display.setRotation(0);

	if(wifi_update || first_boot == 1){
		// All WiFi related activity once every UPDATE_HOUR_INTERVAL hours
		DEBUG.println("Connecting to WiFi...");
		if(connect_wifi() != 0){
			DEBUG.println("Can't connect to WiFi");
			wifi_connected = 0;
		}else{
			wifi_connected = 1;
			// Fetch tasks
			fetch_todo();
			weather = fetch_weather();
			// Sync time
			//set_time(); //set time to a predefined value
      configTime(0, 0, "pool.ntp.org");
      if(get_date_dtls(TIME_ZONE) < 0){
        configTime(0, 0, "pool.ntp.org");
      }
		}
	}

	// Battery status calculation
	uint8_t not_charging = digitalRead(CHARGING_PIN);
	pcf8574.digitalWrite(BATT_EN, LOW);
  adc_power_on();
	delay(10);
  adc1_config_width(ADC_WIDTH_BIT_12);
  //adc1_config_channel_atten(BATTERY_VOLTAGE_ADC,ADC_ATTEN_DB_0); // Gives timeout on EPD
  int batt_adc = adc1_get_raw(BATTERY_VOLTAGE_ADC);
	adc_power_off();
	pcf8574.digitalWrite(BATT_EN, HIGH);
	float batt_voltage = (float)((batt_adc/4095.0)*4.2);

	display_battery(&display,batt_voltage, not_charging);
	display_wifi(&display,wifi_connected);
	display_weather(&display,weather);
	display_background(&display);
  display_tasks(&display);
  // Get the date details before any date/time related tasks
  get_date_dtls(TIME_ZONE);
	display_calender(&display);
	display_time(&display); 

  // Prepare to go to sleep
  if(wifi_update || first_boot == 1){
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    btStop();
    if(wifi_connected){
      // if check is not there then device crashes
      esp_wifi_stop();
    }
    esp_bt_controller_disable();
  }

  display.display(false);
  delay(1000);

	DEBUG.println("Turning off everything");
	digitalWrite(SD_EN, HIGH);
	digitalWrite(BATT_EN, HIGH);
	// Powerdown EPD
	display.hibernate(); // Dont use this if you require partial updates
	digitalWrite(EPD_EN, HIGH);
	digitalWrite(EPD_RES, LOW);
	delay(50);
	digitalWrite(EPD_RES, HIGH);
	first_boot = 0;
	get_date_dtls(TIME_ZONE);
	esp_sleep_enable_timer_wakeup((60-now.sec) * uS_TO_S_FACTOR);
	DEBUG.printf("Going to sleep...");
	// Go to sleep
	esp_deep_sleep_start();
}

void loop()
{

}

int8_t connect_wifi(){
 uint8_t connAttempts = 0;
 WiFi.begin(SSID, PASSWORD);
 while (WiFi.status() != WL_CONNECTED ) {
   delay(500);
   DEBUG.print(".");
   if(connAttempts > 40){
    return -1;
   }
   connAttempts++;
 }
 DEBUG.println("Connected");
 return 0;
}

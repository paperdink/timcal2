# TimCal 2
Timcal example without wifi AP based configuration.

![Full Image](timcal_image.jpg)

## Install Library Dependencies (from arduino library manager)
  - Adafruit GFX library by Adafruit (tested with v1.7.5)
  - GxEPD2 by Jean-Marc Zingg version (tested with v1.3.9)
  - JSON Streaming Parser by Daniel Eichhorn (tested with v1.0.5)

## Usage
### Step 1:
Use [ESP32 Sketch data upload](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/) to upload the icons from data folder.
### Step 2:
Update the config.h file with configuration details
```
#define SSID     "*****" // your network SSID (name of wifi network)
#define PASSWORD "*****" // your network password
#define TODOIST_TOKEN "*******" // your todoist API key without the word "Bearer"
#define CITY "*****" // your city for weather
#define COUNTRY "*****" // your country for weather
#define OWM_ID "*****" // your open weather map APP ID
#define TIME_ZONE "PST8PDT,M3.2.0,M11.1.0" // your time zone from list https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
```
### Step 3:
Select `ESP32 Dev module` in arduino `Tools>Board` and hit upload.

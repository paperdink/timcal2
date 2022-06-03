
#ifndef GUI_H
#define GUI_H

#define GxEPD2_DISPLAY_CLASS GxEPD2_BW
#define GxEPD2_DRIVER_CLASS  GxEPD2_420

#include <GxEPD2_BW.h>

#include "FS.h"
#include "SPIFFS.h"
#include "SD.h"

#define TIME_BASE_X 25
#define TIME_BASE_Y 60

void display_tasks(GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, GxEPD2_DRIVER_CLASS::HEIGHT>* display);
void display_weather(GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, GxEPD2_DRIVER_CLASS::HEIGHT>* display, const char* icon);
void display_calender(GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, GxEPD2_DRIVER_CLASS::HEIGHT>* display);
void display_time(GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, GxEPD2_DRIVER_CLASS::HEIGHT>* display);
void display_battery(GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, GxEPD2_DRIVER_CLASS::HEIGHT>* display, float batt_voltage, uint8_t not_charging);
void display_wifi(GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, GxEPD2_DRIVER_CLASS::HEIGHT>* display, uint8_t status);
void display_background(GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, GxEPD2_DRIVER_CLASS::HEIGHT>* display);
void display_update(GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, GxEPD2_DRIVER_CLASS::HEIGHT>* display);

int8_t fetch_todo();
const char* fetch_weather();

void drawBitmapFrom_SD_ToBuffer(GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, GxEPD2_DRIVER_CLASS::HEIGHT>* display, fs::FS &fs, const char *filename, int16_t x, int16_t y, bool with_color);

#endif /* GUI_H */

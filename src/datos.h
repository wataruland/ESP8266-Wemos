#ifndef DATOS_H
#define DATOS_H

/***************************
 * Begin Settings
 **************************/
// Please read http://blog.squix.org/weatherstation-getting-code-adapting-it
// for setup instructions

#define HOSTNAME "ESP8266-OTA-"
#define DHTTYPE DHT22
const char* WIFI_SSID = "SSID";
const char* WIFI_PWD  = "PWD";

// Setup
const int UPDATE_INTERVAL_SECS = 10 * 60; // Update every 10 minutes

// Display Settings
const int I2C_DISPLAY_ADDRESS = 0x3c;
const int SDA_PIN             = D2; //04
const int SDC_PIN             = D3; //00
const int NEO_PIN             = D4; //02
const int DHT_PIN             = D5; //14
const int LUZ_PIN             = A0; //00

// TimeClient settings
const float UTC_OFFSET = 2;

// Wunderground Settings
const boolean IS_METRIC = true;
const String WUNDERGRROUND_API_KEY  = "KEY";
const String WUNDERGRROUND_LANGUAGE = "SP";
const String WUNDERGROUND_COUNTRY   = "ES";
const String WUNDERGROUND_CITY      = "Bilbao_Airport";

//Thingspeak Settings
const String THINGSPEAK_CHANNEL_ID   = "ID";
const String THINGSPEAK_API_READ_KEY = "KEY";

// flag changed in the ticker function every 10 minutes
bool readyForWeatherUpdate = false;

String lastUpdate = "--";

#define       Tiempo_Dormir  50
const     int NOCHE           = 50;
unsigned long OSCURIDAD_timer = 0; // timer for esp8266 shutdown

/***************************
 * End Settings
 **************************/

#endif

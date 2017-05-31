/**The MIT License (MIT)
Copyright (c) 2015 by Daniel Eichhorn
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
See more at http://blog.squix.ch
*/

// Setup
const int UPDATE_INTERVAL_SECS = 10 * 60; // Update every 10 minutes
const int UPDATE_PANTALLA_SECS = 20;      // Cambia la pantalla cada 20 seg.

// Pins for the ILI9341
#define TFT_DC D2
#define TFT_CS D1
#define LED_PIN D8

// WLAN
#define HOSTNAME "ESP8266-OTA-"
const char* WIFI_SSID = "";
const char* WIFI_PWD  = "";

// TimeClient settings
const float UTC_OFFSET = 2;

// Wunderground Settings
const boolean IS_METRIC             = true;
const String WUNDERGRROUND_API_KEY  = "key";
const String WUNDERGRROUND_LANGUAGE = "ES";
const String WUNDERGROUND_COUNTRY   = "ES";
const String WUNDERGROUND_CITY      = "Bilbao_Airport";

//Thingspeak Settings
const String THINGSPEAK_CHANNEL_ID   = "ID";
const String THINGSPEAK_API_READ_KEY = "key";

// List, so that the downloader knows what to fetch
String wundergroundIcons [] = {"chanceflurries","chancerain","chancesleet","chancesnow","clear","cloudy","flurries","fog","hazy","mostlycloudy","mostlysunny","partlycloudy","partlysunny","rain","sleet","snow","sunny","tstorms","unknown"};

// Sensores
#define DHTTYPE DHT22
const int NEO_PIN = D4; //02
const int DHT_PIN = D5; //14
const int LUZ_PIN = A0; //00

// Reposo
#define       Tiempo_Activo  60    // 1 minuto
#define       Tiempo_Dormir  300   // 5 minutos
const     int NOCHE           = 50;
unsigned long OSCURIDAD_timer = 0; // timer for esp8266 shutdown
unsigned long delay_timer     = 0; // timer for esp8266 shutdown


/***************************
 * End Settings
 **************************/

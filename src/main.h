#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include "ILI9341.h"         // Hardware-specific library
#include <SPI.h>
// Additional UI functions
#include "GfxUi.h"

// Fonts created by http://oleddisplay.squix.ch/
#include "ArialRoundedMTBold_14.h"
#include "ArialRoundedMTBold_36.h"

// Download helper
#include "WebResource.h"

#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

// Helps with connecting to internet
#include <WiFiManager.h>

// check settings.h for adapting to your needs
#include "settings.h"
#include <JsonListener.h>
#include <WundergroundClient.h>
#include "TimeClient.h"


//declaring prototypes
void configModeCallback (WiFiManager *myWiFiManager);
void downloadCallback(String filename, int16_t bytesDownloaded, int16_t bytesTotal);
ProgressCallback _downloadCallback = downloadCallback;
void downloadResources();
void updateData();
void updatePantalla();
void drawProgress(uint8_t percentage, String text);
void drawTime();
void drawCurrentWeather();
void drawForecast();
void drawForecastSensores();
void drawForecastDetail(uint16_t x, uint16_t y, uint8_t dayIndex);
void drawForecastDetailSensores(uint16_t, uint16_t, uint8_t);
void drawDormir();
void drawScrollBorrar();
String getMeteoconIcon(String iconText);
void drawAstronomy();
void drawSeparator(uint16_t y);

#endif

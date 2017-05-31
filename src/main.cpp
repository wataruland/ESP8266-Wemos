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

#include "main.h"
/*****************************
 * Important: see settings.h to configure your settings!!!
 * ***************************/
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
GfxUi ui = GfxUi(&tft);

WebResource webResource;
TimeClient timeClient(UTC_OFFSET);

// Set to false, if you prefere imperial/inches, Fahrenheit
WundergroundClient wunderground(IS_METRIC);

long lastDownloadUpdate   = millis();
long lastDrew             = 0;
long lastDownloadPantalla = millis();
int  Pantalla             = 0;
//------------------------------------- -------------------------------------//
void setup()
{
  Serial.begin(115200);

  // The LED pin needs to set HIGH
  // Use this pin to save energy
  pinMode(LED_PIN, OUTPUT);
  //digitalWrite(LED_PIN, LOW);
  digitalWrite(LED_PIN, HIGH);

  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&ArialRoundedMTBold_14);
  ui.setTextColor(ILI9341_ORANGE, ILI9341_BLACK);
  ui.setTextAlignment(CENTER);
  ui.drawString(120, 160, "Connecting to WiFi");

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  // Uncomment for testing wifi manager
  //wifiManager.resetSettings();
  wifiManager.setAPCallback(configModeCallback);

  //or use this for auto generated name ESP + ChipID
  wifiManager.autoConnect();

  //Manual Wifi
  //WiFi.begin(WIFI_SSID, WIFI_PWD);

  // OTA Setup
  String hostname(HOSTNAME);
  hostname += String(ESP.getChipId(), HEX);
  WiFi.hostname(hostname);
  ArduinoOTA.setHostname((const char *)hostname.c_str());
  ArduinoOTA.begin();
  SPIFFS.begin();

  //Uncomment if you want to update all internet resources
  //SPIFFS.format();

  // download images from the net. If images already exist don't download
  downloadResources();

  // load the weather information
  updateData();
}
//------------------------------------- -------------------------------------//
void loop()
{
  // Handle OTA update requests
  ArduinoOTA.handle();

  // Lo usaremos para poner el ESP en reposo si esta ha oscuras un determinado
  // tiempo, pendiente, hay que poner una resistencia entre RST y D0
  // int NivelLuz = analogRead(LUZ_PIN);
  int NivelLuz     = 30;

  delay_timer  = millis();

  if (NivelLuz < NOCHE)
   {
     if (OSCURIDAD_timer == 0)
      {
        OSCURIDAD_timer = delay_timer;
        Serial.print("OSCURIDAD_timer : ");
        Serial.println(OSCURIDAD_timer);
      }
     else
      {
         OSCURIDAD_timer = delay_timer - OSCURIDAD_timer;
         Serial.print("OSCURIDAD_timer total : ");
         Serial.println(OSCURIDAD_timer);
         if (OSCURIDAD_timer > Tiempo_Activo * 1000)
          {
            drawDormir();               // Pantalla de aviso;
            delay(3000);
            OSCURIDAD_timer = 0;
            //Serial.println("Modo SLEEP");
            ESP.deepSleep(Tiempo_Dormir * 1000000,  WAKE_RF_DEFAULT);
            delay(1000);
          }
      }
   }
  else
   {
     OSCURIDAD_timer = 0;
   }


  // Check if we should update the clock
  if (millis() - lastDrew > 30000 && wunderground.getSeconds() == "00")
   {
     drawTime();
     lastDrew = millis();
   }

  // Check if we should update weather information
  if (millis() - lastDownloadUpdate > 1000 * UPDATE_INTERVAL_SECS)
   {
     updateData();
     lastDownloadUpdate = millis();
   }

   // Cambio de pantalla
   if (millis() - lastDownloadPantalla > 1000 * UPDATE_PANTALLA_SECS)
    {
      updatePantalla();
      lastDownloadPantalla = millis();
    }

}
//------------------------------------- -------------------------------------//

//------------------------------------- -------------------------------------//
void configModeCallback (WiFiManager *myWiFiManager) // Called if WiFi has not been configured yet
{
  ui.setTextAlignment(CENTER);
  tft.setFont(&ArialRoundedMTBold_14);
  tft.setTextColor(ILI9341_ORANGE);
  ui.drawString(120, 28, "Wifi Manager");
  ui.drawString(120, 42, "Please connect to AP");
  tft.setTextColor(ILI9341_WHITE);
  ui.drawString(120, 56, myWiFiManager->getConfigPortalSSID());
  tft.setTextColor(ILI9341_ORANGE);
  ui.drawString(120, 70, "To setup Wifi Configuration");
}
//------------------------------------- -------------------------------------//
void downloadCallback(String filename, int16_t bytesDownloaded, int16_t bytesTotal)
{ // callback called during download of files. Updates progress bar
  Serial.println(String(bytesDownloaded) + " / " + String(bytesTotal));

  int percentage = 100 * bytesDownloaded / bytesTotal;
  if (percentage == 0)
   {
     ui.drawString(120, 160, filename);
   }
  if (percentage % 5 == 0)
   {
     ui.setTextAlignment(CENTER);
     ui.setTextColor(ILI9341_ORANGE, ILI9341_BLACK);
     //ui.drawString(120, 160, String(percentage) + "%");
     ui.drawProgressBar(10, 165, 240 - 20, 15, percentage, ILI9341_WHITE, ILI9341_BLUE);
   }
}
//------------------------------------- -------------------------------------//
void downloadResources() // Download the bitmaps
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&ArialRoundedMTBold_14);
  char id[5];
  for (int i = 0; i < 21; i++)
   {
     sprintf(id, "%02d", i);
     tft.fillRect(0, 120, 240, 40, ILI9341_BLACK);
     webResource.downloadFile("http://www.squix.org/blog/wunderground/" + wundergroundIcons[i] + ".bmp", wundergroundIcons[i] + ".bmp", _downloadCallback);
   }
  for (int i = 0; i < 21; i++)
   {
     sprintf(id, "%02d", i);
     tft.fillRect(0, 120, 240, 40, ILI9341_BLACK);
     webResource.downloadFile("http://www.squix.org/blog/wunderground/mini/" + wundergroundIcons[i] + ".bmp", "/mini/" + wundergroundIcons[i] + ".bmp", _downloadCallback);
   }
  for (int i = 0; i < 24; i++)
   {
     tft.fillRect(0, 120, 240, 40, ILI9341_BLACK);
     webResource.downloadFile("http://www.squix.org/blog/moonphase_L" + String(i) + ".bmp", "/moon" + String(i) + ".bmp", _downloadCallback);
   }
}
//------------------------------------- -------------------------------------//
void updateData() // Update the internet based information and update screen
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&ArialRoundedMTBold_14);
  drawProgress(20, "Updating time...");
  timeClient.updateTime();
  drawProgress(50, "Updating conditions...");
  wunderground.updateConditions(WUNDERGRROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
  drawProgress(70, "Updating forecasts...");
  wunderground.updateForecast(WUNDERGRROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
  drawProgress(90, "Updating astronomy...");
  wunderground.updateAstronomy(WUNDERGRROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
  //lastUpdate = timeClient.getFormattedTime();
  //readyForWeatherUpdate = false;
  drawProgress(100, "Done...");
  delay(1000);
  tft.fillScreen(ILI9341_BLACK);
  drawTime();
  drawCurrentWeather();
  drawForecast();
  drawAstronomy();
}
//------------------------------------- -------------------------------------//
void updatePantalla() // Update the internet based information and update screen
{
  switch (Pantalla)
   {
     case 0 : // Tiempo Exterior
              tft.fillScreen(ILI9341_BLACK);
              drawTime();
              drawCurrentWeather();
              drawForecast();
              drawAstronomy();
              Pantalla = 1;
              break;
     case 1 : // Tiempo Exterior
              drawScrollBorrar();
              drawTime();
              drawForecastSensores();
              Pantalla = 0;
              break;
   }
}
//------------------------------------- -------------------------------------//
void drawProgress(uint8_t percentage, String text) // Progress bar helper
{
  ui.setTextAlignment(CENTER);
  ui.setTextColor(ILI9341_ORANGE, ILI9341_BLACK);
  tft.fillRect(0, 140, 240, 45, ILI9341_BLACK);
  ui.drawString(120, 160, text);
  ui.drawProgressBar(10, 165, 240 - 20, 15, percentage, ILI9341_WHITE, ILI9341_BLUE);
}
//------------------------------------- -------------------------------------//
void drawTime() // draws the clock
{
  ui.setTextAlignment(CENTER);
  ui.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setFont(&ArialRoundedMTBold_14);
  String date = wunderground.getDate();
  ui.drawString(120, 20, date);

  tft.setFont(&ArialRoundedMTBold_36);
  String time = timeClient.getHours() + ":" + timeClient.getMinutes();
  ui.drawString(120, 56, time);
  drawSeparator(65);
}
//------------------------------------- -------------------------------------//
void drawCurrentWeather() // draws current weather information
{
  // Weather Icon
  String weatherIcon = getMeteoconIcon(wunderground.getTodayIcon());
  ui.drawBmp(weatherIcon + ".bmp", 0, 55);

  // Weather Text
  tft.setFont(&ArialRoundedMTBold_14);
  ui.setTextColor(ILI9341_ORANGE, ILI9341_BLACK);
  ui.setTextAlignment(RIGHT);
  ui.drawString(220, 90, wunderground.getWeatherText());

  tft.setFont(&ArialRoundedMTBold_36);
  ui.setTextColor(ILI9341_ORANGE, ILI9341_BLACK);
  ui.setTextAlignment(RIGHT);
  String degreeSign = "F";
  if (IS_METRIC)
   {
     degreeSign = "C";
   }
  String temp = wunderground.getCurrentTemp() + degreeSign;
  ui.drawString(220, 125, temp);
  drawSeparator(135);
}
//------------------------------------- -------------------------------------//
void drawForecast() // draws the three forecast columns
{
  drawForecastDetail(10, 165, 0);
  drawForecastDetail(95, 165, 2);
  drawForecastDetail(180, 165, 4);
  drawSeparator(165 + 65 + 10);
}
//------------------------------------- -------------------------------------//
void drawForecastSensores() // Escribe pantalla sensores internos
{
  drawForecastDetailSensores(40,  100, 0); // Temperatura
  drawForecastDetailSensores(150, 100, 1); // Humedad
  drawForecastDetailSensores(40 , 220, 2); // CO2
  drawForecastDetailSensores(150, 220, 3); // Presion
  drawSeparator(165 + 65 + 10);
}
//------------------------------------- -------------------------------------//
void drawForecastDetail(uint16_t x, uint16_t y, uint8_t dayIndex) // helper for the forecast columns
{
  ui.setTextColor(ILI9341_ORANGE, ILI9341_BLACK);
  tft.setFont(&ArialRoundedMTBold_14);
  ui.setTextAlignment(CENTER);
  String day = wunderground.getForecastTitle(dayIndex).substring(0, 3);
  day.toUpperCase();
  ui.drawString(x + 25, y, day);

  ui.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  ui.drawString(x + 25, y + 14, wunderground.getForecastLowTemp(dayIndex) + "|" + wunderground.getForecastHighTemp(dayIndex));

  String weatherIcon = getMeteoconIcon(wunderground.getForecastIcon(dayIndex));
  ui.drawBmp("/mini/" + weatherIcon + ".bmp", x, y + 15);
}
//------------------------------------- -------------------------------------//
void drawForecastDetailSensores(uint16_t x, uint16_t y, uint8_t SensorIndex) // Lectura y escritura sensores internos
{
  String TipoSensor = "";
  String TipoMedida = "";

  switch(SensorIndex)
   {
     case 0 : TipoSensor = "Temperatura";
              TipoMedida = " ºC.";
              break;
     case 1 : TipoSensor = "Humedad";
              TipoMedida = " %h.";
              break;
     case 2 : TipoSensor = " CO2 ";
              TipoMedida = " CO2";
              break;
     case 3 : TipoSensor = " Presion";
              TipoMedida = " hPa";
              break;
   }
  //TipoMedida = LecturaSensor(SensorIndex) + TipoMedida;
  TipoMedida = "25" + TipoMedida;


  ui.setTextColor(ILI9341_ORANGE, ILI9341_BLACK);
  tft.setFont(&ArialRoundedMTBold_14);
  ui.setTextAlignment(CENTER);
  TipoSensor.toUpperCase();
  ui.drawString(x + 25, y, TipoSensor);

  ui.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setFont(&ArialRoundedMTBold_36);
  ui.setTextAlignment(CENTER);
  ui.drawString(x + 25, y + 14, TipoMedida);
}
//------------------------------------- -------------------------------------//
void drawAstronomy() // draw moonphase and sunrise/set and moonrise/set
{
  int moonAgeImage = 24 * wunderground.getMoonAge().toInt() / 30.0;
  ui.drawBmp("/moon" + String(moonAgeImage) + ".bmp", 120 - 30, 255);

  ui.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setFont(&ArialRoundedMTBold_14);
  ui.setTextAlignment(LEFT);
  ui.setTextColor(ILI9341_ORANGE, ILI9341_BLACK);
  ui.drawString(20, 270, "Sun");
  ui.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  ui.drawString(20, 285, wunderground.getSunriseTime());
  ui.drawString(20, 300, wunderground.getSunsetTime());

  ui.setTextAlignment(RIGHT);
  ui.setTextColor(ILI9341_ORANGE, ILI9341_BLACK);
  ui.drawString(220, 270, "Moon");
  ui.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  ui.drawString(220, 285, wunderground.getMoonriseTime());
  ui.drawString(220, 300, wunderground.getMoonsetTime());
}
//------------------------------------- -------------------------------------//
void drawDormir() // draws the clock
{
  tft.fillScreen(ILI9341_BLACK);

  ui.setTextAlignment(CENTER);
  ui.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setFont(&ArialRoundedMTBold_14);
  String date = wunderground.getDate();
  ui.drawString(120, 20, date);

  tft.setFont(&ArialRoundedMTBold_36);
  String time = timeClient.getHours() + ":" + timeClient.getMinutes();
  ui.drawString(120, 56, time);
  drawSeparator(65);

  ui.setTextAlignment(CENTER);
  tft.setFont(&ArialRoundedMTBold_14);
  String texto = "Preparandose para dormir";
  ui.drawString(120, 80, texto);
  drawSeparator(100);
}
//------------------------------------- -------------------------------------//
void drawScrollBorrar() // draws the clock
{
  int MargenSuperio  = 70;
  int MargenInferior = 320;

  ui.setTextAlignment(CENTER);
  ui.setTextColor(ILI9341_WHITE, ILI9341_BLACK);

  for (int i = MargenSuperio; i<= MargenInferior; i++)
   {
      tft.drawLine(0, i, 240, i, ILI9341_BLACK);
      delay(10);                             // Segun como vaya el scroll
   }
}
//------------------------------------- -------------------------------------//
String getMeteoconIcon(String iconText) // Helper function, should be part of the
{                                       // weather station library and should disappear soon
  if (iconText == "F") return "chanceflurries";
  if (iconText == "Q") return "chancerain";
  if (iconText == "W") return "chancesleet";
  if (iconText == "V") return "chancesnow";
  if (iconText == "S") return "chancetstorms";
  if (iconText == "B") return "clear";
  if (iconText == "Y") return "cloudy";
  if (iconText == "F") return "flurries";
  if (iconText == "M") return "fog";
  if (iconText == "E") return "hazy";
  if (iconText == "Y") return "mostlycloudy";
  if (iconText == "H") return "mostlysunny";
  if (iconText == "H") return "partlycloudy";
  if (iconText == "J") return "partlysunny";
  if (iconText == "W") return "sleet";
  if (iconText == "R") return "rain";
  if (iconText == "W") return "snow";
  if (iconText == "B") return "sunny";
  if (iconText == "0") return "tstorms";

  return "unknown";
}
//------------------------------------- -------------------------------------//
void drawSeparator(uint16_t y) // if you want separators, uncomment the tft-line
{
   //tft.drawFastHLine(10, y, 240 - 2 * 10, 0x4228);
}
//------------------------------------- -------------------------------------//

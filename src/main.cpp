#define FASTLED_INTERNAL
#include <Arduino.h>
#include <FS.h>
#include <FastLED.h>
#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include <ESP8266WebServer.h>
#include "constants.h"
#include "shared.h"
#include "FileConfig.h"
#include "programs/IProgram.h"
#include "programs/Multiplexer.h"
#include "programs/TextWriter.h"
#include "programs/ImagePainter.h"
#include "WifiManager.h"

FS *fileSystem = &SPIFFS;
WiFiManager wifiManager;

CRGB leds[NUM_LEDS];
byte buf[900];
int bufIndex = 0;
FastLED_NeoMatrix matrix(leds, 32, 8,
                         NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG +
                             NEO_TILE_TOP + NEO_TILE_LEFT + NEO_TILE_PROGRESSIVE);
ESP8266WebServer webServer(HTTP_PORT);
Multiplexer multiplexer;
TextWriter textWriter(&matrix);
ImagePainter imagePainter(&matrix);
FileConfig config(fileSystem, "config");

String getContentType(String filename)
{
  if (filename.endsWith(".html"))
    return "text/html";
  else if (filename.endsWith(".css"))
    return "text/css";
  else if (filename.endsWith(".js"))
    return "application/javascript";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  return "text/plain";
}

bool readWebServerFile(String path)
{
  if (path.endsWith("/"))
    path += "index.html";
  String contentType = getContentType(path);
  if (fileSystem->exists(path))
  {
    File file = fileSystem->open(path, "r");
    webServer.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void setup()
{
  Serial.begin(115200);
  Serial.println("LightField started");

  pinMode(LED_BUILTIN, OUTPUT);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  config.add(&wifiManager);
  config.add(&multiplexer);
  config.add(&textWriter);
  
  multiplexer.insert("writer", &textWriter);
  multiplexer.insert("image", &imagePainter);

  multiplexer.registerRoutes(&webServer, &config);
  wifiManager.registerRoutes(&webServer, &config);
  textWriter.registerRoutes(&webServer, &config);
  imagePainter.registerRoutes(&webServer, &config);

  webServer.onNotFound([]() {
    if (!readWebServerFile(webServer.uri()))
    {
      webServer.send(404, "text/html", "<!DOCTYPE html><html><head><title>Not Found</title></head><body><h1>Not Found</h1></body></html>");
    }
  });

  delay(5);
  fileSystem->begin();
  config.load();
  wifiManager.begin();
  webServer.begin();

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Black;
  }
}

void loop()
{
  random16_add_entropy(random(65535));
  webServer.handleClient();
  multiplexer.render();
  EVERY_N_MILLISECONDS(10)
  {
#ifdef ESP8266
    ESP.wdtDisable();
#endif
    FastLED.show();
#ifdef ESP8266
    ESP.wdtEnable(1000);
#endif
  }
}
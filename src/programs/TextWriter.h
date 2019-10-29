#pragma once

#include <FastLED_NeoMatrix.h>
#include <ESP8266WebServer.h>
#include "IProgram.h"
#include "FileConfig.h"

class TextWriter : public IProgram, public IConfigConsumer
{
public:
  TextWriter(FastLED_NeoMatrix *matrix);
  ~TextWriter();
  void setColor(unsigned char r, unsigned char g, unsigned char b);
  void setInterval(unsigned int interval);
  void setRepeat(bool repeat);
  void write(const char *text);
  bool render();
  void readConfig(StaticJsonDocument<FILE_CONFIG_CAPACITY> &doc);
  void writeConfig(StaticJsonDocument<FILE_CONFIG_CAPACITY> &doc);
  void registerRoutes(ESP8266WebServer *webServer, FileConfig *config);

private:
  FastLED_NeoMatrix *matrix;
  int pixelPerChar = 7;
  int x = 0;
  std::string text = "";
  unsigned char r = 255;
  unsigned char g = 255;
  unsigned char b = 255;
  unsigned int interval = 20;
  bool repeat = true;
  unsigned long lastTick = 0;
};
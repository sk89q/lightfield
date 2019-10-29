#pragma once

#include <FastLED_NeoMatrix.h>
#include <ESP8266WebServer.h>
#include "IProgram.h"
#include "FileConfig.h"

class ImagePainter : public IProgram
{
public:
  ImagePainter(FastLED_NeoMatrix *matrix);
  ~ImagePainter();
  bool render();
  void registerRoutes(ESP8266WebServer *webServer, FileConfig *config);

private:
  FastLED_NeoMatrix *matrix;
};
#include <FastLED_NeoMatrix.h>
#include <ESP8266WebServer.h>
#include "shared.h"
#include "ImagePainter.h"
#include "FileConfig.h"

ImagePainter::ImagePainter(FastLED_NeoMatrix *matrix)
{
  this->matrix = matrix;
};

ImagePainter::~ImagePainter()
{
}

bool ImagePainter::render()
{
  return true;
}

void ImagePainter::registerRoutes(ESP8266WebServer *webServer, FileConfig *config)
{
  webServer->on("/api/v1/programs/image/", HTTP_POST,
  [this, webServer]() {
    webServer->send(200, "text/plain", "");
  },
  [this, webServer]() {
    HTTPUpload& upload = webServer->upload();
    if (upload.status == UPLOAD_FILE_START)
    {
      bufIndex = 0;
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
      unsigned int newLength = bufIndex + upload.currentSize;
      if (newLength <= sizeof(buf) / sizeof(buf[0])) {
        memcpy(&buf[bufIndex], upload.buf, upload.currentSize);
        bufIndex = newLength;
      }
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
      for (int i = 0; i < bufIndex / 3; i++) {
        leds[i] = CRGB(buf[i * 3], buf[i * 3 + 1], buf[i * 3 + 2]);
      }
    }
  });
}
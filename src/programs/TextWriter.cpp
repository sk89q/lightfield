#include <FastLED_NeoMatrix.h>
#include <ESP8266WebServer.h>
#include "TextWriter.h"
#include "FileConfig.h"

TextWriter::TextWriter(FastLED_NeoMatrix *matrix)
{
  this->matrix = matrix;
};

TextWriter::~TextWriter()
{
}

void TextWriter::setColor(unsigned char r, unsigned char g, unsigned char b)
{
  this->r = r;
  this->g = g;
  this->b = b;
}

void TextWriter::setInterval(unsigned int interval)
{
  this->interval = interval;
}

void TextWriter::setRepeat(bool repeat)
{
  this->repeat = repeat;
}

void TextWriter::write(const char *text)
{
  this->x = this->matrix->width();
  this->text.assign(text);
}

bool TextWriter::render()
{
  if (this->text.length())
  {
    unsigned long now = millis();
    if (now - this->lastTick > this->interval)
    {
      int msgSize = (this->text.length() * pixelPerChar);
      int scrollingMax = msgSize + this->matrix->width();

      this->matrix->setFont();
      this->matrix->setTextColor(matrix->Color(this->r, this->g, this->b));
      this->matrix->fillScreen(0);
      this->matrix->setTextWrap(false);
      this->matrix->setCursor(x, 0);
      this->matrix->print(this->text.c_str());

      if (--this->x < -scrollingMax)
      {
        if (this->repeat)
        {
          this->x = this->matrix->width();
        }
        else
        {
          write("");
        }
      }

      this->lastTick = now;
    }

    return true;
  }
  else
  {
    return false;
  }
}

void TextWriter::readConfig(StaticJsonDocument<FILE_CONFIG_CAPACITY> &doc)
{
  this->setColor(doc["writerR"], doc["writerG"], doc["writerB"]);
  this->setInterval(doc["writerInterval"]);
  this->setRepeat(doc["writerRepeat"]);
  this->write(doc["writerText"] ? doc["writerText"].as<char *>() : "");
}

void TextWriter::writeConfig(StaticJsonDocument<FILE_CONFIG_CAPACITY> &doc)
{
  doc["writerR"] = this->r;
  doc["writerG"] = this->g;
  doc["writerB"] = this->b;
  doc["writerInterval"] = this->interval;
  doc["writerRepeat"] = this->repeat;
  doc["writerText"] = this->text.c_str();
}

void TextWriter::registerRoutes(ESP8266WebServer *webServer, FileConfig *config)
{
  webServer->on("/api/v1/programs/writer/", HTTP_POST, [this, webServer, config]() {
    if (webServer->hasArg("r") && webServer->hasArg("g") && webServer->hasArg("b"))
    {
      const char r = (char)atoi(webServer->arg("r").c_str());
      const char g = (char)atoi(webServer->arg("g").c_str());
      const char b = (char)atoi(webServer->arg("b").c_str());
      this->setColor(r, g, b);
    }

    if (webServer->hasArg("interval"))
    {
      int interval = atoi(webServer->arg("interval").c_str());
      this->setInterval(interval);
    }

    if (webServer->hasArg("repeat"))
    {
      bool repeat = atoi(webServer->arg("repeat").c_str()) == 1;
      this->setRepeat(repeat);
    }

    if (webServer->hasArg("text"))
    {
      this->write(webServer->arg("text").c_str());
    }

    config->save();
    webServer->send(200, "text/plain", "");
  });
}
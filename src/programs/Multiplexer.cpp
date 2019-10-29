#include <map>
#include <FastLED_NeoMatrix.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "constants.h"
#include "IProgram.h"
#include "Multiplexer.h"
#include "FileConfig.h"

Multiplexer::Multiplexer()
{
}

void Multiplexer::insert(std::string key, IProgram *program)
{
  this->programs[key] = program;
}

bool Multiplexer::select(std::string key)
{
  ProgramMap::const_iterator it = this->programs.find(key);
  if (it != this->programs.end())
  {
    this->selectedKey = it->first;
    this->selected = it->second;
    return true;
  }
  else
  {
    return false;
  }
}

bool Multiplexer::render()
{
  if (this->selected != NULL)
  {
    this->selected->render();
  }
  return true;
}

void Multiplexer::readConfig(StaticJsonDocument<FILE_CONFIG_CAPACITY> &doc)
{
  this->select(doc["mplexChoice"] ? doc["mplexChoice"].as<char *>() : "");
}

void Multiplexer::writeConfig(StaticJsonDocument<FILE_CONFIG_CAPACITY> &doc)
{
  doc["mplexChoice"] = this->selectedKey.c_str();
}

void Multiplexer::registerRoutes(ESP8266WebServer *webServer, FileConfig *config)
{
  webServer->on("/api/v1/program/", HTTP_GET, [this, webServer]() {
    webServer->send(200, "text/plain", this->selectedKey.c_str());
  });

  webServer->on("/api/v1/program/", HTTP_POST, [this, webServer, config]() {
    this->select(webServer->arg("program").c_str());
    config->save();
    webServer->send(200, "text/plain", "");
  });
}
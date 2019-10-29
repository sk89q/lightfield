#pragma once

#include <map>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "IProgram.h"
#include "FileConfig.h"

class Multiplexer : public IProgram, public IConfigConsumer
{
  typedef std::map<const std::string, IProgram*> ProgramMap;

public:
  Multiplexer();
  void insert(std::string key, IProgram *program);
  bool select(std::string key);
  bool render();
  void readConfig(StaticJsonDocument<FILE_CONFIG_CAPACITY> &doc);
  void writeConfig(StaticJsonDocument<FILE_CONFIG_CAPACITY> &doc);
  void registerRoutes(ESP8266WebServer *webServer, FileConfig *config);

private:
  ProgramMap programs;
  std::string selectedKey = "";
  IProgram *selected = NULL;
};

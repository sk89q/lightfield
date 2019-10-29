#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "FileConfig.h"

class WiFiManager : public IConfigConsumer
{
public:
  WiFiManager();
  void begin();
  void enableAccessPoint(const char *ssid, const char *password);
  void disableAccessPoint();
  void enableClient(const char *ssid, const char *password);
  void disableClient();
  void readConfig(StaticJsonDocument<FILE_CONFIG_CAPACITY> &doc);
  void writeConfig(StaticJsonDocument<FILE_CONFIG_CAPACITY> &doc);
  void registerRoutes(ESP8266WebServer *webServer, FileConfig *config);

private:
  bool active = false;
  bool wifiApEnabled = false;
  std::string wifiApSsid;
  std::string wifiApPass;
  bool wifiClientEnabled = false;
  std::string wifiClientSsid;
  std::string wifiClientPass;
};

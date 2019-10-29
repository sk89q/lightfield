#include <ESP8266WiFi.h>
#include <ESP8266webServer.h>
#include "constants.h"
#include "WifiManager.h"
#include "FileConfig.h"

WiFiManager::WiFiManager()
{
}

void WiFiManager::readConfig(StaticJsonDocument<FILE_CONFIG_CAPACITY> &doc)
{
  this->active = doc["active"];
  this->wifiApEnabled = doc["wifiApEnabled"];
  this->wifiApSsid.assign(doc["wifiApSsid"] ? doc["wifiApSsid"].as<char *>() : "");
  if (this->wifiApSsid.length() == 0)
  {
    this->wifiApSsid = "LightField";
  }
  this->wifiApPass.assign(doc["wifiApPass"] ? doc["wifiApPass"].as<char *>() : "");
  if (this->wifiApPass.length() == 0)
  {
    this->wifiApSsid = "admin123";
  }
  this->wifiClientEnabled = doc["wifiClientEnabled"];
  this->wifiClientSsid.assign(doc["wifiClientSsid"] ? doc["wifiClientSsid"].as<char *>() : "");
  this->wifiClientPass.assign(doc["wifiClientPass"] ? doc["wifiClientPass"].as<char *>() : "");
}

void WiFiManager::writeConfig(StaticJsonDocument<FILE_CONFIG_CAPACITY> &doc)
{
  doc["active"] = this->active;
  doc["wifiApEnabled"] = this->wifiApEnabled;
  doc["wifiApSsid"] = this->wifiApSsid.c_str();
  doc["wifiApPass"] = this->wifiApPass.c_str();
  doc["wifiClientEnabled"] = this->wifiClientEnabled;
  doc["wifiClientSsid"] = this->wifiClientSsid.c_str();
  doc["wifiClientPass"] = this->wifiClientPass.c_str();
}

void WiFiManager::enableAccessPoint(const char *ssid, const char *password)
{
  this->active = true;
  this->wifiApSsid = ssid;
  this->wifiApPass = password;
  this->wifiApEnabled = true;
}

void WiFiManager::disableAccessPoint()
{
  this->active = true;
  this->wifiApEnabled = false;
}

void WiFiManager::enableClient(const char *ssid, const char *password)
{
  this->active = true;
  this->wifiClientSsid = ssid;
  this->wifiClientPass = password;
  this->wifiClientEnabled = true;
}

void WiFiManager::disableClient()
{
  this->active = true;
  this->wifiClientEnabled = false;
}

void WiFiManager::begin()
{
  digitalWrite(LED_BUILTIN, HIGH);
  WiFi.disconnect();

  if (this->active)
  {
    if (this->wifiApEnabled)
    {
      Serial.print("Creating soft AP named '");
      Serial.print(this->wifiApSsid.c_str());
      Serial.println("'...");

      if (WiFi.softAP(this->wifiApSsid.c_str(), this->wifiApPass.c_str()))
      {
        Serial.println("Soft AP available");
        digitalWrite(LED_BUILTIN, LOW);
      }
      else
      {
        Serial.println("Soft AP did not work");
      }
    }

    if (this->wifiClientEnabled)
    {
      Serial.print("Connecting to AP named '");
      Serial.print(this->wifiClientSsid.c_str());
      Serial.println("'...");

      WiFi.begin(this->wifiClientSsid.c_str(), this->wifiClientPass.c_str());
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
  else
  {
    Serial.print("Creating soft AP named '");
    Serial.print(WIFI_CONFIG_SSID);
    Serial.print("' with password '");
    Serial.print(WIFI_CONFIG_PASS);
    Serial.println("'...");

    if (WiFi.softAP(WIFI_CONFIG_SSID, WIFI_CONFIG_PASS))
    {
      Serial.println("Soft AP available");
      digitalWrite(LED_BUILTIN, LOW);
    }
    else
    {
      Serial.println("Soft AP did not work");
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }
}

void WiFiManager::registerRoutes(ESP8266WebServer *webServer, FileConfig *config)
{
  webServer->on("/api/v1/wifi/ap/", HTTP_POST, [this, webServer, config]() {
    const char *ssid = webServer->arg("ssid").c_str();
    const char *password = webServer->arg("password").c_str();

    if (strlen(ssid))
    {
      this->enableAccessPoint(ssid, password);
    }
    else
    {
      this->disableAccessPoint();
    }

    config->save();
    this->begin();
    webServer->send(200, "text/plain", "");
  });

  webServer->on("/api/v1/wifi/client/", HTTP_POST, [this, webServer, config]() {
    const char *ssid = webServer->arg("ssid").c_str();
    const char *password = webServer->arg("password").c_str();

    if (strlen(ssid))
    {
      this->enableClient(ssid, password);
    }
    else
    {
      this->disableClient();
    }

    config->save();
    this->begin();
    webServer->send(200, "text/plain", "");
  });
}
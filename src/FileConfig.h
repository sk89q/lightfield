#pragma once

#include <vector>

#include <FS.h>
#include <ArduinoJson.h>
#include "constants.h"

class IConfigConsumer
{
public:
  virtual void readConfig(StaticJsonDocument<FILE_CONFIG_CAPACITY> &doc) = 0;
  virtual void writeConfig(StaticJsonDocument<FILE_CONFIG_CAPACITY> &doc) = 0;
};

class FileConfig
{
public:
  FileConfig(FS *fileSystem, const char *filename);
  void add(IConfigConsumer *consumer);
  void load();
  void save();

private:
  FS *fileSystem;
  const char *filename;
  std::vector<IConfigConsumer*> consumers;
};

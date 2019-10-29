#include <vector>
#include <FS.h>
#include <ArduinoJson.h>
#include "FileConfig.h"
#include "constants.h"

FileConfig::FileConfig(FS *fileSystem, const char *filename)
{
  this->fileSystem = fileSystem;
  this->filename = filename;
}

void FileConfig::add(IConfigConsumer *consumer)
{
  this->consumers.insert(this->consumers.begin(), consumer);
}

void FileConfig::load()
{
  File f = this->fileSystem->open(this->filename, "r");
  if (f)
  {
    StaticJsonDocument<FILE_CONFIG_CAPACITY> doc;
    DeserializationError error = deserializeJson(doc, f);
    if (!error)
    {
      size_t count = 0;
      for (std::vector<IConfigConsumer*>::iterator it = this->consumers.begin(); it != this->consumers.end(); ++it)
      {
        (*it)->readConfig(doc);
        count++;
      }

      Serial.println("Config read");
    }
    else
    {
      Serial.print("Config could not be parsed: ");
      Serial.println(error.c_str());
    }
    f.close();
  }
  else
  {
    Serial.println("Config file could not be opened for reading");
  }
}

void FileConfig::save()
{
  StaticJsonDocument<FILE_CONFIG_CAPACITY> doc;
  File f = this->fileSystem->open(this->filename, "w");
  if (f)
  {
    for (std::vector<IConfigConsumer*>::iterator it = this->consumers.begin(); it != this->consumers.end(); ++it)
    {
      (*it)->writeConfig(doc);
    }

    int size = serializeJson(doc, f);
    if (size > 0)
    {
      Serial.print("Config file saved with ");
      Serial.print(size);
      Serial.println(" byte(s)");
    }
    else
    {
      Serial.println("Config file could not be written to");
    }
    f.close();
  }
  else
  {
    Serial.println("Config file could not be opened for writing");
  }
}

#ifndef FileSystem_h
#define FileSystem_h

#include <Arduino.h>
#include <LittleFS.h>

class FileSystem {
 private:
  boolean isMounted = false;

 public:
  void begin();
  boolean write(String key, String value);
  String read(String key, String defaultValue);
  boolean format();
};

#endif
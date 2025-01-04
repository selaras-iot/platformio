#ifndef SelarasFS_h
#define SelarasFS_h

#include <Arduino.h>
#include <LittleFS.h>

class SelarasFS {
 private:
  boolean isMounted = false;

 public:
  void begin();
  boolean write(String key, String value);
  String read(String key, String defaultValue);
};

#endif
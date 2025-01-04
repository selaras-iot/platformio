#ifndef Network_h
#define Network_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FileSystem.h>
#include <config.h>

enum class NetworkMode { STA, AP };

class Network {
 private:
  unsigned long previousMillis = 0;
  FileSystem *fileSystem;
  boolean isConnected = false;
  void (*onConnected)();
  NetworkMode networkMode;

 public:
  void begin(FileSystem *fileSystem, NetworkMode networkMode,
             void (*onConnected)());
  void loop();
};

#endif
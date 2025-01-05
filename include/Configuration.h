#ifndef Configuration_h
#define Configuration_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <Config.h>
#include <ESPAsyncWebServer.h>
#include <FileSystem.h>

// device config
#define CONFIG_SSID_PATH "/ssid.txt"
#define CONFIG_PASSWORD_PATH "/password.txt"
#define CONFIG_LED_COUNT_PATH "/led_count.txt"

// led strip
#define CONFIG_AUTO_MODE_PATH "/auto_mode.txt"
#define CONFIG_MODE_PATH "/mode.txt"
#define CONFIG_BRIGHTNESS_PATH "/brightness.txt"
#define CONFIG_SPEED_PATH "/speed.txt"
#define CONFIG_COLOR_PATH "/color.txt"

struct deviceConfig {
  String ssid;
  String password;
  int ledCount;
};
typedef struct deviceConfig DeviceConfig;

struct ledConfig {
  boolean autoEnabled;
  int mode;
  int brightness;
  int speed;
  int color;
};
typedef struct ledConfig LEDConfig;

class Configuration {
 private:
  FileSystem *fileSystem;

 public:
  void begin(FileSystem *fileSystem);
  void beginServer(AsyncWebServer *server);
  boolean saveDeviceConfig(DeviceConfig config);
  boolean saveLEDConfig(LEDConfig config);
  DeviceConfig readDeviceConfig();
  LEDConfig readLEDConfig();
};

#endif
#include <Configuration.h>

void Configuration::begin(FileSystem *fileSystem) {
  this->fileSystem = fileSystem;
}

boolean Configuration::saveDeviceConfig(DeviceConfig config) {
  if (this->fileSystem == NULL) return false;

  this->fileSystem->write(CONFIG_SSID_PATH, config.ssid);
  this->fileSystem->write(CONFIG_PASSWORD_PATH, config.password);
  this->fileSystem->write(CONFIG_LED_COUNT_PATH, String(config.ledCount));

  return true;
}

boolean Configuration::saveLEDConfig(LEDConfig config) {
  if (this->fileSystem == NULL) return false;

  this->fileSystem->write(CONFIG_AUTO_MODE_PATH,
                          config.autoEnabled ? "true" : "false");
  this->fileSystem->write(CONFIG_MODE_PATH, String(config.mode));
  this->fileSystem->write(CONFIG_BRIGHTNESS_PATH, String(config.brightness));
  this->fileSystem->write(CONFIG_SPEED_PATH, String(config.speed));

  return true;
}

DeviceConfig Configuration::readDeviceConfig() {
  DeviceConfig config = DeviceConfig();

  config.ssid = this->fileSystem->read(CONFIG_SSID_PATH, "");
  config.password = this->fileSystem->read(CONFIG_PASSWORD_PATH, "");
  config.ledCount = this->fileSystem->read(CONFIG_LED_COUNT_PATH, "0").toInt();

  return config;
}

LEDConfig Configuration::readLEDConfig() {
  LEDConfig config = LEDConfig();

  config.autoEnabled =
      this->fileSystem->read(CONFIG_AUTO_MODE_PATH, "false") == "true";
  config.mode = this->fileSystem->read(CONFIG_MODE_PATH, "0").toInt();
  config.brightness =
      this->fileSystem->read(CONFIG_BRIGHTNESS_PATH, "0").toInt();
  config.speed = this->fileSystem->read(CONFIG_SPEED_PATH, "0").toInt();

  return config;
}
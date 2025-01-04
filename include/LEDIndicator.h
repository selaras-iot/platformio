#ifndef LEDIndicator_h
#define LEDIndicator_h

#include <Arduino.h>

class LEDIndicator {
 private:
  unsigned long interval = 1000;
  unsigned long previousMillis = 0;
  boolean isTurnedOn = false;

 public:
  void begin();
  void turnOn(unsigned long interval);
  void turnOff();
  void loop();
};

#endif
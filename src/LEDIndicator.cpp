#include <LEDIndicator.h>

void LEDIndicator::begin() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, OUTPUT);
}

void LEDIndicator::turnOn(unsigned long interval) {
  if (!LEDIndicator::isTurnedOn || LEDIndicator::interval != interval) {
    LEDIndicator::isTurnedOn = true;
    LEDIndicator::interval = interval;
  }
}

void LEDIndicator::turnOff() {
  if (LEDIndicator::isTurnedOn) LEDIndicator::isTurnedOn = false;
}

void LEDIndicator::loop() {
  unsigned long currentMillis = millis();
  if (LEDIndicator::isTurnedOn &&
      currentMillis - LEDIndicator::previousMillis >= LEDIndicator::interval) {
    LEDIndicator::previousMillis = currentMillis;

    int currentState = digitalRead(LED_BUILTIN);
    digitalWrite(LED_BUILTIN, currentState == HIGH ? LOW : HIGH);
  }
}
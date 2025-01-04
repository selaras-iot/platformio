#include <ResetDetector.h>

void ResetDetector::saveCounter(int counter) {
  ResetDetector::fileSystem->write(RESET_COUNT_FILE, String(counter));
}

void ResetDetector::begin(FileSystem* fileSystem, void (*callback)()) {
  ResetDetector::fileSystem = fileSystem;

  int resetCount = fileSystem->read(RESET_COUNT_FILE, "1").toInt();

  if (resetCount >= 3) {
    callback();
    ResetDetector::saveCounter(1);
  } else {
    ResetDetector::saveCounter(resetCount + 1);
  }
}

void ResetDetector::loop() {
  unsigned long currentMillis = millis();

  if (currentMillis >= 3000 && !ResetDetector::isTimeout) {
    ResetDetector::isTimeout = true;
    ResetDetector::saveCounter(1);
  }
}

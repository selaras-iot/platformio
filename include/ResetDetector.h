#ifndef ResetDetector_h
#define ResetDetector_h

#include <Arduino.h>
#include <FileSystem.h>

#define RESET_COUNT_FILE "/resetCount.txt"

class ResetDetector {
 private:
  boolean isTimeout = false;
  FileSystem* fileSystem;
  void saveCounter(int counter);

 public:
  void begin(FileSystem* fileSystem, void (*callback)());
  void loop();
};

#endif
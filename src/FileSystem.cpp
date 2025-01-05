#include <FileSystem.h>

void FileSystem::begin() {
  FileSystem::isMounted = LittleFS.begin();
  if (!FileSystem::isMounted) Serial.println("Failed to mount file system");
}

boolean FileSystem::write(String key, String value) {
  if (FileSystem::isMounted) {
    File file = LittleFS.open(key, "w");
    if (!file) return false;

    boolean result = file.print(value);
    file.close();

    return result;
  }

  return false;
}

String FileSystem::read(String key, String defaultValue) {
  if (FileSystem::isMounted) {
    File file = LittleFS.open(key, "r");
    if (!file || (file && file.size() == 0)) return defaultValue;

    String value = file.readString();
    file.close();

    return value;
  }

  return defaultValue;
}

boolean FileSystem::format() {
  if (this->isMounted) return LittleFS.format();
  return false;
}
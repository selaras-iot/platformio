#include <SelarasFS.h>

void SelarasFS::begin() { SelarasFS::isMounted = LittleFS.begin(); }

boolean SelarasFS::write(String key, String value) {
  if (SelarasFS::isMounted) {
    File file = LittleFS.open(key, "w");
    if (!file) return false;

    boolean result = file.print(value);
    file.close();

    return result;
  }

  return false;
}

String SelarasFS::read(String key, String defaultValue) {
  if (SelarasFS::isMounted) {
    File file = LittleFS.open(key, "r");
    if (!file || (file && file.size() == 0)) return defaultValue;

    String value = file.readString();
    file.close();

    return value;
  }

  return defaultValue;
}
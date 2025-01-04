#include <Network.h>

void Network::begin(FileSystem *fileSystem, NetworkMode networkMode,
                    void (*onConnected)()) {
  Network::onConnected = onConnected;
  Network::networkMode = networkMode;

  if (networkMode == NetworkMode::STA) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
  } else if (networkMode == NetworkMode::AP)
    WiFi.softAP("Selaras Smart Device", "");
}

void Network::loop() {
  unsigned long currentMillis = millis();
  if (!Network::isConnected &&
      currentMillis - Network::previousMillis >= 1000 &&
      Network::networkMode == NetworkMode::STA) {
    Network::previousMillis = currentMillis;

    if (WiFi.status() == WL_CONNECTED) {
      Network::onConnected();
      Network::isConnected = true;
      Serial.println("Connected!");
      Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
    } else {
      Serial.println("Connecting to wifi...");
    }
  }
}
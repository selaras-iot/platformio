#include <Network.h>

void Network::begin(String ssid, String password, NetworkMode networkMode) {
  this->networkMode = networkMode;

  if (networkMode == NetworkMode::STA) {
    WiFi.begin(ssid, password);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
  } else if (networkMode == NetworkMode::AP)
    WiFi.softAP("Selaras Smart Device", "");
}

void Network::loop() {
  unsigned long currentMillis = millis();
  if (!this->isConnected && currentMillis - this->previousMillis >= 1000 &&
      this->networkMode == NetworkMode::STA) {
    this->previousMillis = currentMillis;

    if (WiFi.status() == WL_CONNECTED) {
      this->isConnected = true;
      Serial.println("Connected!");
      Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
    } else {
      Serial.println("Connecting to wifi...");
    }
  }
}
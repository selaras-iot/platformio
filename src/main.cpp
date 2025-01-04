#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#elif defined(ESP32)
#include <AsyncTCP.h>
#include <WiFi.h>
#endif

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <FileSystem.h>
#include <LEDIndicator.h>
#include <MQTT.h>
#include <NeoPixelBus.h>
#include <Network.h>
#include <ResetDetector.h>
#include <WS2812FX.h>
#include <WebSerial.h>
#include <config.h>

AsyncWebServer server(80);
WS2812FX* ws2812fx;
NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod>* neoPixelBus;

FileSystem fileSystem;
ResetDetector resetDetector;
Network network;
LEDIndicator ledIndicator;
MQTT mqtt;

boolean isConfigurationModeEnabled = false;
void onEnterConfigurationMode() {
  Serial.println("ESP entering configuration mode!!");
  isConfigurationModeEnabled = true;

  ledIndicator.turnOn(250);
}

void onConnected() { Serial.println("Network connected!"); }

void setup() {
  Serial.begin(115200);
  delay(1000);

  randomSeed(micros());
  ledIndicator.begin();

  Serial.println("Initialize file system...");
  Serial.println("Initialize file system...");
  Serial.println("Initialize file system...");
  Serial.println("Initialize file system...");
  Serial.println("Initialize file system...");
  Serial.println("Initialize file system...");

  fileSystem.begin();

  // initialize led strip
  int ledCount = 44;
  // int ledPin = 3;
  ws2812fx = new WS2812FX(ledCount, 3, NEO_GRB + NEO_KHZ800);
  ws2812fx->init();

  neoPixelBus =
      new NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod>(ledCount);
  neoPixelBus->Begin();
  neoPixelBus->Show();

  resetDetector.begin(&fileSystem, onEnterConfigurationMode);
  network.begin(&fileSystem,
                isConfigurationModeEnabled ? NetworkMode::AP : NetworkMode::STA,
                onConnected);

  // initialize custom show
  ws2812fx->setCustomShow([]() {
    if (neoPixelBus->CanShow()) {
      // copy the WS2812FX pixel data to the NeoPixelBus instance
      memcpy(neoPixelBus->Pixels(), ws2812fx->getPixels(),
             neoPixelBus->PixelsSize());
      neoPixelBus->Dirty();
      neoPixelBus->Show();
    }
  });

  // initialize default config for ws2812
  ws2812fx->setBrightness(100);
  ws2812fx->setSpeed(1000);
  ws2812fx->setMode(FX_MODE_STATIC);
  ws2812fx->start();

  // Serve a simple webpage
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain",
                  "Hi! This is WebSerial demo. You can access the WebSerial "
                  "interface at http://" +
                      WiFi.localIP().toString() + "/webserial");
  });

  WebSerial.begin(&server);

  server.begin();
  mqtt.begin(
      [](boolean isConnected) {
        if (isConnected)
          ledIndicator.turnOn(1000);
        else
          ledIndicator.turnOff();
      },
      [](MQTT_TOPIC topic, String payload) {
        Serial.printf("MQTT message received: %s\n", payload.c_str());
      });
}

// int value = 0;
// unsigned long lastMsg = 0;
// #define MSG_BUFFER_SIZE (50)
// char msg[MSG_BUFFER_SIZE];

unsigned long lastReconnectAttempt = 0;
unsigned long lastStatsPrint = 0;
unsigned long previousMillisConfigModeIndicator = 0;

void loop() {
  if (!isConfigurationModeEnabled) {
    // only executed on normal mode
    resetDetector.loop();
    mqtt.loop();
    ws2812fx->service();
  }

  // unsigned long now = millis();
  // if (now - lastMsg > 2000) {
  //   lastMsg = now;
  //   ++value;
  //   snprintf(msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
  //   Serial.print("Publish message: ");
  //   Serial.println(msg);
  //   client.publish("rdaOutTopic", msg);
  // }

  // Print every 2 seconds (non-blocking)
  if ((unsigned long)(millis() - lastStatsPrint) > 2000) {
    WebSerial.print(F("IP address: "));
    WebSerial.println(WiFi.localIP());
    WebSerial.printf("Uptime: %lums\n", millis());
    WebSerial.printf("Free heap: %u\n", ESP.getFreeHeap());
    lastStatsPrint = millis();
  }

  WebSerial.loop();
  network.loop();
  ledIndicator.loop();
}
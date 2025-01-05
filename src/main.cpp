#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#elif defined(ESP32)
#include <AsyncTCP.h>
#include <WiFi.h>
#endif

#include <Arduino.h>
#include <Configuration.h>
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
Configuration configuration;

boolean isDevicedConfigured = false;
boolean isConfigurationModeEnabled = false;

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

  // read configuration
  configuration.begin(&fileSystem);
  auto deviceConfig = configuration.readDeviceConfig();
  if (deviceConfig.ssid.length() > 0 && deviceConfig.ledCount > 0) {
    Serial.println("Device already configured!");
    isDevicedConfigured = true;
  } else {
    Serial.println("Device need configuration using http server!");
  }

  // initialize led strip
  if (isDevicedConfigured) {
    ws2812fx = new WS2812FX(deviceConfig.ledCount, 3, NEO_GRB + NEO_KHZ800);
    ws2812fx->init();

    neoPixelBus = new NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod>(
        deviceConfig.ledCount);
    neoPixelBus->Begin();
    neoPixelBus->Show();
  }

  resetDetector.begin(&fileSystem, []() {
    Serial.println("ESP entering configuration mode!!");
    isConfigurationModeEnabled = true;
    ledIndicator.turnOn(250);

    // setup server endpoint for configuration
    configuration.beginServer(&server);
  });

  if (isDevicedConfigured || isConfigurationModeEnabled) {
    network.begin(
        deviceConfig.ssid, deviceConfig.password,
        isConfigurationModeEnabled ? NetworkMode::AP : NetworkMode::STA);
  }

  if (isDevicedConfigured) {
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
    ws2812fx->setSpeed(1000);
    ws2812fx->setMode(FX_MODE_STATIC);
    ws2812fx->setBrightness(100);
    ws2812fx->start();

    // initialize mqtt
    mqtt.begin(
        [](boolean isConnected) {
          if (isConnected)
            ledIndicator.turnOn(1000);
          else
            ledIndicator.turnOff();
        },
        [](MQTT_TOPIC topic, String payload) {
          if (topic == MQTT_TOPIC::MODE) {
            int newMode = payload.toInt();
            if (ws2812fx->getMode() != newMode) {
              ws2812fx->setMode(newMode);
            }
          } else if (topic == MQTT_TOPIC::BRIGHTNESS) {
            int newBrightness = payload.toInt() + 1;
            if (ws2812fx->getBrightness() != newBrightness) {
              ws2812fx->setBrightness(newBrightness);
            }
          } else if (topic == MQTT_TOPIC::SPEED) {
            int newSpeed = payload.toInt();
            if (ws2812fx->getSpeed() != newSpeed) {
              ws2812fx->setSpeed(newSpeed);
            }
          }
        });
  }

  // initialize web server, with/without device configured
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain",
                  "Hi! This is WebSerial demo. You can access the WebSerial "
                  "interface at http://" +
                      WiFi.localIP().toString() + "/webserial");
  });

  WebSerial.begin(&server);
  server.begin();
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

    if (isDevicedConfigured) {
      mqtt.loop();
      ws2812fx->service();
    }
  }

  if (isDevicedConfigured || isConfigurationModeEnabled) {
    network.loop();
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
  ledIndicator.loop();
}
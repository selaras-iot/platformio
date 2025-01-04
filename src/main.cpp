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
#include <PubSubClient.h>
#include <ResetDetector.h>
#include <WS2812FX.h>
#include <WebSerial.h>
#include <config.h>

AsyncWebServer server(80);
// WiFiClient wifiClient;
// PubSubClient client(wifiClient);

FileSystem fileSystem;
ResetDetector resetDetector;
Network network;
LEDIndicator ledIndicator;
MQTT mqtt;

WS2812FX* ws2812fx;
NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod>* neoPixelBus;

// topics
// String subTopicMode = "", subTopicBrightness = "";

// void callback(char* topic, byte* payload, unsigned int length) {
//   String strTopic = String(topic);
//   String strPayload = "";

//   for (unsigned int a = 0; a < length; a++) strPayload += (char)payload[a];

//   WebSerial.printf("Message arrived [%s] %s\n", strTopic.c_str(),
//                    strPayload.c_str());

//   if (strTopic == subTopicMode) {
//     // WebSerial.printf("Change mode to %s\n", strPayload.c_str());
//     // int newMode = strPayload.toInt();
//     // if (ws2812fx.getMode() != newMode) {
//     //   ws2812fx.setMode(newMode);
//     //   // const uint32_t colors[] = {RED, BLACK, BLACK};
//     //   // ws2812fx.setSegment(0, 0, 44 - 1, newMode, colors, 2000,
//     //   NO_OPTIONS);
//     // }
//   }

//   if (strTopic == subTopicBrightness) {
//     // WebSerial.printf("Change brightness to %s\n", strPayload.c_str());
//     // int newBrightness = strPayload.toInt();
//     // if (ws2812fx.getBrightness() != newBrightness)
//     //   ws2812fx.setBrightness(newBrightness);
//   }

//   // WebSerial.print("Message arrived [");
//   // WebSerial.print(topic);
//   // WebSerial.print("] ");
//   // for (int i = 0; i < length; i++) {
//   //   WebSerial.print((char)payload[i]);
//   // }
//   // WebSerial.println();
//   // if (strcmp(topic,
//   //            ("com.rizalanggoro.selaras/" + String(DEVICE_UUID) + "/mode")
//   //                .c_str()) == 0) {
//   //   int mode = atoi((char*)payload);
//   //   ws2812fx.setMode(mode);
//   //   WebSerial.print("mode: ");
//   //   WebSerial.println(mode);
//   // } else if (strcmp(topic, ("com.rizalanggoro.selaras/" +
//   // String(DEVICE_UUID)
//   // +
//   //                           "/brightness")
//   //                              .c_str()) == 0) {
//   //   int brightness = atoi((char*)payload);
//   //   ws2812fx.setBrightness(brightness);
//   //   WebSerial.print("brightness: ");
//   //   WebSerial.println(brightness);
//   // }
// }

// boolean reconnect() {
//   WebSerial.print("Attempting MQTT connection...");
//   String clientId = "ESP8266Client-";
//   clientId += String(random(0xffff), HEX);
//   WebSerial.println(clientId);

//   if (client.connect(clientId.c_str())) {
//     client.subscribe(subTopicMode.c_str());
//     client.subscribe(subTopicBrightness.c_str());
//   }

//   return client.connected();
// }

// void initializeTopics() {
//   String org = "com.rizalanggoro.selaras";
//   String prefix = org + "/" + DEVICE_UUID + "/";

//   subTopicMode = prefix + "mode";
//   subTopicBrightness = prefix + "brightness";
// }

boolean isConfigurationModeEnabled = false;
void onEnterConfigurationMode() {
  Serial.println("ESP entering configuration mode!!");
  isConfigurationModeEnabled = true;

  ledIndicator.turnOn(250);
}

void onConnected() { Serial.println("Network connected!"); }

void mqttCallback(boolean isConnected) {
  if (isConnected)
    ledIndicator.turnOn(1000);
  else
    ledIndicator.turnOff();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  ledIndicator.begin();

  Serial.println("Initialize file system...");
  Serial.println("Initialize file system...");
  Serial.println("Initialize file system...");
  Serial.println("Initialize file system...");
  Serial.println("Initialize file system...");
  Serial.println("Initialize file system...");

  fileSystem.begin();
  resetDetector.begin(&fileSystem, onEnterConfigurationMode);
  network.begin(&fileSystem,
                isConfigurationModeEnabled ? NetworkMode::AP : NetworkMode::STA,
                onConnected);

  // initialize led strip
  int ledCount = 10;
  int ledPin = 3;

  ws2812fx = new WS2812FX(ledCount, ledPin, NEO_GRB + NEO_KHZ800);
  neoPixelBus =
      new NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod>(ledCount);

  ws2812fx->init();
  neoPixelBus->Begin();
  neoPixelBus->Show();

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

  randomSeed(micros());

  // Serve a simple webpage
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain",
                  "Hi! This is WebSerial demo. You can access the WebSerial "
                  "interface at http://" +
                      WiFi.localIP().toString() + "/webserial");
  });

  WebSerial.begin(&server);

  server.begin();

  WebSerial.println("Connecting to MQTT broker...");
  // initializeTopics();
  // client.setServer(MQTT_BROKER, 1883);
  // client.setCallback(callback);

  mqtt.begin(mqttCallback);
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
    // if (!client.connected()) {
    //   unsigned long currentMillis = millis();
    //   if (currentMillis - lastReconnectAttempt > 5000) {
    //     lastReconnectAttempt = currentMillis;
    //     if (reconnect()) {
    //       lastReconnectAttempt = 0;
    //       ledIndicator.turnOn(1000);
    //     } else {
    //       WebSerial.print("failed, rc=");
    //       WebSerial.print(client.state());
    //       WebSerial.println(" try again in 5 seconds");
    //       ledIndicator.turnOff();
    //     }
    //   }
    // } else {
    //   client.loop();
    // }
    mqtt.loop();
    ws2812fx->service();
    resetDetector.loop();
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
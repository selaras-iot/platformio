#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include <WS2812FX.h>
#include <WebSerial.h>
#include <config.h>

AsyncWebServer server(80);
WiFiClient wifiClient;
PubSubClient client(wifiClient);
WS2812FX ws2812fx = WS2812FX(44, 5, NEO_GRB + NEO_KHZ800);

void callback(char* topic, byte* payload, unsigned int length) {
  WebSerial.print("Message arrived [");
  WebSerial.print(topic);
  WebSerial.print("] ");
  for (int i = 0; i < length; i++) {
    WebSerial.print((char)payload[i]);
  }
  WebSerial.println();
  // if (strcmp(topic,
  //            ("com.rizalanggoro.selaras/" + String(DEVICE_UUID) + "/mode")
  //                .c_str()) == 0) {
  //   int mode = atoi((char*)payload);
  //   ws2812fx.setMode(mode);
  //   WebSerial.print("mode: ");
  //   WebSerial.println(mode);
  // } else if (strcmp(topic, ("com.rizalanggoro.selaras/" + String(DEVICE_UUID)
  // +
  //                           "/brightness")
  //                              .c_str()) == 0) {
  //   int brightness = atoi((char*)payload);
  //   ws2812fx.setBrightness(brightness);
  //   WebSerial.print("brightness: ");
  //   WebSerial.println(brightness);
  // }
}

boolean reconnect() {
  WebSerial.print("Attempting MQTT connection...");
  String clientId = "ESP8266Client-";
  clientId += String(random(0xffff), HEX);
  WebSerial.println(clientId);

  if (client.connect(clientId.c_str())) {
    client.subscribe(
        ("com.rizalanggoro.selaras/" + String(DEVICE_UUID) + "/mode").c_str());
    client.subscribe(
        ("com.rizalanggoro.selaras/" + String(DEVICE_UUID) + "/brightness")
            .c_str());
  }

  return client.connected();
}

// void reconnect2() {
//   while (!client.connected()) {
//     if (client.connect(clientId.c_str())) {
//       Serial.println("connected");
//       // client.publish("rdaOutTopic", "hello world");

//     } else {

//       delay(5000);
//     }
//   }
// }

void setup() {
  Serial.begin(115200);

  // connect to wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("\nWiFi connected!");
  Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());

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
  client.setServer(MQTT_BROKER, 1883);
  client.setCallback(callback);

  ws2812fx.init();
  ws2812fx.setBrightness(1);
  ws2812fx.setSpeed(1000);
  ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.start();
}

// int value = 0;
// unsigned long lastMsg = 0;
// #define MSG_BUFFER_SIZE (50)
// char msg[MSG_BUFFER_SIZE];

unsigned long lastReconnectAttempt = 0;
unsigned long lastStatsPrint = 0;

void loop() {
  if (!client.connected()) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = currentMillis;
      if (reconnect())
        lastReconnectAttempt = 0;
      else {
        WebSerial.print("failed, rc=");
        WebSerial.print(client.state());
        WebSerial.println(" try again in 5 seconds");
      }
    }
  } else
    client.loop();

  // unsigned long now = millis();
  // if (now - lastMsg > 2000) {
  //   lastMsg = now;
  //   ++value;
  //   snprintf(msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
  //   Serial.print("Publish message: ");
  //   Serial.println(msg);
  //   client.publish("rdaOutTopic", msg);
  // }

  ws2812fx.service();

  // Print every 2 seconds (non-blocking)
  if ((unsigned long)(millis() - lastStatsPrint) > 2000) {
    WebSerial.print(F("IP address: "));
    WebSerial.println(WiFi.localIP());
    WebSerial.printf("Uptime: %lums\n", millis());
    WebSerial.printf("Free heap: %u\n", ESP.getFreeHeap());
    lastStatsPrint = millis();
  }

  WebSerial.loop();
}
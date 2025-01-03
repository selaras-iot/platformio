#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WS2812FX.h>
#include <config.h>

WiFiClient wifiClient;
PubSubClient client(wifiClient);
WS2812FX ws2812fx = WS2812FX(44, 5, NEO_GRB + NEO_KHZ800);

void callback(char* topic, byte* payload, unsigned int length) {
  // Serial.print("Message arrived [");
  // Serial.print(topic);
  // Serial.print("] ");
  // for (int i = 0; i < length; i++) {
  //   Serial.print((char)payload[i]);
  // }
  // Serial.println();
  if (strcmp(topic,
             ("com.rizalanggoro.selaras/" + String(DEVICE_UUID) + "/mode")
                 .c_str()) == 0) {
    int mode = atoi((char*)payload);
    ws2812fx.setMode(mode);
    Serial.print("mode: ");
    Serial.println(mode);
  } else if (strcmp(topic, ("com.rizalanggoro.selaras/" + String(DEVICE_UUID) +
                            "/brightness")
                               .c_str()) == 0) {
    int brightness = atoi((char*)payload);
    ws2812fx.setBrightness(brightness);
    Serial.print("brightness: ");
    Serial.println(brightness);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    Serial.println(clientId);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // client.publish("rdaOutTopic", "hello world");
      client.subscribe(
          ("com.rizalanggoro.selaras/" + String(DEVICE_UUID) + "/mode")
              .c_str());
      client.subscribe(
          ("com.rizalanggoro.selaras/" + String(DEVICE_UUID) + "/brightness")
              .c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

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

  Serial.println("Connecting to MQTT broker...");
  client.setServer(MQTT_BROKER, 1883);
  client.setCallback(callback);

  ws2812fx.init();
  ws2812fx.setBrightness(100);
  ws2812fx.setSpeed(1000);
  ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);
  ws2812fx.start();
}

// int value = 0;
// unsigned long lastMsg = 0;
// #define MSG_BUFFER_SIZE (50)
// char msg[MSG_BUFFER_SIZE];

void loop() {
  if (!client.connected()) reconnect();
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
}
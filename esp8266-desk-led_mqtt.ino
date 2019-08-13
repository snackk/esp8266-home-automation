#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* WIFI_SSID = "snackkster";
const char* WIFI_PASSWORD = "maybegofuckyourself026";

const char* MQTT_SERVER = "farmer.cloudmqtt.com";
const char * MQTT_USERNAME = "unchrdkg";
const char * MQTT_PASSWORD = "DKFrYaiG5nEX";
const String MQTT_HOSTNAME  = "snackkmedia-iot";
const char * MQTT_COMMAND_TOPIC = "wol/set";
const int MQTT_PORT = 18345;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(String("\nConnecting to WiFi network: (") + WIFI_SSID + ")" );
  }
  Serial.println(String("\nConnected to WiFi network: (") + WIFI_SSID + ")" );

  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(onMessage);
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {
    if (checkMqttConnection()) {
      client.loop();
    }
  }
}

bool checkMqttConnection() {
  if (!client.connected()) {
    if (client.connect(MQTT_HOSTNAME.c_str(), MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("Connected to MQTT broker: " + String(MQTT_SERVER));
      client.subscribe(MQTT_COMMAND_TOPIC);
    }
  }
  return client.connected();
}

void onMessage(char* topic, byte* payload, unsigned int length) {
  String payloadStr = "";
  for (int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }
  String message = String(payloadStr);

  Serial.println(String("\nMessage arrived in topic: (") + topic + "), with payload: " +  message);
  Serial.println("-----------------------");
}

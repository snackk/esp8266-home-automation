#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define WOL 12

const char* WIFI_SSID = "ssid";
const char* WIFI_PASSWORD = "pass";

const char* MQTT_SERVER = "mqtt";
const char * MQTT_USERNAME = "user";
const char * MQTT_PASSWORD = "pass";
const String MQTT_HOSTNAME  = "snackkmedia-iot";
const char * MQTT_COMMAND_TOPIC = "topic";
const int MQTT_PORT = 18345;

const int ALIVE_MILIS = 300;

//NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600);

WiFiClient wifiClient;
PubSubClient client(wifiClient);

unsigned long startTime;

bool isDeepSleepEnabled = false;

void setup() {
  startTime = micros();

  Serial.begin(115200);

  pinMode(WOL, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println(String("\nConnecting to WiFi network"));
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print('.');
  }
  Serial.println(String("\nConnected to WiFi network: (") + WIFI_SSID + ")" );

  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(onMessage);

  timeClient.begin();
}

void loop() {
  if(isDeepSleepEnabled) {
    unsigned long deltaTime = (micros() - startTime) / 1000000;
    if(deltaTime < ALIVE_MILIS) {
      mqqtLoop();
    } else {
      Serial.println("Going into deep sleep for 2 minutes.");
      client.disconnect();
      WiFi.disconnect();
      ESP.deepSleep(120e6);
    }
  } else {
    mqqtLoop();
  }
}

void mqqtLoop() {
  if(WiFi.status() == WL_CONNECTED) {
    if (checkMqttConnection()) {
      client.loop();
    }
  } else {
    Serial.println("Connection with MQTT broker: " + String(MQTT_SERVER) + ", failed! rc=");
    Serial.print(client.state());
  }
}

bool checkMqttConnection() {
  if (!client.connected()) {
    if (client.connect(MQTT_HOSTNAME.c_str(), MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("Connected to MQTT broker: " + String(MQTT_SERVER));
      client.subscribe(MQTT_COMMAND_TOPIC);
      Serial.println("Subscribing MQTT topic: " + String(MQTT_COMMAND_TOPIC));
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

  if(message.equals("W")) {
    pushPwr();
    timeClient.update();
    String messageToPublish = "WOL command received at: " + timeClient.getFormattedTime();
    client.publish(MQTT_COMMAND_TOPIC, messageToPublish.c_str(), true);  //nasty hack: last retained message must be != W
  }
  if(message.equals("D")) {
    isDeepSleepEnabled != isDeepSleepEnabled;
    timeClient.update();
    String messageToPublish = "DeepSleep command received at: " + timeClient.getFormattedTime();
    client.publish(MQTT_COMMAND_TOPIC, messageToPublish.c_str(), true);  //nasty hack: last retained message must be != W
  }
}

void pushPwr() {
  pinMode(WOL, OUTPUT);
  digitalWrite(WOL, LOW);
  delay(300);
  pinMode(WOL, INPUT);
}

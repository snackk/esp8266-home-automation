#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define WOL 20

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

const int ESP_BUILTIN_LED = 2;

void setup() {
  startTime = micros();

  Serial.begin(115200);

  pinMode(WOL, INPUT);

  //ALWAYS ON LED
  //pinMode(ESP_BUILTIN_LED, OUTPUT);

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

  //OTA
  //ArduinoOTA.setPort(8266);
  //ArduinoOTA.setHostname("myesp8266");
  //ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
}

void loop() {
  if(isDeepSleepEnabled) {
    unsigned long deltaTime = (micros() - startTime) / 1000000;
    if(deltaTime < ALIVE_MILIS) {
      mqqtLoop();
      ArduinoOTA.handle();
    } else {
      Serial.println("Going into deep sleep for 2 minutes.");
      client.disconnect();
      WiFi.disconnect();
      ESP.deepSleep(120e6);
    }
  } else {
    mqqtLoop();
    ArduinoOTA.handle();
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

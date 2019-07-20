#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <WebSocketsServer.h>

ESP8266WiFiMulti wifiMulti;

ESP8266WebServer server(80);
WebSocketsServer webSocket(81);

File fsUploadFile;

const char *ssid = "led-lamp";
const char *password = "test1234";

const char *OTAName = "esp8266";
const char *OTAPassword = "esp8266";

#define LED_RED     14
#define LED_GREEN   12
#define LED_BLUE    13
#define WOL         15

const char* mdnsName = "led-lamp";

// SETUP
void setup() {
  //LED PIN SETUP
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  //WOL PIN SETUP
  pinMode(WOL, OUTPUT);
  digitalWrite(WOL, LOW);

  Serial.begin(115200);
  delay(10);
  Serial.println("\r\n");

  startWiFi();

  startOTA();

  startSPIFFS();

  startWebSocket();

  startMDNS();

  startServer();
} // END SETUP

// LOOP
bool rainbow = false;

unsigned long prevMillis = millis();
int hue = 0;

void loop() {
  webSocket.loop();                           // check for websocket events
  server.handleClient();                      // run the server
  ArduinoOTA.handle();                        // listen for OTA events

  if(rainbow) {
    if(millis() > prevMillis + 32) {
      if(++hue == 360) {                      // cycle through the color wheel every 32 ms
        hue = 0;
      }
      setHue(hue);
      prevMillis = millis();
    }
  }
} // END LOOP

// SETUP
void startWiFi() {
  WiFi.softAP(ssid, password);
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" started.\r\n");

  wifiMulti.addAP("ssid_from_AP_1", "your_password_for_AP_1");
  wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
  wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

  Serial.print("Connecting to a WIFI station ");
  while (wifiMulti.run() != WL_CONNECTED && WiFi.softAPgetStationNum() < 1) {
    delay(250);
    Serial.print('.');
  }
  Serial.println("\r\n");

  if(WiFi.softAPgetStationNum() == 0) {      // esp is connected to an AP
    Serial.print("Connected to \"");
    Serial.print(WiFi.SSID());
    Serial.println(""\".");
    Serial.print("IP address:\t");
    Serial.print(WiFi.localIP());
  } else {                                   // station is connected to the esp AP
    Serial.print("Station connected to esp8266 AP");
  }
  Serial.println("\r\n");
}

void startOTA() {
  ArduinoOTA.setHostname(OTAName);
  ArduinoOTA.setPassword(OTAPassword);

  ArduinoOTA.onStart([]() {
    Serial.println("Start.");
    // turn off LEDs
    digitalWrite(LED_RED, 0);
    digitalWrite(LED_GREEN, 0);
    digitalWrite(LED_BLUE, 0);
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\r\nEnd.");
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
  Serial.println("OTA ready.\r\n");
}

void startSPIFFS() {
  SPIFFS.begin();
  Serial.println("SPIFFS started.");
  Serial.println("Files:");
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("\t%s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }
}

void startWebSocket() {
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
  Serial.println("WebSocket server started.");
}

void startMDNS() {
  MDNS.begin(mdnsName);
  Serial.print("mDNS started at http://");
  Serial.print(mdnsName);
  Serial.println(".local");
}

void startServer() {
  server.on("/upload.html",  HTTP_POST, []() {
    server.send(200, "text/plain", "");
  }, handleFileUpload);

  server.onNotFound(handleFileRequest);        // check if the file exists in the flash memory (SPIFFS), if so, send it

  server.begin();
  Serial.println("HTTP server started.");
  Serial.println("-------------------");
} // END SETUP

// SERVER HANDLERS
void handleFileRequest() {
  if(!handleFileRead(server.uri())){
    server.send(404, "text/plain", "404: File not found");
  }
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected.\n", num);
      break;

    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d, requested url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        rainbow = false;
      }
      break;

    case WStype_TEXT:
      Serial.printf("[%u] GET payload: %s\n", num, payload);
      handlePayload(payload);
      break;
  }
}

void handlePayload(uint8_t * payload) {
  switch(payload[0]) {
    case '#':
      uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);
      int r = ((rgb >> 20) & 0x3FF);                     // 10 bits per color, so R: bits 20-29
      int g = ((rgb >> 10) & 0x3FF);                     // G: bits 10-19
      int b = rgb & 0x3FF;                               // B: bits  0-9

      analogWrite(LED_RED, r);
      analogWrite(LED_GREEN, g);
      analogWrite(LED_BLUE, b);
      break;

    case 'R':
      rainbow = true;
      break;

    case 'N':
      rainbow = false;
      break;

    case 'W':
      digitalWrite(WOL, HIGH);
      delay(300);
      digitalWrite(WOL, LOW);
      break;
  }
} // END SERVER HANDLERS

// FILE SYSTEM
bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) {
    path += "index.html";
  } else if (path.endsWith("upload") || path.endsWith("wol")) {
    path += ".html";
  }
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz)) {
      path += ".gz";
    }
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile not found: ") + path);
  return false;
}

void handleFileUpload() {
  HTTPUpload& upload = server.upload();
  String path;
  if(upload.status == UPLOAD_FILE_START) {
    path = upload.filename;
    if(!path.startsWith("/")) {
      path = "/" + path;
    }
    if(!path.endsWith(".gz")) {
      String pathWithGz = path + ".gz";
      if(SPIFFS.exists(pathWithGz)) {
         SPIFFS.remove(pathWithGz);
      }
    }
    Serial.print("handleFileUpload Name: ");
    Serial.println(path);
    fsUploadFile = SPIFFS.open(path, "w");
    path = String();
  } else if(upload.status == UPLOAD_FILE_WRITE) {
    if(fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
    }
  } else if(upload.status == UPLOAD_FILE_END) {
    if(fsUploadFile) {
      fsUploadFile.close();
      Serial.print("handleFileUpload Size: ");
      Serial.println(upload.totalSize);
      server.sendHeader("Location","/success.html");
      server.send(303);
    } else {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }
} // END FILE SYSTEM

// GENERIC
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
}

String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
} // END GENERIC

// LED RGB
void setHue(int hue) { // Set the RGB LED to a given hue (color) (0° = Red, 120° = Green, 240° = Blue)
  hue %= 360;                   // hue is an angle between 0 and 359°
  float radH = hue*3.142/180;   // Convert degrees to radians
  float rf, gf, bf;

  if(hue>=0 && hue<120){        // Convert from HSI color space to RGB
    rf = cos(radH*3/4);
    gf = sin(radH*3/4);
    bf = 0;
  } else if(hue>=120 && hue<240){
    radH -= 2.09439;
    gf = cos(radH*3/4);
    bf = sin(radH*3/4);
    rf = 0;
  } else if(hue>=240 && hue<360){
    radH -= 4.188787;
    bf = cos(radH*3/4);
    rf = sin(radH*3/4);
    gf = 0;
  }
  int r = rf*rf*1023;
  int g = gf*gf*1023;
  int b = bf*bf*1023;

  analogWrite(LED_RED, r);
  analogWrite(LED_GREEN, g);
  analogWrite(LED_BLUE, b);
} // END LED RGB

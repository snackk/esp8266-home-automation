/*
 * Server on: 192.168.4.1
 * Add this to Arduino preferences: http://arduino.esp8266.com/stable/package_esp8266com_index.json
 */
#include <ESP8266WiFi.h>

const char AP_NameChar[] = "LedLamp" ;
const char WiFiPassword[] = "12345678";

WiFiServer server(80);

String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

String html = "<!DOCTYPE html>";
html += "<html>";
html += "<head>";
html += "   <meta charset='utf-8'>";
html += "   <meta http-equiv='X-UA-Compatible' content='IE=edge'>";
html += "   <meta name='viewport' content='width=device-width, initial-scale=1'>";
html += "   <link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css'>";
html += "   <script src='https://maxcdn.bootstrapcdn.com/bootstrap/4.1.3/js/bootstrap.min.js'></script>";
html += "   <script src='https://cdnjs.cloudflare.com/ajax/libs/jscolor/2.0.4/jscolor.min.js'></script>";
html += "   <style>body {font-size:140%;} #main {display: table; margin: auto;  padding: 0 10px 0 10px; } h2,{text-align:center; } .button { padding:10px 10px 10px 10px; width:100%;  background-color: #4CAF50; font-size: 120%;}</style>";
html += "   <title>LED Lamp</title>";
html += "</head>";
html += "<body>";
html += "   <div class='container' id='main'>";
html += "       <h2>Choose the LED color</h2>";
html += "       <a type='submit' id='change_color' type='button' class='btn btn-primary'>Change it!</a>";
html += "       <input class='jscolor {onFineChange:'update(this)'}' id='rgb'>";
html += "   </div>";
html += "   <script>";
html += "     function update(picker) {";
html += "       document.getElementById('rgb').innerHTML = Math.round(picker.rgb[0]) + ', ' +  Math.round(picker.rgb[1]) + ', ' + Math.round(picker.rgb[2]);";
html += "       document.getElementById('change_color').href='?r=' + Math.round(picker.rgb[0]*4.0117) + '&g=' +  Math.round(picker.rgb[1]*4.0117) + '&b=' + Math.round(picker.rgb[2]*4.0117);";
html += "     }";
html += "   </script>";
html += "</body>";
html += "</html>";

// TODO Deprecate this...
String html_1 = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'/><meta charset='utf-8'><style>body {font-size:140%;} #main {display: table; margin: auto;  padding: 0 10px 0 10px; } h2,{text-align:center; } .button { padding:10px 10px 10px 10px; width:100%;  background-color: #4CAF50; font-size: 120%;}</style><title>LED Control</title></head><body><div id='main'><h2>LED Control</h2>";
String html_2 = "";
String html_4 = "</div></body></html>";

String request = "";
//int RED = 14;
//int GREEN = 12;
//int BLUE = 13;

//TODO Deprecate this...
int LED_Pin = 14;

void setup() {
    pinMode(LED_Pin, OUTPUT);

    boolean conn = WiFi.softAP(AP_NameChar, WiFiPassword);
    server.begin();
}

void loop() {
    // Check if a client has connected
    WiFiClient client = server.available();
    if (!client) {
      return;
    }

    // Read the first line of the request
    request = client.readStringUntil('\r');

    if (request.indexOf("LEDON") > 0) {
      digitalWrite(LED_Pin, HIGH);
    } else if (request.indexOf("LEDOFF") > 0) {
       digitalWrite(LED_Pin, LOW);
     }

    // Get the LED pin status and create the LED status message
    if (digitalRead(LED_Pin) == HIGH) {
        // the LED is on so the button needs to say turn it off
        html_2 = "<form id='F1' action='LEDOFF'><input class='button' type='submit' value='Turn of the LED' ></form><br>";
    } else {
        // the LED is off so the button needs to say turn it on
        html_2 = "<form id='F1' action='LEDON'><input class='button' type='submit' value='Turn on the LED' ></form><br>";
    }

    client.flush();

    client.print( header );
    client.print( html_1 );
    client.print( html_2 );
    client.print( html_4);

    delay(5);
  // The client will actually be disconnected when the function returns and 'client' object is detroyed
}

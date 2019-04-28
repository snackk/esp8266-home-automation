/*
 * Server on: 192.168.4.1
 * Add this to Arduino preferences: http://arduino.esp8266.com/stable/package_esp8266com_index.json
 */
#include <ESP8266WiFi.h>

const char AP_NameChar[] = "LedLamp" ;
const char WiFiPassword[] = "12345678";

WiFiServer server(80);

String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

String html ="<!DOCTYPE html> <html lang=\"en\"> <head> <meta charset=\"utf-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\"> <title>Led Lamp</title> <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\" integrity=\"sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T\" crossorigin=\"anonymous\"> </head> <body> <div class=\"container\"> <h2>Choose Led color</h2> <form> <input name=\"color\" type=\"hidden\" onchange=\"update(this.value)\" id=\"color_value\" value=\"99cc00\"> <button class=\"jscolor {valueElement: 'color_value'}\">Pick a color</button> <a type=\"submit\" id=\"change_color\" type=\"button\" class=\"btn btn-primary\">Change it!</a> </form> </div> <script src=\"https://code.jquery.com/jquery-3.1.1.slim.min.js\" integrity=\"sha384-A7FZj7v+d/sdmMqp/nOQwliLvUsJfDHW+k9Omg/a/EheAdgtzNs3hpfag6Ed950n\" crossorigin=\"anonymous\"></script> <script src=\"https://cdnjs.cloudflare.com/ajax/libs/jscolor/2.0.4/jscolor.min.js\"></script> <script> function update (picker) { var rgb = HexToRgb(picker); var red = rgb[0]; var green = rgb[1]; var blue = rgb[2]; document.getElementById(\"change_color\").href='?r=' + red + '&g=' + green + '&b=' + blue; } function HexToRgb (hex) { 'use strict'; if (hex.charAt(0) === '#') { hex = hex.substr(1); } if ((hex.length < 2) || (hex.length > 6)) { return false; } var values = hex.split(''), r, g, b; if (hex.length === 2) { r = parseInt(values[0].toString() + values[1].toString(), 16); g = r; b = r; } else if (hex.length === 3) { r = parseInt(values[0].toString() + values[0].toString(), 16); g = parseInt(values[1].toString() + values[1].toString(), 16); b = parseInt(values[2].toString() + values[2].toString(), 16); } else if (hex.length === 6) { r = parseInt(values[0].toString() + values[1].toString(), 16); g = parseInt(values[2].toString() + values[3].toString(), 16); b = parseInt(values[4].toString() + values[5].toString(), 16); } else { return false; } return [r, g, b]; } </script> </body> </html>";

String request = "";
int RED_PIN = 14;
int GREEN_PIN = 12;
int BLUE_PIN = 13;

void setup() {
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);

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

    //Parse data HERE

     analogWrite(RED_PIN, 0);
     analogWrite(GREEN_PIN, 0);
     analogWrite(BLUE_PIN, 0);

    client.flush();

    client.print(header);
    client.print(html);

    delay(5);
}

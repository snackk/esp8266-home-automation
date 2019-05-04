### ESP8266 led strip with WOL support

## Overview

This project may seam a bit odd, to mix both led strip controller and WOL (Wake on Lan). Basically I needed to have WOL on my computer, but given that it's connected via wireless, there was no way to transmit the magic packet via wireless, so I simply connected the esp8266 to the power switch of the computer to be able to turn it on remotly.

## Prerequisites

* ESP8266 nodemcu (duh?)
* Install ESP8266 on Arduino IDE (http://arduino.esp8266.com/stable/package_esp8266com_index.json)
* WebSocket library for Arduino [arduinoWebSockets](https://github.com/Links2004/arduinoWebSockets)
* ESP8266 file system uploader [arduino-esp8266fs-plugin](https://github.com/esp8266/arduino-esp8266fs-plugin)

## Documentation

This project contains several parts, naming [WIFI](#wifi), [OTA](#ota), [SPIFFS](#spiffs), [WebSocket](#websocket), [mDNS](#mdns).

Any issue flashing the esp8266 should refer to the [problems](#problems) section.

### <a name="wifi"></a> WIFI

The wifi setup creates a single AP and tries to connect to several pre-defined stations.

### <a name="ota"></a> OTA

It's possible to do OTA (Over The Air updates) instead of flashing the code on the Arduiono IDE.

### <a name="spiffs"></a> SPIFFS

File System.

### <a name="websocket"></a> WebSocket

WebSocket

### <a name="mdns"></a> mDNS

mDNS

## <a name="problems"></a> Problems

Most of the problems I faced while flashing the esp8266 nodemcu was due to wrong configuration on the Arduino IDE, that or I haven't set the esp8266 to flash mode.
 
  Written by [@snackk](https://github.com/snackk)

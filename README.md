### ESP8266 Home Automation

## Overview

This project may seem a bit odd, to mix both led strip controller and a remote desktop power switch, that acts like WOL (Wake on Lan). So basically my desk computer is not connected via lan, therefore there's no way to transmit the magic packet via wireless. So I thought on having a simple esp8266 that could serve as an external actuator to remotely turn on the computer, and also to control the led strip.

## Prerequisites

* ESP8266 nodemcu (duh?)
* Install ESP8266 on Arduino IDE [esp8266](http://arduino.esp8266.com/stable/package_esp8266com_index.json)
* WebSocket library for Arduino [arduinoWebSockets](https://github.com/Links2004/arduinoWebSockets)
* ESP8266 file system uploader [arduino-esp8266fs-plugin](https://github.com/esp8266/arduino-esp8266fs-plugin)

## Documentation

This project contains several parts, naming [WIFI](#wifi), [OTA](#ota), [SPIFFS](#spiffs), [WebSocket](#websocket), [mDNS](#mdns).

Any issue flashing the esp8266 should refer to the [problems](#problems) section.

### <a name="wifi"></a> WIFI

The wifi setup creates a single AP and tries to connect to several pre-defined stations.

### <a name="ota"></a> OTA

It's possible to do OTA (Over The Air updates), instead of flashing the code locally on the Arduino IDE.

### <a name="spiffs"></a> SPIFFS

A File System capable of storing several files. This is used to serve HTML pages, files can be stored in a compressed to save some space.

### <a name="websocket"></a> WebSocket

WebSocket

### <a name="mdns"></a> mDNS

mDNS

## <a name="problems"></a> Problems

Most of the problems I faced while flashing the esp8266 nodemcu was due to wrong configuration on the Arduino IDE, that or I haven't set the esp8266 to flash mode.

  Written by [@snackk](https://github.com/snackk)

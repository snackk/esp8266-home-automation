### ESP8266 Home Automation

## Overview

This repository contains several IoT firmwares for esp8266 based hardware.

### Wake on Lan
IoT device to remotly power up a computer. This is particular useful for wireless connected computers, whose wireless card don't support the **wol** magic packet. Esp8266 acts as a MQTT client that consumes **wake up** events from a given topic, in this case 'wol/push', and shorts the power switch circuit for a brief amount of time, emulating the button switch, switching on/off the computer.

<img width="451" alt="Screenshot 2020-01-27 at 22 44 09" src="https://user-images.githubusercontent.com/9936714/73220573-a9346e80-4156-11ea-9cc3-16674fdf1ef2.png">

### Led Stripe
Led Stripe RGB activated by an IoT device. Esp8266 acts as a MQTT client that consumes **light events** from a given topic, in this case 'lights/push' lightning the led stripe to match the chosen color.

<img width="451" alt="Screenshot 2020-01-27 at 22 44 09" src="https://user-images.githubusercontent.com/9936714/73220573-a9346e80-4156-11ea-9cc3-16674fdf1ef2.png">


## Prerequisites

* esp8266 nodemcu
* esp8266 boards for Arduino IDE [esp8266](http://arduino.esp8266.com/stable/package_esp8266com_index.json)
* WebSocket library for Arduino [arduinoWebSockets](https://github.com/Links2004/arduinoWebSockets)
* esp8266 file system uploader [arduino-esp8266fs-plugin](https://github.com/esp8266/arduino-esp8266fs-plugin)

## Documentation

This projects might be composed on several other modules, namely [WIFI](#wifi), [OTA](#ota), [SPIFFS](#spiffs), [WebSocket](#websocket), [mDNS](#mdns).

Any issues flashing the esp8266 should refer to the [problems](#problems) section.

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

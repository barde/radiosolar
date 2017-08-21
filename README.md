# RadioSolar

## Power Independent Measurement of Radiation by ESP8266

### Why

Proof of concept to make a low power device with the ESP8266.
Maybe some help for makers who don't want to spend their time debugging and searching through 
lots of forum posts.

### How

+ Get the hardware.
+ Clone repository.
+ Make your own credentials.h with SSID, Wifi Passwort and your Thinkspeak configuration.

### Features

+ synchronous wait only for WiFi syscalls. Yes, they are mandatory.
+ collection of geiger counts via interrupt
+ ESP8266 Modem Sleep for power preservation
+ logging of battery charge and discharge
+ MQTT support for thinkspeak

### Hardware

+ ESP8266
+ Sparkfun Geiger Counter
+ INA219 Current/Voltage sensor
+ Chinese solar panel
+ charge controller
+ Batteries
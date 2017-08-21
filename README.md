# RadioSolar

![](doc/logo.png?raw=true)

### Why

Proof of concept to make a low power device with the ESP8266.
Maybe some help for makers who don't want to spend their time debugging and searching through 
lots of forum posts.

### How

+ Get the hardware
  + some ESP8266
  + some tube with CPM output to a digital pin of the ESP
  + INA219
  + power supply
    + this experimental setup uses a solar panel and a battery
    + you could use a simple USB charger or any other stationary power supply
+ Clone repository
+ Create your credentials.h

#### credentials.h

Save as credentials.h in the src directory.

```cpp
unsigned long myChannelNumber = 123456;
const char * myWriteAPIKey = "DEADBEEF123";

const char* ssid = "mySSID";
const char* password = "HopefullySomethingSensible";
char wifiHostname[] = "radiosolar";

const char* radmonUsername = "myRadmonUser";
const char* radmonPassword = "LetTheAdminGenerateIt";
```

### Features

+ synchronous wait only for WiFi syscalls. Yes, they are [mandatory](http://www.esp8266.com/viewtopic.php?p=38984&sid=e092a19d9806be5b6415ccd3439251ec#p38984).
+ collection of geiger counts per minute via interrupt
+ mean averaging to preserve power by turning the wifi on only every 15 minutes
+ ESP8266 Modem Sleep (=Wifi turned off) for power preservation
+ logging of battery charge and discharge
+ MQTT support for Thingspeak as IoT data sink

### Hardware in this setup

+ Wemos D1 Mini
+ Sparkfun Geiger Counter
+ INA219 Current/Voltage sensor
+ 2 serial 6W solar panels with each bypass diodes on each terminals interconnected with a series diode
+ all diodes are LX2410A by MicroSemi
+ BQ24650 solar charge controller
+ Polulu 5V step down with >98% efficiency
+ 2s2p 8.4 V 4 Ah Lithium Ion battery

#### Power considerations

Well, probably some numbers are required for getting the correct value of solar panel and battery sizes.

This setup contains a voltage and current sensor for monitoring the battery state. Also useful as sunshine sensor.

A switching power supply in combination with a low power optimized solar charging control is recommended. MPPT is 
advised. My setup uses a 2p2s Lithium Ion combination with the BQ24650 from TI.

### Schema

![](doc/schema.png?raw=true)

### Whacky prototype

Pic tbd. Still too whacky. Really.

### Possible improvements

+ remove the Atmega Microcontroller on the Sparkfun Geiger Board as we don't need it
+ if we leave it we could at least let it collect the mean value and deep sleep the esp completely
+ remove useless onboard LED
+ custom PCB with only necessary components (no LEDs, no Serial to USB chips, no other power drains)
+ if we get under 1mA we could try power harvesting stuff as getting just the environment heat to power the device

### Demo

[Radmon worldwide map](http://radmon.org/)

[Radmon profile](http://www.radmon.org/radmon.php?function=showuserpage&user=clms)

[Thingspeak raw data](https://thingspeak.com/channels/305931)

![](doc/logoEnd.png?raw=true)
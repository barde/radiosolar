# RadioSolar

![](doc/logo.png?raw=true)

## End of life

:exclamation: **This project is now discontinued** :exclamation:

:exclamation: **More featured radio logger on ESP8622 platform is [Radmon](https://sites.google.com/site/diygeigercounter/gk-radmon)** :exclamation:


:cloud: **_A "sell computing power for randomness" is offered as a service at https://rng.cloud_** :cloud:

### Why

Proof of concept to make a low power device with the ESP8266.
Mostly the low power functionality of the ESP8266 is heavily used such as the "modem sleep" and the "light sleep".

The battery load and charge is logged which also provides the sunshine time. This is used for making the proper size for the
needed battery capacity and solar panel size. 

Later on the desire for additional functionality lead to the inclusion of the well known "randomness by radioactive decay time period differences" algorithm and
later on some back-end.

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
+ Compile and flash the code with either the Arduino IDE, PlatformIO or the Espressif IDF

#### Recommended tooling

Install [PlatformIO](http://platformio.org/) - most comfortably from the extension manager in [Visual Studio Code](https://code.visualstudio.com/).
Select your board and use the initialization command. Restart the IDE on every dialog requesting you to do so until no more dialogs appear. 
Connect your device and start the upload task. You should be ready. 

#### credentials.h

Save as credentials.h in the src directory and fill in your data.

```cpp
unsigned long myChannelNumber = 123456;
const char * myWriteAPIKey = "DEADBEEF123";

const char* ssid = "mySSID";
const char* password = "HopefullySomethingSensible";
char wifiHostname[] = "radiosolar";

const char* radmonUsername = "myRadmonUser";
const char* radmonPassword = "LetTheAdminGenerateIt";

const char* otaHostname = "myServer";
const int otaPort     = 8887;
const char* otaUri    = "esp.cgi";
const char* firmwareVersion = "0.1";

const char* trueRngEndpoint = "https://example.com/Data";
const char* trueRngSecret = "ABCD123";
```

### Features

+ synchronous wait only for WiFi syscalls. Yes, they are [mandatory](http://www.esp8266.com/viewtopic.php?p=38984&sid=e092a19d9806be5b6415ccd3439251ec#p38984).
+ collection of geiger counts per minute via interrupt
+ mean averaging to preserve power by turning the wifi on only every 15 minutes
+ ESP8266 Modem Sleep (=Wifi turned off) for power preservation
+ logging of battery charge and discharge
+ MQTT support for Thingspeak as IoT data sink
+ generates slowly albeit constantly true random numbers and posts them to your own API endpoint
+ Posting of radiation data to distributed map service Radmon

### Hardware in this setup

+ Wemos D1 Mini
+ Sparkfun Geiger Counter
+ INA219 Current/Voltage sensor
+ 2 serial 6W solar panels with each bypass diodes on each terminals interconnected with a series diode
+ all diodes are LX2410A by MicroSemi
+ BQ24650 solar charge controller
+ Polulu 5V step down with >98% efficiency
+ 2s2p 8.4 V 4 Ah Lithium Ion battery with a BMS and protection

#### Power considerations

Well, probably some numbers are required for getting the correct value of solar panel and battery sizes.

This setup contains a voltage and current sensor for monitoring the battery state. Also useful as sunshine sensor.

A switching power supply in combination with a low power optimized solar charging control is recommended. MPPT is 
advised. My setup uses a 2p2s Lithium Ion combination with the BQ24650 from TI.

### Schema

![](doc/schema.png?raw=true)

### Prototype

<img width="500" alt="prototype with case" src="doc/radiosolar_prototype.jpg?raw=true">

#### Case

This project comes with ~~batteries~~ [a 3D printed case](../master/doc/case.stl) included.

### Possible improvements

Most improvements are only usable in a production scenario. As this setup is experimental all 
changes below hinder possible bug fixing and development.

+ remove the Atmega micro controller on the Sparkfun Geiger Board as we don't need it
+ if we leave it we could at least let it collect the mean value and deep sleep the esp completely
+ remove useless onboard LED
+ turn off the Serial-USB chip on the Wemos D1 (leaving the D+ and D- USB lines open does not change the current)
+ custom PCB with only necessary components (no LEDs, no Serial to USB chips, no other power drains)
+ if we get under 1mA we could try power harvesting stuff as getting just the environment heat to power the device

### Random Numbers

This setup also generates true random numbers. The algorithm is a derived form of John Walker's 
[random number generator](https://sites.google.com/site/astudyofentropy/project-definition/radioactive-decay).
It posts a full unsigned 32 bit integer when enough entropy has been collected.

The collection of all collected randomness is available to the IoT data sink page. Every bit is truly random and has no 
connection to any previous or following values.

#### Example Usage

The [example](../master/example/) subdirectory contains sample code for using true randomness in your application.

### Demo

+ [Radmon worldwide map](http://radmon.org/)

+ [Radmon profile](http://www.radmon.org/radmon.php?function=showuserpage&user=clms)

+ [Thingspeak raw data](https://thingspeak.com/channels/305931)
  + The raw data includes the battery management overview. The voltage and current draw/charge is logged.

### Known Issues

While charging the CPM count increases nearly by factor two. This is probably caused by the increased V+ voltage 
from the switching power regulator. A possible fix would be a cleaner 5V source or a LDO after the switching output.

![](doc/chargingIssue.png?raw=true)

### Kudos

+ [Phialo 3D Design](https://phialo.de) for the 3D sun model, 3D-printing the enclosure and general support
+ Adafruit, Arduino and Thingspeak for their great libraries, data sink and general awesomeness
+ Sparkfun and Espressif for their neat hardware
+ [Radmon](http://radmon.org/) for their nice map of Geiger counter values. Check it out!
+ all the ingenious and industrious tinkerers who helped to pave my way into the depths of the ESP8266

![](doc/logoEnd.png?raw=true)

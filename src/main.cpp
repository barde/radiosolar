/**
 * Radiosolar by Bartholomäus Dedersen
 * 2017
 *
 * Sends a converted CPM count from a Geiger Miller tube to a websink for IoT data logging.
 * Optimized for ESP8266 low power usage by switching the wifi only on when needed. Default is a 
 * push of the average for 15 minutes. The data is converted to a rough estimation in micro Sievert per hour.
 */
#include "Arduino.h"
#include <ESP8266WiFi.h>    
extern "C" {
  #include "user_interface.h"
  #include "gpio.h"  
}
#include <ThingSpeak.h> 
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <ESP8266HTTPClient.h>

// include your own credentials in this file
#include "credentials.h"    

// the out pin of the geiger tube is connected via transistor to this pin of the esp8266
int gm_action_pin = D8;

// make a mean value of multiple counts. default is 15 minutes.
const byte reportsMean = 15;

// --- END OF CONFIGURATION ---

// ina219 current/voltage sensor
Adafruit_INA219 ina219;

// measure for one minute to get counts per minute
const long interval = 1000 * 60;

// data cache for usvh, voltage and current
float cachedData[reportsMean];
float cachedVoltage[reportsMean];
float cachedCurrent[reportsMean];

// async waiting for data collection
unsigned long previousMillisReport = 0;

// count of already collected data
byte writtenReports = 0;

// async waiting for the push
unsigned long previousMillisRead = 0;

// counts the events per minute in the ISR
volatile int cpm_raw = 0;

// magic number for LND 712 to convert from cpm to uSv/h
const double coefficientOfConversion = 0.00812;

WiFiClient espClient;
HTTPClient http;

void connectWiFi()
{
  if(WiFi.status() == WL_CONNECTED)
  {
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  wifi_station_set_hostname(wifiHostname);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
}

void wakeupFromLightSleep()
{
  cpm_raw = cpm_raw + 1;  
  Serial.println("Woke up from sleep");
}

void enterLightSleep()
{
  Serial.println("going to light sleep...");
  wifi_station_disconnect();
  wifi_set_opmode(NULL_MODE);
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T); //light sleep mode
  gpio_pin_wakeup_enable(GPIO_ID_PIN(gm_action_pin), GPIO_PIN_INTR_HILEVEL); //set the interrupt to look for HIGH pulses on Pin 0 (the PIR).
  wifi_fpm_open();
  delay(100);
  wifi_fpm_set_wakeup_cb(wakeupFromLightSleep); //wakeup callback
  wifi_fpm_do_sleep(0xFFFFFFF); 
  delay(100);
}

void wakeWifi()
{
  wifi_fpm_close;
  wifi_set_opmode(STATION_MODE);
  wifi_station_connect();
  Serial.println("Wifi enabled");
}

float getAverage(float singleValues[])
{
  float totalAddedValue;

  for(int i = 0; i < reportsMean; i++)
  {
    totalAddedValue += singleValues[i];
  }

  return totalAddedValue / reportsMean;
}

void sendDataToRadmon(double cpm)
{
  String uri = String("http://radmon.org/radmon.php?function=submit&user=");
  uri.concat(String(radmonUsername) + String("&password=") + String(radmonPassword));
  uri.concat(String("&value=") + String(cpm,0) + String("&unit=CPM"));

  Serial.print("RadmonString:");
  Serial.println(uri);

  http.begin(uri); //HTTP
  
  int httpCode = http.GET();

  if(httpCode == HTTP_CODE_OK) 
  {
    Serial.println("Data sent to Radmon");
  }
  else 
  {
    Serial.println("Error sending data to Radmon");
  }
  
  http.end();
}

void sendDataToThingspeak(float usvh, float voltage, float current)
{
    // make a neat char array from the usvh to post
    String data = String(usvh, 5);
    int length = data.length();
    char msgBuffer[length];
    data.toCharArray(msgBuffer,length+1);
    Serial.print("Sending usvh data to thingspeak: ");
    Serial.println(msgBuffer);  
    ThingSpeak.setField(1, msgBuffer);

    // voltage and current
    ThingSpeak.setField(2, voltage);
    Serial.print("Sending voltage data to thingspeak: ");
    Serial.println(voltage);     
    ThingSpeak.setField(3, current);
    Serial.print("Sending current data to thingspeak: ");
    Serial.println(current);  

    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    Serial.println("Data sent to IoT sink");
}

void logVoltageCurrent(byte logPlace)
{
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.println("");

  cachedVoltage[logPlace] = loadvoltage;
  cachedCurrent[logPlace] = current_mA;
}

void setup()
{
  Serial.begin(9600);
  pinMode(gm_action_pin, INPUT_PULLDOWN_16);
  connectWiFi();
  Serial.println("Wifi connected");
  ThingSpeak.begin(espClient);
  Serial.println("Thingspeak connected");
  ina219.begin();
  // no calibration = 32V 2A
  //ina219.setCalibration_32V_1A();
  ina219.setCalibration_16V_400mA();
}

void loop()
{
  unsigned long currentMillis = millis();

  // timing is not exact beacause it is only checked on geiger events
  // the reason is the buggy wifi_fpm_do_sleep() which does not respect any microsecond value except 0xFFFFFFFF
  // run roughly every minute
  if (currentMillis - previousMillisRead >= interval) 
  {
    previousMillisRead = currentMillis;

    // transform cpm to microsievert per second
    noInterrupts();
    float usvh = cpm_raw * coefficientOfConversion;
    cpm_raw = 0;
    interrupts();

    Serial.print("uSv/h: ");
    Serial.println(usvh, 5);

    cachedData[writtenReports] = usvh;
    logVoltageCurrent(writtenReports);
    writtenReports = writtenReports + 1;
  }

  // run roughly every reportMean * minutes time. default is every 15 minutes.
  if (currentMillis - previousMillisReport >= interval * reportsMean)
  {
    previousMillisReport = currentMillis;
    
    writtenReports = 0;

    float meanReportValue = getAverage(cachedData);
    float meanVoltage = getAverage(cachedVoltage);
    float meanCurrent_mA = getAverage(cachedCurrent);

    wakeWifi();

    // push to data sinks
    sendDataToThingspeak(meanReportValue, meanVoltage, meanCurrent_mA);
    sendDataToRadmon(meanReportValue / coefficientOfConversion);
  }

  enterLightSleep();      
}
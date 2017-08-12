/**
 * Radiosolar by Bartholomäus Dedersen
 * 2017
 *
 * Sends a converted CPM count from a Geiger Miller tube to a websink for IoT data logging.
 * Optimized for ESP8266 low power usage by switching the wifi only on when needed.
 */
#include "Arduino.h"

#include <ESP8266WiFi.h>    
extern "C" {
  #include "user_interface.h"
}

#include <ThingSpeak.h> 

#include "credentials.h"    


int gm_action_pin = D1;

const long interval = 1000 * 60;

const byte reportsMean = 2;

double cachedData[reportsMean];

unsigned long previousMillisReport = 0;

byte writtenReports = 0;

unsigned long previousMillisRead = 0;

volatile int cpm_raw = 0;

const double coefficientOfConversion = 0.00812;


WiFiClient espClient;



void cpm_event()
{
    cpm_raw = cpm_raw + 1;
}

void setWifi(bool isOn)
{
  if(isOn)
  {
    // wake wifi
    WiFi.mode(WIFI_STA);
    Serial.println("Wifi woken");
  }

  if(!isOn)
  {
    // put wifi into sleep again
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin(reportsMean * 1000000L - 250000);
    Serial.println("Wifi sleeping");
  }
}

void connectWiFi()
{
  if(WiFi.status() == WL_CONNECTED)
  {
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //wifi_station_set_auto_connect(true);
  wifi_station_set_hostname(wifiHostname);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
}

double getAverage(double singleValues[])
{
  double totalAddedValue;

  for(int i = 0; i < reportsMean; i++)
  {
    totalAddedValue += singleValues[i];
  }

  return totalAddedValue / reportsMean;
}

void sendDataToThingspeak(double usvh)
{
    // wake wifi
    setWifi(true);

    // make a neat char array from the double to post
    String data = String(usvh, 5);
    int length = data.length();
    char msgBuffer[length];
    data.toCharArray(msgBuffer,length+1);
    Serial.print("Sending data to thingspeak: ");
    Serial.println(msgBuffer);  
    ThingSpeak.writeField(myChannelNumber, 1, msgBuffer, myWriteAPIKey);
    Serial.println("Data sent to IoT sink");

    // put wifi into sleep again
    setWifi(false);
}

void setup()
{
  Serial.begin(9600);
  pinMode(gm_action_pin, INPUT);
  attachInterrupt(gm_action_pin, cpm_event, RISING);
  Serial.println("Interrupts attached!");
  connectWiFi();
  Serial.println("Wifi connected");
  ThingSpeak.begin(espClient);
  Serial.println("Thingspeak connected");
}

void loop()
{
  unsigned long currentMillis = millis();

  // run every minute
  if (currentMillis - previousMillisRead >= interval) 
  {
    previousMillisRead = currentMillis;

    // transform cpm to microsievert per second
    noInterrupts();
    double usvh = cpm_raw * coefficientOfConversion;
    cpm_raw = 0;
    interrupts();

    Serial.print("uSv/h: ");
    Serial.println(usvh, 5);

    cachedData[writtenReports] = usvh;
    writtenReports = writtenReports + 1;
  }

  // run every reportMean * minutes time. default is every 15 minutes.
  if (currentMillis - previousMillisReport >= interval * reportsMean)
  {
    previousMillisReport = currentMillis;
    
    writtenReports = 0;

    double meanReportValue = getAverage(cachedData);

    sendDataToThingspeak(meanReportValue);
  }
}
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
}

#include <ThingSpeak.h> 

// include your own credentials in this file
#include "credentials.h"    

// the out pin of the geiger tube is connected via transistor to this pin of the esp8266
int gm_action_pin = D1;

// make a mean value of multiple counts. default is 15 minutes.
const byte reportsMean = 15;

// --- END OF CONFIGURATION ---


// measure for one minute to get counts per minute
const long interval = 1000 * 60;

// data cache
double cachedData[reportsMean];

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

void ICACHE_RAM_ATTR cpm_event()
{
    cpm_raw = cpm_raw + 1;
}

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

bool isWifiSleeping = false;
void setWifi(bool isOn)
{
  //noInterrupts();

  if(isOn && isWifiSleeping)
  {
    // wake wifi
    WiFi.forceSleepWake();
    delay(250);

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(100);
    }

    Serial.println("Wifi woken");
    isWifiSleeping = false;
  }

  if(!isOn && !isWifiSleeping)
  {
    // put wifi into sleep
    WiFi.forceSleepBegin();
    delay(250);
    Serial.println("Wifi sleeping");
    isWifiSleeping = true;
  }

  //interrupts();
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
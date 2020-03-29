//@Author: EPCB TECH
//@Date: 2020
//@Description: ESP8266 With ThingSpeak Server

#include <DHT.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "DHT.h"

#define DHTPIN D2 
#define SOIL_MOIST_1_PIN A0
#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

float humi; 
float temp;
float moist;

//WiFiSetting
const char* ssid      = "Type SSID(WiFi Name)";
const char* password  = "Type password";

WiFiClient client;

//ThingSpeakSetting
const int channelID		= 12345678// Enter Channel ID;
String writeAPIKey		= "Enter write APIKey Here";
const char* server		= "api.thingspeak.com";

//FunctionDeclare
void    wifiSetup();
void    thingConnect();
void    readSensor(void);
float   getMoist();
void    printData(void);

void setup() {
  Serial.begin(115200);
  Serial.println("Start Reading Sensor!\n");
  dht.begin();
  wifiSetup();
}

void loop() {
  thingConnect();
  readSensor();
  printData();
}

void wifiSetup() {
  Serial.print("Connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.print("\r\nWiFi connected");
  logoConsole();
}

void thingConnect() {
  if (client.connect(server, 80)) {
    String body = "field1=" + String(temp, 1) + "&field2=" + String(humi, 1) + "&field3=" + String(moist, 1);
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + writeAPIKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(body.length());
    client.print("\n\n");
    client.print(body);
    client.print("\n\n");
  }
}

void printData(void) {
  Serial.printf("Temp: %s°C - Hum: %s% - Moist: %s%\r\n", String(temp, 1).c_str(), String(humi, 1).c_str(), String(moist, 1).c_str());
}

void readSensor(void) {
  temp 	= dht.readTemperature();
  humi 	= dht.readHumidity();
  moist = getMoist();
}

float getMoist() {
  float i = 0;
  float anaValue = 0;
  for (i = 0; i < 10; i++)  
  {
    anaValue += analogRead(SOIL_MOIST_1_PIN);
    delay(50);
  }
  anaValue = anaValue / (i);
  anaValue = map(anaValue, 1023, 0, 0, 99);
  return anaValue;
}
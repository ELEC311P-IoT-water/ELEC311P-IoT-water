#ifndef __CC3200R1M1RGC__
// Do not include SPI for CC3200 LaunchPad
#include <SPI.h>
#endif

#include <WiFi.h>
#include "settings_pass.h"

#define DEBUG 1

//Sensor variables
int moistureSensorPin = 2;
float moistureVoltage = 0.0f;
int moistureSensorValue = 0;

//Defualt passwords and keys, true ones in settings_pass.h
#ifndef SETTINGS_PASS_H
#define SETTINGS_PASS_H
#define WIFI_PASSWORD ""
#define DEV_ID 1
#endif

//WiFi variables
char ssid[] = "zcabmze";
char password[] = WIFI_PASSWORD;

//AWS variables
char aws_endpoint[] = "axf431iq9a2ee.iot.eu-central-1.amazonaws.com";
int aws_port = 8443;

WiFiClient client;

void setup() {
  if(DEBUG){
    Serial.begin(9600);
    Serial.print("Connecting to WiFi.");
  }

  pinMode(moistureSensorPin, INPUT);
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    if(DEBUG) Serial.print(".");
    delay(300);
  }

  if(DEBUG) Serial.println();
  
  while (WiFi.localIP() == INADDR_NONE) {
    if(DEBUG) Serial.print(".");
    delay(300);
  }

  if(DEBUG){
    Serial.print("\nIP Address obtained: ");
    Serial.println(WiFi.localIP());
  }
}

void loop() {
  moistureSensorValue = analogRead(moistureSensorPin);
  moistureVoltage = moistureSensorValue * 1.4f / 4095;
  if(DEBUG) Serial.print("Raw value: ");
  if(DEBUG) Serial.println(moistureSensorValue);
  if(DEBUG) Serial.print("Voltage: ");
  if(DEBUG) Serial.println(moistureVoltage);

  String rx = "";
  while(client.available()){
    rx += (char)client.read();
  }
  if(rx != "") Serial.println("Received: " + rx);

  if(client.useRootCA() && DEBUG){Serial.println("Using rootCA");}
  if(!client.connected()){
    client.sslConnect(aws_endpoint, aws_port);
  }
  if(client.connected()){
    if(DEBUG) Serial.println("Connected");
    char postData[100];
    sprintf(postData, "{\"serialNumber\" : \"%d\", \"moisture\": %d}", DEV_ID, (moistureSensorValue/16));
    String request = "POST /topics/sdkTest/sub?qos=1";
    request += " HTTP/1.1";
    Serial.print("Request:\t"); Serial.println(request);
    Serial.print("Post data:\t"); Serial.println(postData);

    client.println(request);
    client.print("Host: ");
    client.println(aws_endpoint);
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.print("Content-Length: "); client.println(strlen(postData));
    client.println();
    client.println(postData);
    client.println();
  }

  delay(10000);
}

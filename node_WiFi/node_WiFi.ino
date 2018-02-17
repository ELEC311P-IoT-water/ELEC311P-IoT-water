#ifndef __CC3200R1M1RGC__
// Do not include SPI for CC3200 LaunchPad
#include <SPI.h>
#endif
#include <WiFi.h>

#define DEBUG 1

int moistureSensorPin = A0;
int moistureSensorValue = 0;

//WiFi settings
char ssid[] = "test";
char password[] = "test";

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
  if(DEBUG) Serial.println(moistureSensorValue);
  
  while(client.available()){
    if(DEBUG) Serial.write(client.read());
  }
  
  if(!client.connected()){
    client.stop();
    client.connect("google.com", 80);
    client.println("GET /index.html HTTP/1.1");
    client.println("Host: google.com");
    client.println("Connection: close");
    client.println("");
  }

  delay(10000);
}

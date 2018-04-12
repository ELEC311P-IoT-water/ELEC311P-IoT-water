#include <SPI.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "settings_pass.h"

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
char wifi_ssid[] = "zcabmze";
char wifi_password[] = WIFI_PASSWORD;
WiFiClientSecure wifiClient;

//AWS variables
char aws_endpoint[] = "axf431iq9a2ee.iot.eu-central-1.amazonaws.com";
int aws_port = 8883;
const char * certPath = "/cert/cert.der";
const char * keyPath = "/cert/key.der";
void callback(char* topic, byte* payload, unsigned int length);
PubSubClient client(aws_endpoint, aws_port, callback, wifiClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received message for topic ");
  Serial.print(topic);
  Serial.print(" with length ");
  Serial.println(length);
  Serial.println("Message:");
  Serial.write(payload, length);
  Serial.println();
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(wifi_ssid, wifi_password);
  while ( WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(300); }
  while (WiFi.localIP() == INADDR_NONE) { Serial.print("."); delay(300); }
}

#define DEBUG 1

void reconnect(){
  while(!client.connected()){
    if(DEBUG) Serial.print("Attempting MQTT connection... ");

    wifiClient.removeSocketOpts();
    SlSockSecureMethod method;
    method.secureMethod = SL_SO_SEC_METHOD_TLSV1_2;
    wifiClient.addConnectSocketOpt(SL_SOL_SOCKET, SL_SO_SECMETHOD, (_u8 *)&method, sizeof(method));
    SlSockSecureMask cipher;
    cipher.secureMask = SL_SEC_MASK_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA;
    wifiClient.addConnectSocketOpt(SL_SOL_SOCKET, SL_SO_SECURE_MASK, (_u8 *)&cipher, sizeof(cipher));
    wifiClient.addConnectSocketOpt(SL_SOL_SOCKET, SL_SO_SECURE_FILES_CERTIFICATE_FILE_NAME, certPath, strlen(certPath));
    wifiClient.addConnectSocketOpt(SL_SOL_SOCKET, SL_SO_SECURE_FILES_PRIVATE_KEY_FILE_NAME, keyPath , strlen(keyPath) );
    wifiClient.useRootCA();
    
    if(client.connect("sensor1")){
      if(DEBUG) Serial.println("Connected");
      client.subscribe("water");
    }
    else{
      if(DEBUG) Serial.print("Failed, rc=");
      if(DEBUG) Serial.print(client.state());
      if(DEBUG) Serial.println(" trying again in 5 seconds.");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  moistureSensorValue = analogRead(moistureSensorPin);
  char buffer[128];
  sprintf(buffer, "{\"id\": %d, \"moisture\": %d}", DEV_ID, (moistureSensorValue/16));
  if(client.publish("water", buffer)) {
    if(DEBUG) Serial.println("Publish success");
  } else {
    if(DEBUG) Serial.println("Publish failed");
  }
 
  // Check if any messages were received
  // on the topic we subscribed to
  client.loop();
  delay(1000);
}

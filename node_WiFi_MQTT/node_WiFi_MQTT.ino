#include <SPI.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

//Sensor variables
int moistureSensorPin = 2;
float moistureVoltage = 0.0f;
int moistureSensorValue = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received message for topic ");
  Serial.print(topic);
  Serial.print(" with length ");
  Serial.println(length);
  Serial.println("Message:");
  Serial.write(payload, length);
  Serial.println();
}

WiFiClientSecure wifiClient;
const char * domain = "axf431iq9a2ee.iot.eu-central-1.amazonaws.com";
int port = 8883;
const char * certPath = "/cert/cert.der";
const char * keyPath = "/cert/key.der";
PubSubClient client(domain, port, callback, wifiClient);

void setup() {
  Serial.begin(9600);
  WiFi.begin("zcabmze", "mironpass");
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
  
  if(client.publish("water","{\"id\": 6, \"moisture\": 129}")) {
    Serial.println("Publish success");
  } else {
    Serial.println("Publish failed");
  }
 
  // Check if any message were received
  // on the topic we subscribed to
  client.loop();
  delay(1000);
}

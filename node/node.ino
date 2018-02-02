//Adapted from https://github.com/sandeepmistry/arduino-LoRa/blob/master/examples/LoRaSender/LoRaSender.ino
//Using the Arduino LoRa library from https://github.com/sandeepmistry/arduino-LoRa

#include <SPI.h>
#include <LoRa.h>

int counter = 0;
int id = 1; //ID of this device

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Sender");

  if (!LoRa.begin(865E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print(id);
  LoRa.print(" hello ");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;

  delay(5000);
}

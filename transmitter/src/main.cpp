#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// put function declarations here:

RF24 radio(9,10);// CE, CSN

const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);
  Serial.write("Transmitting");
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  const char text[] = "Hello World";
  radio.write(&text, sizeof(text));
  delay(1000);
}
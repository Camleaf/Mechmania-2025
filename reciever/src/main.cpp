
#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define RightSideAnalog 3 // check if this and LSDir actually overwrite or wtf happened here
#define LeftSideAnalog 4
#define RsDir1 8
#define RsDir2 7
#define LsDir1 6
#define LsDir2 5


int Ldir = 0;
int Rdir = 0;
typedef struct{
  int16_t Rspeed;
  int16_t Lspeed;
}__attribute__((packed)) SpeedStruct;

SpeedStruct dataPackage;



RF24 radio(1,2); // CE, CSN CHANGE THESE PINS

const byte address[5] = "0110";

void setup() {
  // pinmodes

  pinMode(1,  OUTPUT); 
  pinMode(2, OUTPUT);

  pinMode(LsDir1, OUTPUT);
  pinMode(RsDir1, OUTPUT);
  pinMode(RsDir2, OUTPUT);
  pinMode(LsDir2, OUTPUT);
  // end pinmodes

  Serial.begin(9600); 
  radio.begin();
  // check return for radio.begin for both modules
  radio.openReadingPipe(0, address);
  radio.setPALevel(-18);
  radio.startListening();

  // Establish connection with transmitter
  while (!(radio.available()));
  char text[22] = "";
  radio.read(&text, sizeof(text));
  radio.stopListening();
  Serial.print(text);
  // Once init command is given return a response

  radio.startListening();
  Serial.write("Listening");

}

void loop() {
  while (!radio.available());
  radio.read( &dataPackage, sizeof(SpeedStruct) ); // reminder to add u10 capacitor near power
  Serial.print("\n");
  Serial.print("\n");
  // Serial.print("Recieved");
  // Serial.print("data.RightSpeed= ");
  // Serial.print(dataPackage.Rspeed);
  // Serial.print("\n");
  // Serial.print("data.LeftSpeed= ");
  // Serial.print(dataPackage.Lspeed);

  // add a bit of comprehension just to split tasks between controller and car
  if (dataPackage.Lspeed < 0){ //just fixing up how output was given by other thing
    digitalWrite(LsDir1, HIGH);
    digitalWrite(LsDir2, LOW);
    dataPackage.Lspeed *= -1;
    Serial.print("\n");
    Serial.print("LeftDown");

  } else {
    digitalWrite(LsDir1, LOW);
    digitalWrite(LsDir2, HIGH);
    Serial.print("\n");
    Serial.print("LeftUp");
  }
  if (dataPackage.Rspeed < 0) { // fix up rspeed
    digitalWrite(RsDir1, HIGH);
    digitalWrite(RsDir2, LOW);
    dataPackage.Rspeed *= -1;
    Serial.print("\n");
    Serial.print("RightDown");
  } else {
    digitalWrite(RsDir1, LOW);
    digitalWrite(RsDir2, HIGH);
    Serial.print("\n");
    Serial.print("RightUp");
  }

  analogWrite(RightSideAnalog, dataPackage.Rspeed);
  analogWrite(LeftSideAnalog, dataPackage.Lspeed);

  
}
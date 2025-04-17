
#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define RightSideAnalog 5
#define LeftSideAnalog 4
#define RsDir1 8
#define RsDir2 7
#define LsDir1 6
#define LSDir2 5


int Ldir = 0;
int Rdir = 0;
typedef struct{
  int Rspeed = 0;
  int Lspeed = 0;
}
SpeedStruct;

SpeedStruct dataPackage;



RF24 radio(9,10); // CE, CSN

const byte address[6] = "11001";

void setup() {
  // pinmodes

  pinMode(9,  OUTPUT); 
  pinMode(10, OUTPUT);
  // end pinmodes

  Serial.begin(9600);
  Serial.write("Listening");
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  if ( radio.available() )
  {
    radio.read( &dataPackage, sizeof(dataPackage) );
  }

  // add a bit of comprehension just to split tasks between controller and car
  if (dataPackage.Lspeed < 0){ //just fixing up how output was given by other thing
    digitalWrite(LsDir1, HIGH);
    digitalWrite(LsDir1, LOW);
    dataPackage.Lspeed *= -1;
  } else {
    digitalWrite(LsDir1, LOW);
    digitalWrite(LsDir1, HIGH);
  }
  if (dataPackage.Rspeed < 0) { // fix up rspeed
    digitalWrite(RsDir1, HIGH);
    digitalWrite(RsDir1, LOW);
    dataPackage.Rspeed *= -1;
  } else {
    digitalWrite(RsDir1, LOW);
    digitalWrite(RsDir1, HIGH);
  }

  analogWrite(RightSideAnalog, dataPackage.Rspeed);
  analogWrite(LeftSideAnalog, dataPackage.Lspeed);

  delay(2);

  
}
#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

using namespace std;

// put function declarations here:
#define RjoyX A0
#define RjoyY A1
#define LjoyX A3
#define LjoyY A2 // check if these are switched at all and in wrong spots

#define ControlDeadZone 20


int rawAccelForce = 512;
int rawTurnForce = 512;
long accelForce = 0;
long turnForce = 0;
int maximum = 0;
int difference = 0;
int total = 0;

typedef struct{
  int Rspeed;
  int Lspeed;
}
SpeedStruct;

SpeedStruct dataPackage; // using struct to package data to send to other arduino


RF24 radio(9,10);// CE, CSN Check that these pins are in the right place

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

  //tank drive
 

  rawAccelForce = analogRead(LjoyX);
  rawTurnForce = analogRead(RjoyY);
  // read raw values

  // the anolog controller works on a scale of 0 - 1023 where ~ 512 is centered
  accelForce = 0; // if stick is in deadzone no movement
  if (!(512 - ControlDeadZone < rawAccelForce && 512 + ControlDeadZone > rawAccelForce)){ // check for control deadzones
    // this almost works just the entry condition only works for up not down
    Serial.print("working");
    // handle all rotation calculations here to deal with less strain on actual motor controller
    if (rawAccelForce < 511){
      Serial.print("small");
      accelForce= 512-rawAccelForce;
      accelForce = (accelForce*100) / 512;
      accelForce = accelForce * 255;
      accelForce = round(accelForce/100);
    } else {
      accelForce = rawAccelForce - 512;
      Serial.print(accelForce);
      accelForce = (accelForce*100) / 512;
      accelForce= accelForce * 255;
      accelForce= -round(accelForce/100);
     
    }

  }
  turnForce = 0;
 
  if (!(512 - ControlDeadZone < rawTurnForce && 512 + ControlDeadZone > rawTurnForce)) { //check for control deadzones
    // turn
    Serial.print("working");
    // handle all rotation calculations here to deal with less strain on actual motor controller
    if (rawTurnForce < 511){
      turnForce = 512-rawTurnForce;
      turnForce = (turnForce*100) / 512;
      turnForce = turnForce * 255;
      turnForce = round(turnForce/100);
    } else {
      turnForce = rawTurnForce - 512;
      turnForce = (turnForce*100) / 512;
      turnForce = turnForce * 255;
      turnForce = -round(turnForce/100);
    }
  }
 
  maximum = max(abs(accelForce), abs(turnForce));
  difference = accelForce - turnForce;
  total = accelForce + turnForce;

  if (accelForce >= 0){ // L or R direction vars don't matter here as at this point they are the same
    if (turnForce >= 0){ // Joystick stop right
      dataPackage.Lspeed = maximum;
      dataPackage.Rspeed = difference;
    } else {  // Joystick top left
      dataPackage.Lspeed = total;
      dataPackage.Rspeed = maximum;
    }
  } else {
    if (turnForce >= 0) { // Joystick  bottom right
      dataPackage.Lspeed = total;
      dataPackage.Rspeed = -maximum;
    } else { // Joystick bottom left
      dataPackage.Lspeed = -maximum;
      dataPackage.Rspeed = difference;
    }
  }

  Serial.print(rawAccelForce);
  Serial.print('\n');
  Serial.print(accelForce);
  Serial.print('\n');
  radio.write(&dataPackage, sizeof(dataPackage));
  delay(1000);
}
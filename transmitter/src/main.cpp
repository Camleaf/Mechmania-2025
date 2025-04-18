#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

using namespace std;

// put function declarations here:
#define RjoyX A0
#define RjoyY A1
#define LjoyX A2
#define LjoyY A3 // check if these are switched at all and in wrong spots

#define ControlDeadZone 20


int rawAccelForce = 512;
int rawTurnForce = 512;
int accelForce = 0;
int turnForce = 0;
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
  

  rawAccelForce = analogRead(LjoyY);
  rawTurnForce = analogRead(RjoyX);
  // read raw values

  // the anolog controller works on a scale of 0 - 1023 where ~ 512 is centered
  
  if (!(512 - ControlDeadZone < rawAccelForce && 512 + ControlDeadZone > rawAccelForce)){ // check for control deadzones
    // this almost works just the entry condition only works for up not down
    if (rawAccelForce < 511){
      accelForce = round(((512-rawAccelForce) / 512)  * 255);
    } else {
      accelForce = -round(((rawAccelForce - 512) / 512) * 255 );
    }

  } else {
    accelForce = 0; // if stick is in deadzone no movement
  }

  if (!(512 - ControlDeadZone < rawTurnForce && 512 + ControlDeadZone > rawTurnForce)) { //check for control deadzones
    // turn
    // handle all rotation calculations here to deal with less strain on actual motor controller
    if (rawTurnForce < 511){
      turnForce = round(((512-rawTurnForce) / 512) * 255);
    } else {
      turnForce = -round(((rawTurnForce - 512) / 512) * 255);
    }
  } else {
    turnForce = 0;
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

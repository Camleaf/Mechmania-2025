#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <stdio.h>
#include <printf.h>
#include <RF24_config.h>
using namespace std;

// put function declarations here:
#define RjoyX A2
#define RjoyY A3
#define LjoyX A1
#define LjoyY A0 // check if these are switched at all and in wrong spots

#define ControlDeadZone 20


int rawAccelForce = 512;
int rawTurnForce = 512;
long accelForce = 0;
long turnForce = 0;
int maximum = 0;
int difference = 0;
int total = 0;

typedef struct{
  int16_t Rspeed;
  int16_t Lspeed;
}__attribute__((packed)) SpeedStruct;

SpeedStruct dataPackage; // using struct to package data to send to other arduino


RF24 radio(9,10);// CE, CSN CHANGE THESE PINS

const byte address[5] = "0110";

void setup() {
  printf_begin();
  Serial.begin(9600);
  if (!radio.begin()){Serial.println();}
  // radio.printDetails();
  // check return for radio.begin for both modules to check wiring and power. Also add u10 capacitor near power pins for both
  radio.openWritingPipe(address);
  radio.setPALevel(-18);
  radio.stopListening();


  // while(1) {}
  // Establish connection with reciever
  char text[22] = "Initialize";
  radio.write(text, sizeof(text));
  radio.startListening();
  Serial.print("Initialized pipe, waiting for response");
  radio.stopListening();
  Serial.write("Transmitting");
}

void loop() {

  //tank drive
 

  rawAccelForce = analogRead(RjoyX);
  rawTurnForce = analogRead(LjoyX);
  // read raw values


  // the anolog controller works on a scale of 0 - 1023 where ~ 512 is centered
  accelForce = 0; // if stick is in deadzone no movement
  if (!(512 - ControlDeadZone < rawAccelForce && 512 + ControlDeadZone > rawAccelForce)){ // check for control deadzones
    // this almost works just the entry condition only works for up not down
    // handle all rotation calculations here to deal with less strain on actual motor controller
    if (rawAccelForce < 511){
      accelForce= 512-rawAccelForce;
      accelForce = (accelForce*100) / 512;
      accelForce = accelForce * 255;
      accelForce = round(accelForce/100);
    } else {
      accelForce = rawAccelForce - 512;
      accelForce = (accelForce*100) / 512;
      accelForce= accelForce * 255;
      accelForce= -round(accelForce/100);
     
    }

  }
  turnForce = 0;
 
  if (!(512 - ControlDeadZone < rawTurnForce && 512 + ControlDeadZone > rawTurnForce)) { //check for control deadzones
    // turn
    // handle all rotation calculations here to deal with less strain on actual motor controller
    if (rawTurnForce < 511){
      turnForce = 512-rawTurnForce;
      turnForce = (turnForce*100) / 512;
      turnForce = turnForce * 255;
      turnForce = -round(turnForce/100);
    } else {
      turnForce = rawTurnForce - 512;
      turnForce = (turnForce*100) / 512;
      turnForce = turnForce * 255;
      turnForce = round(turnForce/100);
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
    }//
  }
  Serial.print('\n');
  Serial.print('\n');
  Serial.print('\n');
  Serial.print("data LeftSpeed= ");
  Serial.print(dataPackage.Lspeed);
  Serial.print('\n');
  Serial.print("data RightSpeed= ");
  Serial.print(dataPackage.Rspeed);


  radio.write(&dataPackage, sizeof(SpeedStruct));
  
  delay(10);
}
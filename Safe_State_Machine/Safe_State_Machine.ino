// Finite state machine to operate fingerprint sensor safe
// Used code from FPS_IDFinger.ino - Library example for controlling GT-511C3 Finger Print Scanner created by Josh Hawley
// Created by David Yu

#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"

//set up software serial pins for Arduino's w/ Atmega328P's
FPS_GT511C3 fps(4,5);

//Declare states
enum State_enum {LOCKED, CHECKING, ACTIVESOL, FAILED};

//declare global variables
uint8_t state = LOCKED;
int solenoid = 8;

void setup() {
  //setup LEDs
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(solenoid, OUTPUT);

  //setup Arduino hardware and fingerprint sensor
  Serial.begin(9600); //set up Arduino's hardware serial UART
  delay(100);
  fps.Open();         //send serial command to initialize fps
  fps.SetLED(true);   //turn on LED so fps can see fingerprint
}

void loop() {
  // put your main code here, to run repeatedly:
  state_machine_run();
  delay(100);
}

void state_machine_run() {
  switch(state) {
    //Waiting state in which safe door is locked and waiting for user to touch the sensor
    case LOCKED:
      Serial.println("In LOCKED state");
      if (fps.IsPressFinger()) {
        state = CHECKING;
      }
      else {
        state = LOCKED;
      }
      break;
      
    //Fingerprint verification state- checks user's fingerprint with senso'r database
    case CHECKING:
      Serial.println("Verifying fingerprint");
      if (fps.IsPressFinger()) {
        fps.CaptureFinger(false);
        int id = fps.Identify1_N();
        if (id < 200) {
          Serial.print("Verified ID: ");
          Serial.println(id);
          state = ACTIVESOL;
        } else {
          state = FAILED;
        }
      }
      break;

    //Failed verification state- user's fingerprint is not in the database
    case FAILED: 
      Serial.println("Fingerprint does not match");
      digitalWrite(13, HIGH);
      delay(300);
      digitalWrite(13, LOW);
      state = LOCKED;
      delay(1000);
      break;

    //After successful verification, unlock the safe door
    case ACTIVESOL:
      Serial.println("Verification Success");
      digitalWrite(12, HIGH);
      delay(300);
      digitalWrite(solenoid, HIGH);
      delay(2000);
      digitalWrite(solenoid, LOW);
      delay(100);
      digitalWrite(12, LOW);
      delay(3000);
      if (fps.IsPressFinger()) {
        state = CHECKING;  
      }
      else {
        state = LOCKED;
      }
      break;

  }    
}

/*----------
    PIR.hpp  -  PIR (Pyroelectric/Passive InfraRed) sensor library
    Created by Tuan T. Nguyen
    Date: Mar. 6, 2020
*/

// select number of pins 
// check pin status separately
// check all pin simutaneously

// SHOULD USE ARRAY FOR PIN ARGUMENT

#include "PIR.hpp"

bool pin_state_1, pin_state_2, pin_state_3, pin_state_4, pin_state_5, pin_state_6;

PIR::PIR(uint8_t pin1,uint8_t pin2,uint8_t pin3,uint8_t pin4,uint8_t pin5,uint8_t pin6){
  _pin1 = pin1;
  _pin2 = pin2;
  _pin3 = pin3;
  _pin4 = pin4;
  _pin5 = pin5;
  _pin6 = pin6;
  };

void PIR::configuration(){
  pinMode(_pin1, INPUT);
  pinMode(_pin2, INPUT);
  pinMode(_pin3, INPUT);
  pinMode(_pin4, INPUT);
  pinMode(_pin5, INPUT);
  pinMode(_pin6, INPUT);
  };

bool PIR::humanPresenceCheck(){
  pin_state_1 = digitalRead(_pin1);
  pin_state_2 = digitalRead(_pin2);
  pin_state_3 = digitalRead(_pin3);
  pin_state_4 = digitalRead(_pin4);
  pin_state_5 = digitalRead(_pin5);
  pin_state_6 = digitalRead(_pin6);
  if(pin_state_1||pin_state_2||pin_state_3||pin_state_4||pin_state_5||pin_state_6){
    return 1;
    }
  else{
    return 0;
    }
  }

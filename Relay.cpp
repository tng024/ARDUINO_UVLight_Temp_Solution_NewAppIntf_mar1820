/*----------
    Relay.cpp  -  Relay control library
    Created by Tuan T. Nguyen
    Date: Mar. 6, 2020
*/

#include "Relay.hpp"
#include "Arduino.h"
#include "Wire.h"

Relay::Relay(int pin){
  pinMode(pin, OUTPUT);
  _pin = pin;
  }

void Relay::initialize(){
  digitalWrite(_pin, LOW);
  }

void Relay::ON(){
  digitalWrite(_pin, HIGH);
  };

void Relay::OFF(){
  digitalWrite(_pin, LOW);
  }

void Relay::stateSwitch(bool state){
  digitalWrite(_pin, (state?HIGH:LOW));
  }

bool Relay::stateIsChanged(bool currentState, bool previousState){
  if(currentState != previousState){
    return 1;
    }
  else{
    return 0;
    }
  }
  

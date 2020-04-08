/*----------
    PIR.hpp  -  PIR (Pyroelectric/Passive InfraRed) sensor library
    Created by Tuan T. Nguyen
    Date: Mar. 6, 2020
*/

// select number of pins 
// check pin status separately
// check all pin simutaneously

#pragma once

#include "Arduino.h"
#include "Wire.h"

class PIR{
  
  public:
//    PIR(uint8_t pin1,uint8_t pin2,uint8_t pin3,uint8_t pin4,uint8_t pin5,uint8_t pin6);
    PIR(uint8_t pin1);
    void configuration();
    bool humanPresenceCheck();
    
  private:
    uint8_t _pin1;
//    uint8_t _pin2;
//    uint8_t _pin3;
//    uint8_t _pin4;
//    uint8_t _pin5;
//    uint8_t _pin6;
  };

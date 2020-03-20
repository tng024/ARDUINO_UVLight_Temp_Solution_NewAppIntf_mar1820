/*----------
    Relay.hpp  -  Relay control library
    Created by Tuan T. Nguyen
    Date: Mar. 6, 2020
*/

#pragma once

#include "Arduino.h"
#include "Wire.h"

class Relay{
  public:
    Relay(int pin);
    void initialize();
    void ON();
    void OFF();
    void stateSwitch(bool state);

  private: 
    int _pin;  
  };

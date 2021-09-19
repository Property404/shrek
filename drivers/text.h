#pragma once
#include "Driver.h"

class DEVICE_COUPLING_INTERFACE(TextDeviceCoupling) {
 public:
     virtual int getCharacter() = 0;
     virtual void putCharacter(char c) = 0;
     virtual bool characterIsAvailable() = 0;
};

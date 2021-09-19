#pragma once
#include "../Driver.h"

class DEVICE_COUPLING_INTERFACE(TimerDeviceCoupling) {
 public:
     virtual uint64_t getTicks() = 0;
};

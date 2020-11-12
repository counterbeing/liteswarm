#pragma once
#include <Arduino.h>

#if BOARD == BOARD_NANO
#  include "./MyKnob/MyKnobNano.h"
#elif BOARD == BOARD_ESP32
#  include "./MyKnob/MyKnobEsp32.h"
#else
#  error "No `MyKnob` was defined for this `BOARD`"
#endif

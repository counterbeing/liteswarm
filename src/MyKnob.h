#include "DebugLog.h"
#include "config.h"
#include <Arduino.h>
#include <Bounce2.h>
#include <Encoder.h>

#ifndef MyKnob_H
#  define MyKnob_H
#  if BOARD == BOARD_NANO
#    include "./MyKnob/MyKnobNano.h"
#  elif BOARD == BOARD_ESP32
#    include "./MyKnob/MyKnobEsp32.h"
#  else
#    error "No `MyKnob` was defined for this `BOARD`"
#  endif
#endif

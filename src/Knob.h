#pragma once
#include "DebugLog.h"
#include "KnobSettings.h"
#include "config.h"
#include <ESP32Encoder.h>

ESP32Encoder encoder;

class Knob {
 private:
  int32_t currentValue;
  int32_t minValue;
  int32_t maxValue;
  bool loopRotary;

 public:
  Knob()
      : currentValue(0)
      , minValue(0)
      , maxValue(100)
      , loopRotary(false) {
    ESP32Encoder::useInternalWeakPullResistors = UP;
  }

  int32_t get() { return currentValue; }

  void set(int32_t newValue) {
    if (newValue < minValue) {
      newValue = loopRotary ? maxValue : minValue;
    } else if (newValue > maxValue) {
      newValue = loopRotary ? minValue : maxValue;
    }

    if (newValue == currentValue)
      return;

    encoder.setCount(newValue);
    currentValue = newValue;
  }

  int32_t getCorrectedValue() {
    int32_t position = encoder.getCount();
    if (position < minValue) {
      position = loopRotary ? maxValue : minValue;
      encoder.setCount(position);
    } else if (position > maxValue) {
      position = loopRotary ? minValue : maxValue;
      encoder.setCount(position);
    }
    return position;
  }

  bool update() {
    int32_t newValue = getCorrectedValue();
    if (newValue != currentValue) {
      currentValue = newValue;
      return true;
    }
    return false;
  }

  // When activating knob settings, you're setting the ranges that are allowed
  // for the knob for a particular animation. This is called by the Animation
  // controller.
  void activate(KnobSettings knobSettings) {
    currentValue = knobSettings.currentValue;
    minValue = knobSettings.minValue;
    maxValue = knobSettings.maxValue;
    loopRotary = knobSettings.loopRotary;
    encoder.setCount(currentValue);
  }

  void setup() { encoder.attachHalfQuad(19, 18); }
};
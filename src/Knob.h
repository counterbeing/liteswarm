#include "DebugLog.h"
#include "config.h"
#include <ESP32Encoder.h>

ESP32Encoder encoder;

class Knob {
 private:
  int32_t currentValue;
  const int32_t minValue;
  const int32_t maxValue;
  const bool loopRotary;

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

  void activate() { encoder.setCount(currentValue); }

  void setup() { encoder.attachHalfQuad(19, 18); }
};
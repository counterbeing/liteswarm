#ifndef ANIMATION_H
#define ANIMATION_H
#include <MyKnob.h>
#include "FastLED.h"
#include "config.h"

class Animation {
 protected:
  int start;
  int finish;
  int initialSetting;
  long previousMillis = 0;
  bool initialized = false;

  int remapInRange(int index) {
    if (index < 0) return NUMPIXELS;
    if (index > NUMPIXELS) return 0;
    return index;
  }

  bool nonBlockDelay(unsigned int interval) {
    unsigned long current_millis = millis();
    if ((current_millis - previousMillis) > interval) {
      previousMillis = current_millis;
      return true;
    }
    return false;
  }

 public:
  virtual void setup() = 0;
  virtual void loop() = 0;
  Animation(){};
};
#endif
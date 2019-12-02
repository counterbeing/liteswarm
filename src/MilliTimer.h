#include "FastLED.h"

#ifndef MilliTimer_H
#define MilliTimer_H

class MilliTimer {
 private:
  unsigned long previousMillis = 0;

 public:
  bool hasElapsed(unsigned int interval) {
    unsigned long current_millis = millis();
    return (current_millis - previousMillis) > interval;
  }

  bool hasElapsedWithReset(unsigned int interval) {
    unsigned long current_millis = millis();
    if ((current_millis - previousMillis) > interval) {
      previousMillis = current_millis;
      return true;
    }
    return false;
  }
};

#endif // MilliTimer_H
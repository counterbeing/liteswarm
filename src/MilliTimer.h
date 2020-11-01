#include "FastLED.h"

#ifndef MilliTimer_H
#  define MilliTimer_H

class MilliTimer {
 private:
  unsigned long previousMillis = 0;

 public:
  bool hasElapsed(const unsigned int interval) {
    return (millis() - previousMillis) > interval;
  }

  bool hasElapsedWithReset(const unsigned int interval) {
    const unsigned long current_millis = millis();
    if ((current_millis - previousMillis) > interval) {
      previousMillis = current_millis;
      return true;
    }
    return false;
  }

  void reset() { previousMillis = millis(); }
};

#endif // MilliTimer_H
#include "Animation.h"
#include "MilliTimer.h"

class Strobe : public Animation {
 private:
  CRGB *leds;
  bool goWhite = true;
  unsigned long startMillis = 0;
  const static unsigned long strobeDuration = 750;
  MilliTimer timer{};

 public:
  Strobe(CRGB leds_[]) : leds(leds_) {}

  bool is_done() {
    if (startMillis == 0) {
      startMillis = millis();
    }

    if (nonBlockDelaySinceStart(strobeDuration)) {
      startMillis = 0;
      return true;
    }
    return false;
  }

  void wakeUp() {
  }

  void update() {
    if (timer.hasElapsedWithReset(50)) {
      if (goWhite) {
        fill_solid(leds, NUMPIXELS, CRGB::White);
        goWhite = false;
      }
      else {
        fill_solid(leds, NUMPIXELS, CRGB::Black);
        goWhite = true;
      }      
    }
  }

  bool nonBlockDelaySinceStart(unsigned int interval) {
    unsigned long current_millis = millis();
    if ((current_millis - startMillis) > interval) {
      return true;
    }
    return false;
  }
};

#include "Animation.h"
#include "MilliTimer.h"

class FindMyBike : public Animation {
 private:
  MilliTimer timer{};

 public:
  FindMyBike(CRGB leds_[]) : Animation(leds_) {}

  void wakeUp() {
  }

  void loop() {
    FastLED.clear();
    if (timer.hasElapsedWithReset(3000)) {
      fill_solid(leds, NUMPIXELS, CRGB::White);
    }
  }

};

#include "Animation.h"
#include "DebugLog.h"
#include "MilliTimer.h"

class Stripes : public Animation {
 private:
  KnobSetting delay{180, 0, 500, false};
  MilliTimer timer{};
  int lastPosition = 0;

 public:
  Stripes(CRGB leds_[]) : Animation(leds_) {}

  void wakeUp() override {
    delay.activate();
  }

  void update() override {
    if (delay.update()) {
      configChangeFlag = true;
    }

    int stripeLength = 4;

    if (timer.hasElapsedWithReset(delay.get())) {
      lastPosition++;
      if (lastPosition >= (stripeLength * 2)) {
        lastPosition = 0;
      }
      fill_solid(leds, NUMPIXELS, CRGB::Red);
      for (int dot = lastPosition; dot < NUMPIXELS; dot += (stripeLength * 2)) {
        for (int band = 0; band < stripeLength; band++) {
          leds[dot + band] = CRGB::Purple;
        }
      }

    }
  }
};

#include "Animation.h"
#include "MilliTimer.h"

class DiamondNecklace : public Animation {
 private:
  KnobSetting delay{50, 0, 300, false};
  MilliTimer timer{};

 public:
  DiamondNecklace(CRGB leds_[]) : Animation(leds_) {}

  void wakeUp() override {
    delay.activate();
  }

  void update() override {
    if (delay.update()) {
      configChangeFlag = true;
    }

    if (timer.hasElapsedWithReset(delay.get())) {
      for (int i = 0; i < NUMPIXELS; i++) {
        leds[i].fadeLightBy(128);
        if (random(40) == 1) { leds[i] = CRGB::White; }
      }
    }

  }

};

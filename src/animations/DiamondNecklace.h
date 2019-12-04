#include "Animation.h"
#include "MyKnob.h"
#include "MilliTimer.h"

class DiamondNecklace : public Animation {
 private:
  KnobSetting delay{50, 0, 300, false};
  MilliTimer timer{};

 public:
  DiamondNecklace(CRGB leds_[]) : Animation(leds_) {}

 protected:
  void activate() override {
    delay.activate();
  }

  bool updateAnimation(bool justActivated) override {
    delay.update();

    if (timer.hasElapsedWithReset(delay.get()) || justActivated) {
      for (int i = 0; i < NUMPIXELS; i++) {
        leds[i].fadeLightBy(128);
        if (random(40) == 1) { leds[i] = CRGB::White; }
      }
      return true;
    }

    return false;
  }

  uint32_t getKnobPosition() override {
    return delay.get();
  }

};

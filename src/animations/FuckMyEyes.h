#include "Animation.h"
#include "MilliTimer.h"
#include "MyKnob.h"

class FuckMyEyes : public Animation {
 private:
  KnobSetting delay{2000, 0, 300, false};
  MilliTimer timer{};
  uint8_t lastColor = 0;

 public:
  FuckMyEyes(CRGB leds_[]) : Animation(leds_) {}

 protected:
  void activate() override { delay.activate(); }

  bool updateAnimation(bool justActivated) override {
    delay.update();

    if (timer.hasElapsedWithReset(delay.get()) || justActivated) {
      switch (lastColor) {
        case 0:
          fill_solid(leds, NUMPIXELS, CRGB::Red);
          lastColor = 1;
          break;
        case 1:
          fill_solid(leds, NUMPIXELS, CRGB::Green);
          lastColor = 2;
          break;
        case 2:
          fill_solid(leds, NUMPIXELS, CRGB::Blue);
          lastColor = 0;
          break;
      }
      return true;
    }

    return false;
  }

  uint32_t getKnobPosition() override { return delay.get(); }
};

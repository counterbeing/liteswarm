#include "Animation.h"
#include "MilliTimer.h"

class FuckMyEyes : public Animation {
 private:
  KnobSetting delay{2000, 0, 300, false};
  MilliTimer timer{};
  uint8_t lastColor = 0;

 public:
  FuckMyEyes(CRGB leds_[]) : Animation(leds_) {}

  void wakeUp() override {
    delay.activate();
  }

  void update() override {
    if (delay.update()) {
      configChangeFlag = true;
    }

    if (timer.hasElapsedWithReset(delay.get())) {
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
    }
  }

};

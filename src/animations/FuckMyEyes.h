#include "Animation.h"
#include "MilliTimer.h"

class FuckMyEyes : public Animation {
 private:
  CRGB *leds;
  int lastColor = 0;
  MilliTimer timer{};
  int32_t delay = 200;
  KnobControl knobControl{0, 300, false};

 public:
  FuckMyEyes(CRGB leds_[]) : leds(leds_) {}

  void wakeUp() {
    knobControl.setPosition(delay);
  }

  void update() {
    if (knobControl.updateSettingOnChange(delay)) {
      configChangeFlag = true;
      if (ANIM_DEBUG) debugLog("config change: FuckMyEyes::delay = ", delay);
    }

    if (timer.hasElapsedWithReset(delay)) {
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

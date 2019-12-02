#include "Animation.h"
#include "MilliTimer.h"

class DiamondNecklace : public Animation {
 private:
  CRGB *leds;
  MilliTimer timer{};
  int32_t delay = 50;
  KnobControl knobControl{0, 300, false};

 public:
  DiamondNecklace(CRGB leds_[]) : leds(leds_) {}

  void wakeUp() {
    knobControl.setPosition(delay);
  }

  void update() {
    if (knobControl.updateSettingOnChange(delay)) {
      configChangeFlag = true;
      if (ANIM_DEBUG) debugLog("config change: DiamondNecklace::delay = ", delay);
    }

    if (timer.hasElapsedWithReset(delay)) {
      for (int i = 0; i < NUMPIXELS; i++) {
        leds[i].fadeLightBy(128);
        if (random(40) == 1) { leds[i] = CRGB::White; }
      }
    }

  }

};

#include "Animation.h"
#include "DebugLog.h"
#include "MilliTimer.h"

class Stripes : public Animation {
 private:
  MilliTimer timer{};
  int lastPosition = 0;
  int32_t delay = 180;
  KnobControl knobControl{0, 500, false};

 public:
  Stripes(CRGB leds_[]) : Animation(leds_) {}

  void wakeUp() {
    knobControl.setPosition(delay);
  }

  void update() {
    if (knobControl.updateSettingOnChange(delay)) {
      configChangeFlag = true;
      if (ANIM_DEBUG) debugLog("config change: Strips::delay = ", delay);
    }

    int stripeLength = 4;

    if (timer.hasElapsedWithReset(delay)) {
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
}
;

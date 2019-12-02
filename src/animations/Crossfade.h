// #include <MyKnob.h>
#include "Animation.h"
// #include "FastLED.h"
#include "MilliTimer.h"

class Crossfade : public Animation {
 private:
  CRGB *leds;
  int32_t delay = 35;
  MilliTimer timer{};
  KnobControl knobControl{5, 200, false};
  uint8_t hue = 0;

 public:
  Crossfade(CRGB leds_[]) : leds(leds_) {}

  void wakeUp() {
    knobControl.setPosition(delay);
    debugLog("Crossfade::wakeUp() set position (delay) to ", delay);
  }

  void update() {
    if (knobControl.updateSettingOnChange(delay)) {
      configChangeFlag = true;
      if (ANIM_DEBUG) debugLog("config change: Crossfade::delay = ", delay);
    }

    if (timer.hasElapsedWithReset(delay)) {
      fill_solid(leds, NUMPIXELS, CHSV(hue++, 255, 255));
    }
  }

};

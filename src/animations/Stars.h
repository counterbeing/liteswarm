#include "Animation.h"
#include "DebugLog.h"
#include "MilliTimer.h"

// TODO: Stars is basically the same as Stripes but with different settings. Collapse them?
class Stars : public Animation {
 private:
  CRGB *leds;
  MilliTimer timer{};
  int lastPosition = 0;
  int32_t delay = 100;
  int32_t numPixelsBetweenStars = 13;
  KnobControl knobControl{0, 1000, false};

 public:
  Stars(CRGB leds_[]) : leds(leds_) {}

  void wakeUp() {
    knobControl.setPosition(delay);
  }

  // TODO: is something wrong with going past finish?
  void update() {
    if (knobControl.updateSettingOnChange(delay)) {
      configChangeFlag = true;
      if (ANIM_DEBUG) debugLog("config change: Stars::delay = ", delay);
    }

    if (timer.hasElapsedWithReset(delay)) {
      lastPosition++;
      if (lastPosition > numPixelsBetweenStars) {
        lastPosition = 0;
      }
      fill_solid(leds, NUMPIXELS, CRGB::Black);
      for (int dot = lastPosition; dot < NUMPIXELS; dot += numPixelsBetweenStars + 1) {
        leds[dot] = CRGB::Coral;
      }
    }
  }

};

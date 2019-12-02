#include "Animation.h"

class Rainbow : public Animation {
 private:
  CRGB *leds;
  int32_t offset = 1;
  KnobControl knobControl{1, 255, true};

 public:
  Rainbow(CRGB leds_[]) : leds(leds_) {}

  void wakeUp() {
    knobControl.setPosition(offset);
  }

  void update() {
    if (knobControl.updateSettingOnChange(offset)) {
      configChangeFlag = true;
      if (ANIM_DEBUG) debugLog("config change: Rainbow::offset = ", offset);
    }

    fill_rainbow(leds, NUMPIXELS, offset, 5);
    
  }

};

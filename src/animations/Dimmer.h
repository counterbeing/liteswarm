#include "Animation.h"
#include <string.h>

class Dimmer : public Animation {
 private:
  int32_t brightness = 100;
  KnobControl knobControl{10, 180, false};

 public:
  Dimmer(CRGB leds_[]) : Animation(leds_) {}

  void wakeUp() {
    knobControl.setPosition(brightness);
  }


  void update() {
    if (knobControl.updateSettingOnChange(brightness)) {
      configChangeFlag = true;
      if (ANIM_DEBUG) debugLog("config change: Dimmer::brightness = ", brightness);
    }

    fill_solid(leds, NUMPIXELS, CHSV(0, 0, brightness));
  }

};

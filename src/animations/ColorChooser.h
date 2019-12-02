#include "Animation.h"

class ColorChooser : public Animation {
 private:
  int32_t hue = 180;
  KnobControl knobControl{0, 255, true};

 public:
  ColorChooser(CRGB leds_[]) : Animation(leds_) {}

  void wakeUp() {
    knobControl.setPosition(hue);
    debugLog("ColorChooser::wakeUp() set position (hue) to ", hue);
  }

  void update() {
    if (knobControl.updateSettingOnChange(hue)) {
      configChangeFlag = true;
      if (ANIM_DEBUG) debugLog("config change: ColorChooser::hue = ", hue);
    }

    fill_solid(leds, NUMPIXELS, CHSV(hue, 255, 255));
  }

};

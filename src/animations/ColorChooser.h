#include "Animation.h"
#include "MyKnob.h"

class ColorChooser : public Animation {
 private:
  KnobSetting hue{180, 0, 255, true};

 public:
  ColorChooser(CRGB leds_[]) : Animation(leds_) {}

 protected:
  void activate() override {
    hue.activate();
  }

  bool updateAnimation(bool justActivated) override {
    if (hue.update()) {
      configChangeFlag = true;
    }

    if (configChangeFlag || justActivated) {
      fill_solid(leds, NUMPIXELS, CHSV(hue.get(), 255, 255));
      return true;
    }

    return false;
  }

};

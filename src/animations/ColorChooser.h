#include "Animation.h"
#include "MyKnob.h"

class ColorChooser : public Animation {
 private:
  KnobSetting hue{180, 0, 255, true};

 public:
  ColorChooser(CRGB leds_[]) : Animation(leds_) {}

  void wakeUp() override {
    hue.activate();
  }

  void update() override {
    if (hue.update()) {
      configChangeFlag = true;
    }

    fill_solid(leds, NUMPIXELS, CHSV(hue.get(), 255, 255));
  }

};

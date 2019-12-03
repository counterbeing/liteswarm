#include "Animation.h"

class Dimmer : public Animation {
 private:
  KnobSetting brightness{100, 10, 180, false};

 public:
  Dimmer(CRGB leds_[]) : Animation(leds_) {}

  void wakeUp() override {
    brightness.activate();
  }

  void update() override {
    if (brightness.update()) {
      configChangeFlag = true;
    }

    fill_solid(leds, NUMPIXELS, CHSV(0, 0, brightness.get()));
  }

};

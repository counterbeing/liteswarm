#include "Animation.h"
#include "MyKnob.h"

class Dimmer : public Animation {
 private:
  KnobSetting brightness{100, 10, 180, false};

 public:
  Dimmer(CRGB leds_[]) : Animation(leds_) {}

 protected:
  void activate() override {
    brightness.activate();
  }

  bool updateAnimation(bool justActivated) override {
    if (brightness.update()) {
      configChangeFlag = true;
    }

    if (configChangeFlag || justActivated) {
      fill_solid(leds, NUMPIXELS, CHSV(0, 0, brightness.get()));
      return true;
    }

    return false;
  }

};

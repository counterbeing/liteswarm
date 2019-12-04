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
    bool configChangeFlag = brightness.update();

    if (configChangeFlag || justActivated) {
      fill_solid(leds, NUMPIXELS, CHSV(0, 0, brightness.get()));
      return true;
    }

    return false;
  }

  uint32_t getKnobPosition() override {
    return brightness.get();
  }

};

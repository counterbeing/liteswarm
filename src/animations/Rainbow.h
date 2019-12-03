#include "Animation.h"

class Rainbow : public Animation {
 private:
  KnobSetting offset{1, 1, 255, true};

 public:
  Rainbow(CRGB leds_[]) : Animation(leds_) {}

 protected:
  void activate() override {
    offset.activate();
  }

  bool updateAnimation(bool justActivated) override {
    if (offset.update()) {
      configChangeFlag = true;
    }

    if (configChangeFlag || justActivated) {
      fill_rainbow(leds, NUMPIXELS, offset.get(), 5);
      return true;
    }
    
    return false;
  }

};

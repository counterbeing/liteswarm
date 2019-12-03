#include "Animation.h"

class Rainbow : public Animation {
 private:
  KnobSetting offset{1, 1, 255, true};

 public:
  Rainbow(CRGB leds_[]) : Animation(leds_) {}

  void wakeUp() override {
    offset.activate();
  }

  void update() override {
    if (offset.update()) {
      configChangeFlag = true;
    }

    fill_rainbow(leds, NUMPIXELS, offset.get(), 5);
    
  }

};

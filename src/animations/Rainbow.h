#include "Animation.h"
#include "MyKnob.h"

class Rainbow : public Animation {
 private:
  KnobSetting offset{1, 1, 255, true};

 public:
  Rainbow(CRGB leds_[]) : Animation(leds_) {}

 protected:
  void activate() override { offset.activate(); }

  bool updateAnimation(const bool justActivated) override {
    bool configChangeFlag = offset.update();

    if (configChangeFlag || justActivated) {
      fill_rainbow(leds, NUMPIXELS, offset.get(), 5);
      return true;
    }

    return false;
  }

  uint32_t getKnobPosition() override { return offset.get(); }

  void setKnobPosition(const uint32_t newPosition) override {
    offset.set(newPosition);
  }
};

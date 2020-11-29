#include "Animation.h"
#include "Knob.h"

class Dimmer : public Animation {
 private:
  Knob brightness{100, 10, 180, false};

 public:
  Dimmer(CRGB leds_[]) : Animation(leds_) {}

 protected:
  void activate() override { brightness.activate(); }

  bool updateAnimation(const bool justActivated) override {
    bool configChangeFlag = brightness.update();

    if (configChangeFlag || justActivated) {
      fill_solid(leds, NUMPIXELS, CHSV(0, 0, brightness.get()));
      return true;
    }

    return false;
  }

  uint32_t getKnobPosition() override { return brightness.get(); }

  void setKnobPosition(const uint32_t newPosition) override {
    brightness.set(newPosition);
  }
};

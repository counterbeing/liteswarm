#include "Animation.h"
#include "MilliTimer.h"

class FindMyBike : public Animation {
 private:
  MilliTimer timer{};

 public:
  FindMyBike(CRGB leds_[]) : Animation(leds_) {}

 protected:
  void activate() override {}

  bool updateAnimation(const bool justActivated) override {
    // FastLED.clear();
    if (timer.hasElapsedWithReset(3000) || justActivated) {
      fill_solid(leds, NUMPIXELS, CRGB::White);
      return true;
    }

    return false;
  }

  uint32_t getKnobPosition() override { return 0; }

  void setKnobPosition(const uint32_t newPosition) override {}

};

#include "Animation.h"
#include "MilliTimer.h"
#include "MyKnob.h"

class Stripes : public Animation {
 private:
  KnobSetting delay{180, 0, 500, false};
  MilliTimer timer{};
  int lastPosition = 0;

 public:
  Stripes(CRGB leds_[]) : Animation(leds_) {}

 protected:
  void activate() override { delay.activate(); }

  bool updateAnimation(bool justActivated) override {
    delay.update();

    int stripeLength = 4;

    if (timer.hasElapsedWithReset(delay.get()) || justActivated) {
      lastPosition++;
      if (lastPosition >= (stripeLength * 2)) {
        lastPosition = 0;
      }
      fill_solid(leds, NUMPIXELS, CRGB::Red);
      for (int dot = lastPosition; dot < NUMPIXELS; dot += (stripeLength * 2)) {
        for (int band = 0; band < stripeLength; band++) {
          leds[dot + band] = CRGB::Purple;
        }
      }
      return true;
    }

    return false;
  }

  uint32_t getKnobPosition() override { return delay.get(); }
};

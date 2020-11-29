#include "Animation.h"
#include "Knob.h"
#include "MilliTimer.h"

class Stripes : public Animation {
 private:
  const static uint8_t STRIPE_LENGTH = 4;
  Knob delay{180, 0, 500, false};
  MilliTimer timer{};
  int lastPosition = 0;

 public:
  Stripes(CRGB leds_[]) : Animation(leds_) {}

 protected:
  void activate() override { delay.activate(); }

  bool updateAnimation(const bool justActivated) override {
    delay.update();

    if (timer.hasElapsedWithReset(delay.get()) || justActivated) {
      lastPosition++;
      if (lastPosition >= (STRIPE_LENGTH * 2)) {
        lastPosition = 0;
      }
      fill_solid(leds, NUMPIXELS, CRGB::Red);
      for (int dot = lastPosition; dot < NUMPIXELS;
           dot += (STRIPE_LENGTH * 2)) {
        for (int band = 0; band < STRIPE_LENGTH; band++) {
          leds[dot + band] = CRGB::Purple;
        }
      }
      return true;
    }

    return false;
  }

  uint32_t getKnobPosition() override { return delay.get(); }

  void setKnobPosition(const uint32_t newPosition) override {
    delay.set(newPosition);
  }
};

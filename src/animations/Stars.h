#include "Animation.h"
#include "MilliTimer.h"
#include "MyKnob.h"

// TODO: Stars is basically the same as Stripes but with different settings.
// Collapse them?
class Stars : public Animation {
 private:
  KnobSetting delay{100, 0, 1000, false};
  MilliTimer timer{};
  int lastPosition = 0;
  int32_t numPixelsBetweenStars = 13;

 public:
  Stars(CRGB leds_[]) : Animation(leds_) {}

 protected:
  void activate() override { delay.activate(); }

  // TODO: is something wrong with going past finish?
  bool updateAnimation(const bool justActivated) override {
    delay.update();

    if (timer.hasElapsedWithReset(delay.get()) || justActivated) {
      lastPosition++;
      if (lastPosition > numPixelsBetweenStars) {
        lastPosition = 0;
      }
      fill_solid(leds, NUMPIXELS, CRGB::Black);
      for (int dot = lastPosition; dot < NUMPIXELS;
           dot += numPixelsBetweenStars + 1) {
        leds[dot] = CRGB::Coral;
      }
      return true;
    }

    return false;
  }

  uint32_t getKnobPosition() override { return delay.get(); }

  void setKnobPosition(const uint32_t newPosition) override { delay.set(newPosition); }

};

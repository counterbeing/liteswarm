#include "Animation.h"
#include "MilliTimer.h"
#include "MyKnob.h"

class Strobe : public Animation {
 private:
  KnobSetting blinkDuration{250, 50, 500, false};
  MilliTimer timer{};
  bool goWhite = true;

 public:
  Strobe(CRGB leds_[]) : Animation(leds_) {}

 protected:
  void activate() override { blinkDuration.activate(); }

  bool updateAnimation(const bool justActivated) override {
    blinkDuration.update();

    if (timer.hasElapsedWithReset(blinkDuration.get()) || justActivated) {
      fill_solid(leds, NUMPIXELS, goWhite ? CRGB::White : CRGB::Black);
      goWhite = !goWhite;
      return true;
    }

    return false;
  }

  uint32_t getKnobPosition() override { return blinkDuration.get(); }

  void setKnobPosition(const uint32_t newPosition) override {
    blinkDuration.set(newPosition);
  }
};

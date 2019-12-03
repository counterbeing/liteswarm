#include "Animation.h"
#include "MilliTimer.h"

class Strobe : public Animation {
 private:
  bool goWhite = true;
  MilliTimer timer{};
  KnobSetting blinkDuration{250, 50, 500, false};

 public:
  Strobe(CRGB leds_[]) : Animation(leds_) {}

 protected:
  void activate() override {
    blinkDuration.activate();
  }

  bool updateAnimation(bool justActivated) override {
    if (blinkDuration.update()) {
      configChangeFlag = true;
    }

    if (timer.hasElapsedWithReset(blinkDuration.get()) || justActivated) {
      fill_solid(leds, NUMPIXELS, goWhite ? CRGB::White : CRGB::Black);
      goWhite = !goWhite;
      return true;
    }

    return false;
  }

};

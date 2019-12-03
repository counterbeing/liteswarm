#include "Animation.h"
#include "MilliTimer.h"

class Strobe : public Animation {
 private:
  bool goWhite = true;
  MilliTimer timer{};
  KnobSetting blinkDuration{250, 50, 500, false};

 public:
  Strobe(CRGB leds_[]) : Animation(leds_) {}

  void wakeUp() override {
    blinkDuration.activate();
  }

  void update() override {
    if (blinkDuration.update()) {
      configChangeFlag = true;
    }

    if (timer.hasElapsedWithReset(blinkDuration.get())) {
      fill_solid(leds, NUMPIXELS, goWhite ? CRGB::White : CRGB::Black);
      goWhite = !goWhite;
    }
  }

};

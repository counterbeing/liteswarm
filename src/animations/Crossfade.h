#include "Animation.h"
#include "MyKnob.h"
#include "MilliTimer.h"

class Crossfade : public Animation {
 private:
  KnobSetting delay{35, 5, 200, false};
  MilliTimer timer{};
  uint8_t hue = 0;

 public:
  Crossfade(CRGB leds_[]) : Animation(leds_) {}

 protected:
  void activate() override {
    delay.activate();
  }

  bool updateAnimation(bool justActivated) override {
    delay.update();

    if (timer.hasElapsedWithReset(delay.get()) || justActivated) {
      fill_solid(leds, NUMPIXELS, CHSV(hue++, 255, 255));
      return true;
    }

    return false;
  }

  uint32_t getKnobPosition() override {
    return delay.get();
  }

};

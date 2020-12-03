#pragma once

#include "Animation.h"

class Crossfade : public Animation {
 private:
  MilliTimer timer{};
  uint8_t hue = 0;
  KnobSettings knobSettings = {35, 5, 200, false};

 public:
  Crossfade(MasterState * masterState)
      : Animation{masterState} {}

 protected:
  void activate() override { knob->activate(knobSettings); }

  bool updateAnimation(const bool justActivated) override {
    knob->update();

    if (timer.hasElapsedWithReset(knob->get()) || justActivated) {
      fill_solid(leds, NUMPIXELS, CHSV(hue++, 255, 255));
      return true;
    }

    return false;
  }
};

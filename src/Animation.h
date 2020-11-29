#pragma once

#include "BaseControllers.h"
#include "FastLED.h"
#include "config.h"

class Animation : public BaseController {
 protected:
  CRGB * leds;
  Knob * knob;

  virtual void loop(const bool justActivated) override {
    if (updateAnimation(justActivated))
      FastLED.show();
  };

  virtual bool updateAnimation(const bool justActivated) = 0;

 public:
  Animation(CRGB leds_[], Knob * knob)
      : leds(leds_)
      , knob(knob){};

  virtual uint32_t getKnobPosition() = 0;

  virtual void setKnobPosition(const uint32_t newPosition) = 0;
};

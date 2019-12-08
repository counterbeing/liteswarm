#ifndef ANIMATION_H
#define ANIMATION_H
#include "BaseControllers.h"
#include "FastLED.h"
#include "config.h"

class Animation : public BaseController {
 protected:
  CRGB * leds;

  virtual void loop(const bool justActivated) override {
    if (updateAnimation(justActivated))
      FastLED.show();
  };

  virtual bool updateAnimation(const bool justActivated) = 0;

 public:
  Animation(CRGB leds_[]) : leds(leds_){};

  virtual uint32_t getKnobPosition() = 0;

  virtual void setKnobPosition(const uint32_t newPosition) = 0;
};

#endif
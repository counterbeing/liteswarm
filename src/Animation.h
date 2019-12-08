#ifndef ANIMATION_H
#define ANIMATION_H
#include "BaseControllers.h"
#include "FastLED.h"
#include "config.h"

class Animation : public BaseController {
 protected:
  CRGB * leds;

  virtual bool updateAnimation(bool justActivated) = 0;

  virtual void loop(bool justActivated) override {
    if (updateAnimation(justActivated))
      FastLED.show();
  };

 public:
  Animation(CRGB leds_[]) : leds(leds_){};

  virtual uint32_t getKnobPosition() { return 0; }
};

#endif
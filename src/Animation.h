#ifndef ANIMATION_H
#define ANIMATION_H
#include "FastLED.h"
#include "config.h"
#include "BaseControllers.h"

class Animation : public BaseController {
 protected:
  CRGB *leds;
  bool configChangeFlag = false;

  virtual bool updateAnimation(bool justActivated) = 0;

  virtual void loop(bool justActivated) override {
    configChangeFlag = false;
    if (updateAnimation(justActivated))
      FastLED.show();
  };

 public:
  Animation(CRGB leds_[]) : leds(leds_) {};

  virtual bool hasConfigChanged() { return configChangeFlag; }
};

#endif
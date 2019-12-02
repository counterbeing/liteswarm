#ifndef ANIMATION_H
#define ANIMATION_H
#include <MyKnob.h>
#include "FastLED.h"
#include "config.h"

class Animation {
 protected:
  bool configChangeFlag = false;

  virtual void update() = 0;

 public:
  virtual void wakeUp() = 0;
  virtual void loop() {
    configChangeFlag = false;
    update();
  };
  virtual bool hasConfigChanged() { return configChangeFlag; }
  Animation(){};
};
#endif
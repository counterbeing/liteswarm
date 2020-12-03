#pragma once

#include "BaseControllers.h"
#include "FastLED.h"
#include "Knob.h"
#include "KnobSettings.h"
#include "MasterState.h"
#include "MilliTimer.h"
#include "config.h"

class Animation : public BaseController {
 protected:
  CRGB * leds;
  Knob * knob;
  KnobSettings * knobSettings;

  // Optionally override the loop if you need more control for some reason.
  virtual void loop(const bool justActivated) override {
    if (updateAnimation(justActivated))
      FastLED.show();
  };

  // This function is defined as virtual so that it must be defined within each
  // animation. This is where the work of moving to the next frame of the
  // animation happens.
  virtual bool updateAnimation(const bool justActivated) = 0;

  // Function called whenever a new animation is activated. This allows us to
  // simply declare the presets for a given animation without explicitly calling
  // the code in each animation, keeping the animation code itself as tidy as
  // possible.
  void activate() { knob->activate(*knobSettings); };

 public:
  Animation(MasterState * masterState)
      : knob(masterState->knob) {}

  // virtual uint32_t getKnobPosition() = 0;

  // virtual void setKnobPosition(const uint32_t newPosition) = 0;
};

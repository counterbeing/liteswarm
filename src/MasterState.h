#pragma once
#include "./Knob.h"
#include "Animation.h"
#include "FastLED.h"

class MasterState {
 private:
  int numberOfAnmia;
  int animationIndex;
  int knobSetting;
  bool manuallySet;

 public:
  Knob * knob;
  CRGB * leds;
  // Animation * animations;

  MasterState(Knob * knob, struct CRGB * leds_)
      : animationIndex(0)
      , knobSetting(0)
      , manuallySet(false)
      , knob(knob)
      , leds(leds_) {}

  void manualUpdateIncrementAnimation() {
    animationIndex =
        (animationIndex == NUMBER_OF_ANMIATIONS - 1) ? 1 : animationIndex + 1;
    manuallySet = true;
  }

  void manuallyChangeKnobSetting(int newKnobSetting) {
    knobSetting = newKnobSetting;
    manuallySet = true;
  }

  void setup() {

  }

  void markAsApplied() { manuallySet = false; }
};
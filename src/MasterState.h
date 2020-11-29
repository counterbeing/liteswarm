#pragma once
#include "./Knob.h"
#include "FastLED.h"

class MasterState {
 private:
  int numberOfAnimations;
  int animationIndex;
  int knobSetting;
  bool manuallySet;

 public:
  Knob * knob;
  CRGB leds;

  MasterState(int numberOfAnimations, Knob * knob, CRGB leds_)
      : numberOfAnimations(numberOfAnimations)
      , animationIndex(0)
      , knobSetting(0)
      , manuallySet(false)
      , knob(knob)
      , leds(leds_) {}

  void manualUpdateIncrementAnimation() {
    animationIndex =
        (animationIndex == numberOfAnimations - 1) ? 1 : animationIndex + 1;
    manuallySet = true;
  }

  void manuallyChangeKnobSetting(int newKnobSetting) {
    knobSetting = newKnobSetting;
    manuallySet = true;
  }

  void markAsApplied() { manuallySet = false; }
};
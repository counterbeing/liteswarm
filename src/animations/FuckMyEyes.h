#include "Animation.h"

class FuckMyEyes : public Animation {
 private:
  int initialPosition = 0;
  int start = 0;
  int finish = 300;
  bool initialized = false;
  bool loopRotary = false;
  MyKnob &knob;
  CRGB *leds;
  int lastColor = 0;
  AnimationVariables &aniVars;

  void setup() {
    knob.setDefaults(initialPosition, start, finish, loopRotary);
  };

  void loop() {
    int dlay = knob.confine();
    if (nonBlockDelay(aniVars.var1)) {
      switch (lastColor) {
        case 0:
          fill_solid(leds, NUMPIXELS, CRGB::Red);
          lastColor = 1;
          break;
        case 1:
          fill_solid(leds, NUMPIXELS, CRGB::Green);
          lastColor = 2;
          break;
        case 2:
          fill_solid(leds, NUMPIXELS, CRGB::Blue);
          lastColor = 0;
          break;
      }
    }
  }

 public:
  FuckMyEyes(MyKnob &knob_, CRGB leds_[], AnimationVariables &aniVars_)
      : knob(knob_), leds(leds_), aniVars(aniVars_) {}
};

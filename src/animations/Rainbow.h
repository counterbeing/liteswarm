#include "Animation.h"

class Rainbow : public Animation {
 private:
  int initialPosition = 1;
  int start = 1;
  int finish = 255;
  bool initialized = false;
  bool loopRotary = true;
  MyKnob &knob;
  CRGB *leds;
  AnimationVariables &aniVars;

  void setup() {
    knob.setDefaults(initialPosition, start, finish, loopRotary);
  };

  void loop() {
    int offset = knob.confine();
    fill_rainbow(leds, NUMPIXELS,aniVars.var1, 5);
  }

 public:
  Rainbow(MyKnob &knob_, CRGB leds_[], AnimationVariables &aniVars_)
      : knob(knob_), leds(leds_), aniVars(aniVars_) {}
};

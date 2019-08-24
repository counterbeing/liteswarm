#include "Animation.h"

class ColorChooser : public Animation {
 private:
  int initialPosition = 180;
  int start = 0;
  int finish = 255;
  bool initialized = false;
  int head = 0;
  bool loopRotary = true;
  MyKnob &knob;
  CRGB *leds;
  AnimationVariables &aniVars;

  void setup() {
    knob.setDefaults(initialPosition, start, finish, loopRotary);
  };

  void loop() {
    knob.confine();
    fill_solid(leds, NUMPIXELS, CHSV(aniVars.var1, 255, 255));
  }

 public:
  ColorChooser(MyKnob &knob_, CRGB leds_[], AnimationVariables aniVars_)
      : knob(knob_), leds(leds_), aniVars(aniVars_) {}
};

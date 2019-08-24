#include "Animation.h"

class Dimmer : public Animation {
 private:
  int initialPosition = 100;
  int start = 10;
  int finish = 180;
  bool initialized = false;
  int head = 0;
  bool loopRotary = false;
  int lastPosition = 0;
  MyKnob &knob;
  CRGB *leds;
  AnimationVariables &aniVars;

  void setup() {
    knob.setDefaults(initialPosition, start, finish, loopRotary);
  };

  void loop() {
    int brightness = knob.confine();
    fill_solid(leds, NUMPIXELS, CHSV(0, 0, aniVars.var1));
  }

 public:
  Dimmer(MyKnob &knob_, CRGB leds_[], AnimationVariables &aniVars_)
      : knob(knob_), leds(leds_), aniVars(aniVars_) {}
};

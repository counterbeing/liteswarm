#include <MyKnob.h>
#include "Animation.h"
#include "FastLED.h"

class Crossfade : public Animation {
 private:
  int initialPosition = 40;
  int start = 5;
  int finish = 200;
  bool initialized = false;
  int head = 0;
  bool loopRotary = false;
  MyKnob &knob;
  AnimationVariables &aniVars;
  CRGB *leds;

  void loop() {
    knob.confine();
    static uint8_t hue = 0;
    if (nonBlockDelay(aniVars.var1)) {
      fill_solid(leds, NUMPIXELS, CHSV(hue++, 255, 255));
    }
  }

 public:
  void setup() {
    knob.setDefaults(initialPosition, start, finish, loopRotary);
  };

  Crossfade(MyKnob &knob_, CRGB leds_[], AnimationVariables &aniVars_)
      : knob(knob_), leds(leds_), aniVars(aniVars_) {}
};

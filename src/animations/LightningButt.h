#include "Animation.h"

class LightningButt : public Animation {
 private:
  int initialPosition = 0;
  int start = 0;
  int finish = 300;
  bool initialized = false;
  bool loopRotary = false;
  MyKnob &knob;
  CRGB *leds;
  int lastColor = 0;

  void setup() {
    knob.setDefaults(initialPosition, start, finish, loopRotary);
  };

  void loop() {
    int dlay = knob.confine();
    if (nonBlockDelay(dlay)) {
      fill_solid(leds, NUMPIXELS, CRGB::LawnGreen);
    }
  }

 public:
  LightningButt(MyKnob &knob_, CRGB leds_[]) : knob(knob_), leds(leds_) {}
};

#include "Animation.h"

class Stars : public Animation {
 private:
  int initialPosition = 100;
  int start = 0;
  int finish = 400;
  bool initialized = false;
  int head = 0;
  bool loopRotary = false;
  int lastPosition = 0;
  MyKnob &knob;
  CRGB *leds;

  void setup() {
    knob.setDefaults(initialPosition, start, finish, loopRotary);
  };

  void loop() {
    int delay = knob.confine();

    int stripeLength = 7;

    if (nonBlockDelay(delay)) {
      lastPosition++;
      if (lastPosition >= (stripeLength * 2)) {
        lastPosition = 0;
      }
      fill_solid(leds, NUMPIXELS, CRGB::Black);
      for (int dot = lastPosition; dot < NUMPIXELS; dot += (stripeLength * 2)) {
        for (int band = 0; band < 1; band++) {
          leds[dot + band] = CRGB::Coral;
        }
      }

      
    }
  }

 public:
  Stars(MyKnob &knob_, CRGB leds_[]) : knob(knob_), leds(leds_) {}
};
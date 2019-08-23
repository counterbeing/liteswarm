#include "Animation.h"

class Stripes : public Animation {
 private:
  int initialPosition = 180;
  int start = 0;
  int finish = 500;
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

    int stripeLength = 4;

    if (nonBlockDelay(delay)) {
      if (ANIMDEBUG) {
        Serial.println(lastPosition);
      }
      lastPosition++;
      if (lastPosition >= (stripeLength * 2)) {
        lastPosition = 0;
      }
      fill_solid(leds, NUMPIXELS, CRGB::Red);
      for (int dot = lastPosition; dot < NUMPIXELS; dot += (stripeLength * 2)) {
        for (int band = 0; band < stripeLength; band++) {
          leds[dot + band] = CRGB::Purple;
        }
      }

      
    }
  }

 public:
  Stripes(MyKnob &knob_, CRGB leds_[]) : knob(knob_), leds(leds_) {}
};

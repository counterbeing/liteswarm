#include "Animation.h"

class Stripes : public Animation {
 private:
  int initialPosition = 180;
  int start = 0;
  // int finish = 500;
  bool initialized = false;
  int head = 0;
  bool loopRotary = false;
  int lastPosition = 0;
  MyKnob &knob;
  CRGB *leds;

  void setup() {
    knob.setDefaults(initialPosition, start, finish, loopRotary);
  };


// TODO figure out why Stripes causes kernel panic
  void loop() {
    Serial.print("Stripes.h knob.get() before knob.confine(): ");
    Serial.println(knob.get());
    int delay = knob.confine();
    Serial.print("Stripes.h delay from knob.confine(): ");
    Serial.println(delay);

    int stripeLength = 4;

    if (nonBlockDelay(delay)) {
      lastPosition++;
      if (lastPosition >= (stripeLength * 2)) {
        lastPosition = 0;
      }
      fill_solid(leds, NUMPIXELS, CRGB::Red);
      for (int dot = lastPosition; dot < NUMPIXELS; dot += (stripeLength * 2)) {
        for (int band = 0; band < stripeLength; band++) {
          leds[dot + band] = CRGB::Purple;
          Serial.println(dot + band);
        }
      }

      
    }
  }

 public:
  Stripes(MyKnob &knob_, CRGB leds_[]) : knob(knob_), leds(leds_) {}
};
#include "Animation.h"

class Strobe : public Animation {
 private:
  int initialPosition = 50;
  int start = 0;
  int finish = 300;
  bool initialized = false;
  int head = 0;
  bool loopRotary = false;
  MyKnob &knob;
  CRGB *leds;
  bool goWhite = true;
  unsigned long startMillis = 0;
  const static unsigned long strobeDuration = 750;

  void setup() {
    knob.setDefaults(initialPosition, start, finish, loopRotary);
  };

  void loop() {
    if (nonBlockDelay(50)) {
      if (goWhite) {
        fill_solid(leds, NUMPIXELS, CRGB::White);
        goWhite = false;
      }
      else {
        fill_solid(leds, NUMPIXELS, CRGB::Black);
        goWhite = true;
      }      
    }
  }

  bool nonBlockDelaySinceStart(unsigned int interval) {
    unsigned long current_millis = millis();
    if ((current_millis - startMillis) > interval) {
      return true;
    }
    return false;
  }

 public:
  Strobe(MyKnob &knob_, CRGB leds_[]) : knob(knob_), leds(leds_) {}

  bool is_done() {
    if (startMillis == 0) {
      startMillis = millis();
    }

    if (nonBlockDelaySinceStart(strobeDuration)) {
      startMillis = 0;
      return true;
    }
    return false;
  }
};

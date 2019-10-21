#include "Animation.h"

class LightningButt : public Animation {
 private:
  int initialPosition = 127;
  int start = 0;
  int finish = 127;
  bool initialized = false;
  bool loopRotary = false;
  MyKnob &knob;
  CRGB *leds;
  int lastBrightness = 0;
  long lastChangeTime;

  void setup() {
    lastChangeTime = millis();
    knob.setDefaults(initialPosition, start, finish, loopRotary);
  };

  void loop() {
    if(knob.manuallyChanged()) {
      lastChangeTime = millis();
    }
    int brightness = knob.confine();
    int timeSinceLastChange = millis() - lastChangeTime;
    fill_solid(leds, NUMPIXELS, CHSV(90, 255, (brightness * 2)));


    if(timeSinceLastChange > 200) {
    if (nonBlockDelay(10)) {
      knob.set(brightness - 1);
    }
    }
  }

 public:
  LightningButt(MyKnob &knob_, CRGB leds_[]) : knob(knob_), leds(leds_) {}
};

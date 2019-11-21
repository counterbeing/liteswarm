#include "../Animation.h"

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
  long lastManualChangeTime;
  int automaticInterval = 10000;

  void setup() {
    lastChangeTime = millis();
    knob.setDefaults(initialPosition, start, finish, loopRotary);
  };

  void loop() {
    int brightness = knob.confine();
    if(knob.manuallyChanged()|| (lastBrightness == 0 && brightness > 0)) {
      lastChangeTime = millis();
    }
    if(knob.manuallyChanged()) {
      lastManualChangeTime = millis();
    }
    lastBrightness = brightness;
    fill_solid(leds, NUMPIXELS, CHSV(90, 255, (brightness * 2)));
    int timeSinceLastChange = millis() - lastChangeTime;
    // Serial.println(timeSinceLastChange);
    if(timeSinceLastChange > 200) {
      if (nonBlockDelay(10)) {
        knob.set(brightness - 1);
      }
    }
    if(timeSinceLastChange > automaticInterval) {
      automaticInterval = random(4000, 20000);
      lastChangeTime = millis();
      knob.fakeManualChange();
      knob.set(255);
    }
  }

 public:
  LightningButt(MyKnob &knob_, CRGB leds_[]) : knob(knob_), leds(leds_) {}
};

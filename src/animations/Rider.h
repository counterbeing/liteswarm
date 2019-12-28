#include "Animation.h"

class Rider : public Animation {
 private:
  int initialPosition = 60;
  int start = 5;
  int finish = 400;
  bool initialized = false;
  int head = 0;
  bool loopRotary = false;
  MyKnob &knob;
  CRGB *leds;

  void setup() {
    knob.setDefaults(initialPosition, start, finish, loopRotary);
  };

  void loop() {
    int dlay = knob.confine();
    // if (nonBlockDelay(dlay)) {
    //   static uint8_t hue = 0;
    //   head = remapInRange(++head);
    //   leds[head] = CRGB::Black;
    //   leds[head] = CHSV(hue++, 255, 150);
    //   head = remapInRange(++head);
    //   leds[head] = CHSV(hue++, 255, 255);
      
    // }
      static byte riderPos = 0;

  // startup tasks
//   if (effectInit == false) {
//     effectInit = true;
//     effectDelay = 5;
//     riderPos = 0;
//   }

  // Draw one frame of the animation into the LED array
  for (byte x = 0; x < kMatrixWidth; x++) {
    int brightness = abs(x * (256 / kMatrixWidth) - triwave8(riderPos) * 2 + 127) * 3;
    if (brightness > 255) brightness = 255;
    brightness = 255 - brightness;
    CRGB riderColor = CHSV(120, 255, brightness);
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = riderColor;
    }
  }

  riderPos++; // byte wraps to 0 at 255, triwave8 is also 0-255 periodic
  }

 public:
  Rider(MyKnob &knob_, CRGB leds_[]) : knob(knob_), leds(leds_) {}
};
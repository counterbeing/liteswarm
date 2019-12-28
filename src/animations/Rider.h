#include "Animation.h"

class Rider : public Animation {
 private:
  int initialPosition = 100;
  int start = 1;
  int finish = 400;
  bool loopRotary = false;
  bool initialized = false;
  bool knobControlsColor = false;
  int head = 0;
  MyKnob &knob;
  CRGB *leds;

  void setup() {
    // randomly pick if knob controls color or speed (default, params above)
    // if true set knob params for changing color
    // BUG NOT WORKING YET
    if(rand() % 2 == 0){
      knobControlsColor = true;
      initialPosition = 0;
      start = 0;
      finish = 255;
      loopRotary = true;
    }

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

    if(knobControlsColor || nonBlockDelay(dlay)) { 
      // Draw one frame of the animation into the LED array
      for (byte x = 0; x < kMatrixWidth; x++) {
        int brightness = abs(x * (256 / kMatrixWidth) - triwave8(riderPos) * 2 + 127) * 3;
        if (brightness > 255) brightness = 255;
        brightness = 255 - brightness;
        
        CRGB riderColor = CHSV(0, 255, brightness);
        if(knobControlsColor){
          CRGB riderColor = CHSV(dlay, 255, brightness);
        }
        
        for (byte y = 0; y < kMatrixHeight; y++) {
          leds[XY(x, y)] = riderColor;
        }
      }
    riderPos++; // byte wraps to 0 at 255, triwave8 is also 0-255 periodic
    }
  }

 public:
  Rider(MyKnob &knob_, CRGB leds_[]) : knob(knob_), leds(leds_) {}
};
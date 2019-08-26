#include "Animation.h"
#include "config.h"

class Stripes : public Animation {
 private:
  int initialPosition = 180;
  int start = 0;
  int finish = 500;
  bool initialized = false;
  int head = 0;
  bool loopRotary = false;
  int lastPosition = 0;
  int stripeLength = 4;

  MyKnob &knob;
  CRGB *leds;
  AnimationVariables &aniVars;

  void setup() {
    randomSeed(analogRead(10));
    int randNumber = random(1, 3);
    Serial.println(randNumber);
    Serial.println("\n");
    aniVars.activeVar = randNumber;
    // set defaults - will be overwritten if someone else sends
    aniVars.var1 = stripeLength;
    aniVars.var2 = initialPosition;
    switch (randNumber) {
      case 1:
        if (ANIM_DEBUG) Serial.println("Changing stripe length");
        knob.setDefaults(stripeLength, 1, 20, false);
        
        break;
      case 2:
        if (ANIM_DEBUG) Serial.println("Changing speed");
        knob.setDefaults(initialPosition, start, finish, loopRotary);
        break;
      default:
        if (ANIM_DEBUG) Serial.println("default case");
    }
  };

  void loop() {
    // int delay = knob.confine();
    knob.confine();
    stripeLength = aniVars.var1;
    if (stripeLength > 18) Serial.println(stripeLength);

    if (nonBlockDelay(150)) {
      lastPosition++;
      if (lastPosition >= (stripeLength * 2)) {    // pretty sure lastPosition is getting really big somehow,
        lastPosition = 0;                          // maybe making the loop take a long time 
      }
      if (ANIM_DEBUG) {
        Serial.print("\n\tlastPosition: ");
        Serial.print(lastPosition);
        Serial.print("\n\taniVars.var1: ");
        Serial.print(aniVars.var1);
        Serial.print("\n\taniVars.var2: ");
        Serial.println(aniVars.var2);
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
  Stripes(MyKnob &knob_, CRGB leds_[], AnimationVariables &aniVars_)
      : knob(knob_), leds(leds_), aniVars(aniVars_) {}
};

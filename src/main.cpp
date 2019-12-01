#include <Arduino.h>
#define FASTLED_INTERNAL
#include <Encoder.h>
#include <MyKnob.h>
#include <SPI.h>
#include "FastLED.h"
#include "Radio.h"
#include "config.h"
#include "DebugLog.h"

#include "animations/ColorChooser.h"
#include "animations/Crossfade.h"
#include "animations/DiamondNecklace.h"
#include "animations/Dimmer.h"
#include "animations/FuckMyEyes.h"
#include "animations/Race.h"
#include "animations/Rainbow.h"
#include "animations/Stars.h"
#include "animations/Stripes.h"
#include "animations/Strobe.h"

// Pins for the rotary
uint8_t rotary1 = 2;
uint8_t rotary2 = 3;

int buttonPin = A0;
bool offMode = false;

CRGB leds[NUMPIXELS];
int feedbackPattern = -1;
MyKnob knob(rotary1, rotary2, offMode, feedbackPattern);

// Load animations...
Crossfade crossfade(knob, leds);
ColorChooser color_chooser(knob, leds);
Race race(knob, leds);
Stars stars(knob, leds);
Rainbow rainbow(knob, leds);
FuckMyEyes fuck_my_eyes(knob, leds);
Stripes stripes(knob, leds);
DiamondNecklace diamond_necklace(knob, leds);
Dimmer dimmer(knob, leds);
Strobe strobe(knob, leds);

const int NUM_ANIMATONS = 9;
Animation* animations[NUM_ANIMATONS] = {
    &crossfade, &color_chooser,    &race,  &stars, &rainbow, &fuck_my_eyes,
    &stripes,   &diamond_necklace, &dimmer};

int animation_index = 0;
Radio radio(knob, animation_index);

class BaseController {
 private:
   bool activate = false;

 public:
  void setAsActive() { activate = true; }

  virtual void run() {
    if (activate) {
      activate = false;
      setup();
    }
    loop();
  }

 protected:
  virtual void setup() = 0;
  virtual void loop() = 0;
};

class OffModeController : public BaseController {
 protected:
  virtual void setup() {
  }

  virtual void loop() {
      fill_solid(leds, NUMPIXELS, CRGB::Black);
      FastLED.show();
  }
};

class AnimationModeController : public BaseController {
 private:
  int previous_animation_index = -1;

 protected:
  virtual void setup() {}

  virtual void loop() {
    bool animationChanged = (animation_index != previous_animation_index);

    if (animationChanged) {
      if (animation_index >= NUM_ANIMATONS)
        animation_index = 0;

      if (KNOB_DEBUG) {
        debugLog("");
        debugLog("Animation Index: ", animation_index);
      }
    }

    Animation *current_animation = animations[animation_index];

    if (animationChanged) {
      current_animation->setup();
      previous_animation_index = animation_index;
    }

    current_animation->loop();

    FastLED.show();
  }
};

class MasterController {
 private:
  OffModeController offModeController{};
  AnimationModeController animationModeController{};

 public:
  void loop() {
    button_debouncer.update();
    knob.check(&animation_index);
    if (offMode) {
      offModeController.run();
    }
    else {
      radio.check();
      animationModeController.run();
    }
  }

} masterController;

void setup() {
#ifdef SCARF_WS2811
  FastLED.addLeds<WS2811, DATAPIN, GRB>(leds, NUMPIXELS);
#endif
#ifdef SCARF_SK9822
  FastLED.addLeds<SK9822, DATAPIN, CLOCKPIN, BGR>(leds, NUMPIXELS);
#endif

#ifdef SCARF_DOTSTAR
  FastLED.addLeds<DOTSTAR, DATAPIN, CLOCKPIN, BGR>(leds, NUMPIXELS);
#endif

#ifdef BIGRED_WS2815
  FastLED.addLeds<WS2811, DATAPIN, BGR>(leds, NUMPIXELS);
#endif
  Serial.begin(57600);

  button_debouncer.attach(buttonPin, INPUT_PULLUP);
  button_debouncer.interval(5);
  radio.setup();
}

void loop() {
  masterController.loop();
}
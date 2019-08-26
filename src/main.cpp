#include <Arduino.h>
#define FASTLED_INTERNAL
#include <Encoder.h>
#include <MyKnob.h>
#include <SPI.h>
#include "FastLED.h"
#include "Radio.h"
#include "State.h"
#include "config.h"

#include "animations/ColorChooser.h"
#include "animations/Crossfade.h"
#include "animations/DiamondNecklace.h"
#include "animations/Dimmer.h"
#include "animations/FuckMyEyes.h"
#include "animations/Race.h"
#include "animations/Rainbow.h"
#include "animations/Stars.h"
#include "animations/Stripes.h"

// Pins for the rotary
uint8_t rotary1 = 2;
uint8_t rotary2 = 3;

int buttonPin = A0;
bool offMode = false;

AnimationVariables aniVars = {0, 0, 0};

CRGB leds[NUMPIXELS];
int feedbackPattern = -1;
MyKnob knob(rotary1, rotary2, offMode, feedbackPattern, aniVars);

// Load animations...
Crossfade crossfade(knob, leds, aniVars);
ColorChooser color_chooser(knob, leds, aniVars);
Race race(knob, leds, aniVars);
Stars stars(knob, leds, aniVars);
Rainbow rainbow(knob, leds, aniVars);
FuckMyEyes fuck_my_eyes(knob, leds, aniVars);
Stripes stripes(knob, leds, aniVars);
DiamondNecklace diamond_necklace(knob, leds, aniVars);
Dimmer dimmer(knob, leds, aniVars);

int animation_index = 0;
Radio radio(knob, animation_index, aniVars);

// Animation *current_animation = &rainbow;
Animation *current_animation = &crossfade;
int previous_animation_index = -1;

int feedbackLength = 800;
long feedbackEnd = (millis() + feedbackLength);

void runAdjustments() {
  if (feedbackPattern < 0) {
    return;
  }
  if (feedbackPattern > 0) {
    // Serial.println("STARTING FEEBACK");
    feedbackEnd = (millis() + feedbackLength);
  }
  feedbackPattern = 0;

  int currentLapse = feedbackEnd - millis();
  if (currentLapse < 0) {
    feedbackPattern = -1;
  }

  CRGB color1;
  CRGB color2;

  switch (feedbackPattern) {
    case 0:
      color1 = CRGB::Blue;
      color2 = CRGB::Green;
      break;
    case 1:
      color1 = CRGB::Black;
      color2 = CRGB::Red;
      break;
  }

  if ((currentLapse / 100) % 2 == 0) {
    fill_solid(leds, NUMPIXELS, color1);
  } else {
    fill_solid(leds, NUMPIXELS, color2);
  }
}
void playAnimation() {
  if (animation_index != previous_animation_index) {
    if (animation_index > 2) animation_index = 0;
    // BUG CAUTION
    // never follow one animation function immediately with itself in the the
    // next case

    if (KNOB_DEBUG) {
      Serial.print("Animation Index: ");
      Serial.println(animation_index);
    }

    switch (animation_index) {
      case 0:
        current_animation = &stripes;

        break;
      case 1:
        current_animation = &color_chooser;
        break;
      case 2:
        current_animation = &race;
        break;
      case 3:
        current_animation = &stars;
        break;
      case 4:
        current_animation = &rainbow;
        break;
      case 5:
        current_animation = &fuck_my_eyes;
        break;
      case 6:
        current_animation = &crossfade;

        break;
      case 7:
        current_animation = &diamond_necklace;
        break;
      case 8:
        current_animation = &dimmer;
        break;
      default:
        // Serial.println("\n\nWARN: default animation switch case");
        break;
    }
    current_animation->setup();
    previous_animation_index = animation_index;
  }
  current_animation->run();

  runAdjustments();

  FastLED.show();
}

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
  button_debouncer.update();
  knob.check(&animation_index);
  if (offMode) {
    fill_solid(leds, NUMPIXELS, CRGB::Black);
    FastLED.show();
    return;
  }
  radio.check();
  playAnimation();
}
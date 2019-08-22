#include <Arduino.h>
#define FASTLED_INTERNAL
#include "FastLED.h"
#include <SPI.h>
#include <MyKnob.h>
#include <Encoder.h>
#include "config.h"
#include "Radio.h"

#include "animations/ColorChooser.h"
#include "animations/Crossfade.h"
#include "animations/DiamondNecklace.h"
#include "animations/Dimmer.h"
#include "animations/Stripes.h"
#include "animations/Stars.h"
#include "animations/FuckMyEyes.h"
#include "animations/Race.h"
#include "animations/Rainbow.h"

// Pins for the rotary
uint8_t rotary1 = 2;
uint8_t rotary2 = 3;

int buttonPin = A0;
bool offMode = false;

CRGB leds[NUMPIXELS];
MyKnob knob(rotary1, rotary2, offMode);

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

int animation_index = 0;
Radio radio(knob, animation_index);

// Animation *current_animation = &rainbow;
Animation *current_animation = &crossfade;
int previous_animation_index = -1;

void playAnimation()
{
  if (animation_index != previous_animation_index)
  {
    if (animation_index > 8)
      animation_index = 0;
    // BUG CAUTION
    // never follow one animation function immediately with itself in the the
    // next case

    if (KNOBDEBUG) {
      Serial.print("Animation Index: ");
      Serial.println(animation_index);
    }

    switch (animation_index)
    {
    case 0:
      current_animation = &crossfade;
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
      current_animation = &stripes;
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
}

void setup()
{
  // FastLED.addLeds<SK9822, DATAPIN, CLOCKPIN, BGR>(leds, NUMPIXELS);   // IT WORKS!
  FastLED.addLeds<WS2811, DATAPIN, GRB>(leds, NUMPIXELS);
  Serial.begin(57600);

  button_debouncer.attach(buttonPin, INPUT_PULLUP);
  button_debouncer.interval(5);
  radio.setup();
}

void loop()
{
  button_debouncer.update();
  knob.check(&animation_index);
  if(offMode) {
    fill_solid(leds, NUMPIXELS, CRGB::Black);
    FastLED.show();
    return;
  }
  radio.check();
  playAnimation();
}
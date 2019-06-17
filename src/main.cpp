#include <Arduino.h>
#define FASTLED_INTERNAL
#include "FastLED.h"
#include <Adafruit_DotStar.h>
#include <SPI.h>
#include <MyKnob.h>
#include <Encoder.h>
#include "config.h"

#include "animations/DiamondNecklace.h"
#include "animations/Crossfade.h"
#include "animations/FuckMyEyes.h"
#include "animations/ColorChooser.h"
#include "animations/FindMyBike.h"
#include "animations/Race.h"

#define DATAPIN 4
#define CLOCKPIN 5

// Pins for the rotary
uint8_t rotary1 = 2;
uint8_t rotary2 = 3;

int buttonPin = A0;
int animation_index = 0;

CRGB leds[NUMPIXELS];
MyKnob knob(rotary1, rotary2);

// Load animations...
FuckMyEyes fuck_my_eyes(knob, leds);
DiamondNecklace diamond_necklace(knob, leds);
Crossfade crossfade(knob, leds);
ColorChooser color_chooser(knob, leds);
FindMyBike find_my_bike(knob, leds);
Race race(knob, leds);

Animation *current_animation = &fuck_my_eyes;

// BUG CAUTION
// never follow one animation function immediately with itself in the the
// next case
int previous_animation_index = 0;
void playAnimation()
{
  if (animation_index != previous_animation_index)
  {
    if (animation_index > 1)
      animation_index = 0;
    switch (animation_index)
    {
    case 0:
      current_animation = &fuck_my_eyes;
      break;
    case 1:
      current_animation = &color_chooser;
      break;
    case 3:
      current_animation = &fuck_my_eyes;
      break;
    case 4:
      current_animation = &race;
      break;
    case 5:
      current_animation = &find_my_bike;
      break;
    }
    current_animation->setup();
    previous_animation_index = animation_index;
  }
  current_animation->run();
}

void setup()
{
  FastLED.addLeds<DOTSTAR, DATAPIN, CLOCKPIN, RGB>(leds, NUMPIXELS);
  Serial.begin(57600);

  pinMode(buttonPin, INPUT_PULLUP);
  button_debouncer.attach(buttonPin);
  button_debouncer.interval(5);

  pinMode(rotary1, INPUT_PULLUP);
  pinMode(rotary2, INPUT_PULLUP);
}

void loop()
{
  button_debouncer.update();
  playAnimation();
  knob.check(&animation_index);
}
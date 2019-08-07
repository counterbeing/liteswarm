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
#include "animations/FindMyBike.h"
#include "animations/FuckMyEyes.h"
#include "animations/Race.h"
#include "animations/Rainbow.h"



// Pins for the rotary
uint8_t rotary1 = 2;
uint8_t rotary2 = 3;

int buttonPin = A0;

CRGB leds[NUMPIXELS];
MyKnob knob(rotary1, rotary2);

// Load animations...
ColorChooser color_chooser(knob, leds);
Crossfade crossfade(knob, leds);
DiamondNecklace diamond_necklace(knob, leds);
FindMyBike find_my_bike(knob, leds);
FuckMyEyes fuck_my_eyes(knob, leds);
Race race(knob, leds);
Rainbow rainbow(knob, leds);

int animation_index = 0;
Radio radio(knob, animation_index);

Animation *current_animation = &rainbow;
int previous_animation_index = -1;
void playAnimation()
{
  if (animation_index != previous_animation_index)
  {
    // Serial.print("Coming from ");
    // Serial.print(previous_animation_index);
    // Serial.print(" to ");
    // Serial.println(animation_index);
    if (animation_index > 6)
      animation_index = 0;
    // BUG CAUTION
    // never follow one animation function immediately with itself in the the
    // next case
    Serial.print("Animation Index: ");
    Serial.println(animation_index);
    switch (animation_index)
    {
    case 0:
      current_animation = &rainbow;
      break;
    case 1:
      current_animation = &color_chooser;
      break;
    case 2:
      current_animation = &race;
      break;
    case 3:
      current_animation = &crossfade;
      break;
    case 4:
      current_animation = &diamond_necklace;
      break;
    case 5:
      current_animation = &fuck_my_eyes;
      break;
    case 6:
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
  Serial.println("SETTING UP -------");
  FastLED.addLeds<WS2811, DATAPIN, BGR>(leds, NUMPIXELS);
  // FastLED.addLeds<DOTSTAR, DATAPIN, CLOCKPIN, RGB>(leds, NUMPIXELS);
  Serial.begin(57600);

  pinMode(buttonPin, INPUT_PULLUP);
  button_debouncer.attach(buttonPin);
  button_debouncer.interval(5);

  pinMode(rotary1, INPUT_PULLUP);
  pinMode(rotary2, INPUT_PULLUP);
  radio.setup();
}

void loop()
{
  radio.check();
  button_debouncer.update();
  playAnimation();
  knob.check(&animation_index);
}
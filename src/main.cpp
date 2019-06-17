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
int animationIndex = 0;

CRGB leds[NUMPIXELS];
MyKnob knob(rotary1, rotary2);

// Load animations...
FuckMyEyes fuck_my_eyes(knob, leds);
DiamondNecklace diamond_necklace(knob, leds);
Crossfade crossfade(knob, leds);
ColorChooser color_chooser(knob, leds);
FindMyBike find_my_bike(knob, leds);
Race race(knob, leds);

// BUG CAUTION
// never follow one animation function immediately with itself in the the
// next case
void playAnimation()
{
  if (animationIndex > 5)
    animationIndex = 0;
  switch (animationIndex)
  {
  case 0:
    crossfade.run();
    break;
  case 1:
    color_chooser.run();
    break;
  case 2:
    diamond_necklace.run();
    break;
  case 3:
    fuck_my_eyes.run();
    break;
  case 4:
    race.run();
    break;
  case 5:
    find_my_bike.run();
    break;
  }
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
  knob.check(&animationIndex);
}
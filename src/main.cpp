#include <Arduino.h>
#define FASTLED_INTERNAL
#include "FastLED.h"
#include <Adafruit_DotStar.h>
#include <SPI.h>
#include <MyKnob.h>
#include <Encoder.h>

#include "DiamondNecklace.h"
#include "Crossfade.h"

#define NUMPIXELS 76
#define DATAPIN 4
#define CLOCKPIN 5

Adafruit_DotStar strip = Adafruit_DotStar(
    NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

int buttonPin = A0;
int head = 0; // Index of first 'on' and 'off' pixels
const uint32_t white = 0xFFFFFF;
const uint32_t red = 0x00FF00;
const uint32_t green = 0xFF0000;
const uint32_t blue = 0x0000FF;
const uint32_t off = 0x000000;
int animationIndex = 0;

// Pins for the rotary
uint8_t rotary1 = 2;
uint8_t rotary2 = 3;

CRGB leds[NUMPIXELS];
MyKnob knob(rotary1, rotary2);
DiamondNecklace diamond_necklace(knob, leds);
Crossfade crossfade(knob, leds);


void complete_color(uint32_t color)
{
  for (int leds = 0; leds < NUMPIXELS; leds++)
  {
    strip.setPixelColor(leds, color);
  }
  strip.show();
}

int remapInRange(int index)
{
  if (index < 0)
    return NUMPIXELS;
  if (index > NUMPIXELS)
    return 0;
  return index;
}

long previousMillis = 0;
bool nonBlockDelay(unsigned int interval)
{
  unsigned long currentMillis = millis();
  if ((currentMillis - previousMillis) > interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

void draw_snake(int head, int green, int red, int blue)
{
  for (int i = 0; i < 5; i++)
  {
    strip.setPixelColor(head - i, green, red, max((blue - 255), 0));
  }
}

void strand_off()
{
  complete_color(off);
}

// void crossfade()
// {
//   int dlay = knob.confine(1, 500);
//   static uint8_t hue = 0;
//   if (nonBlockDelay(dlay))
//   {
//     FastLED.showColor(CHSV(hue++, 255, 255));
//   }
// }

int r_head = 0;
int g_head = 0;
int b_head = 0;

// void shimmer()
// {
//   int dlay = knob.confine(5, 500);
//   if (nonBlockDelay(dlay))
//   {
//     complete_color(off);
//     r_head++;
//     if (r_head > NUMPIXELS)
//       r_head = 0;
//     // green, red, blue
//     draw_snake(r_head, 50, 255, 100);

//     b_head++;
//     if (b_head > NUMPIXELS)
//       b_head = 0;
//     draw_snake((NUMPIXELS - b_head), 30, 20, 255);

//     g_head = g_head + 2;
//     if (g_head > NUMPIXELS)
//       g_head = 0;
//     draw_snake((NUMPIXELS - g_head), 255, 130, 0);

//     strip.show();
//   }
// }

void find_my_bike()
{
  complete_color(off);
  if (nonBlockDelay(3000))
  {
    complete_color(white);
    delay(20);
  }
}

// Associativity specification is redundant for unary operators and is only
// shown for completeness: unary prefix operators always associate right-to-left
// (delete ++*p is delete(++(*p))) and unary postfix operators always associate
// left-to-right (a[1][2]++ is ((a[1])[2])++). Note that the associativity is
// meaningful for member access operators, even though they are grouped with
// unary postfix operators: a.b++ is parsed (a.b)++ and not a.(b++).
// void race()
// {
//   int dlay = knob.confine(5, 500);
//   if (nonBlockDelay(dlay))
//   {
//     static uint8_t hue = 0;
//     head = remapInRange(++head);
//     leds[head] = CRGB::Black;
//     leds[head] = CHSV(hue++, 255, 150);
//     head = remapInRange(++head);
//     leds[head] = CHSV(hue++, 255, 255);
//     FastLED.show();
//   }
// }

// void color_chooser(){
//   int color = knob.confine(0, 255, true);
//   fill_solid(leds, NUMPIXELS, CHSV(color, 255, 255));
//   FastLED.show();
// }

// uint32_t lastColor = red;
// uint32_t currentColor = red;
// void dazzle()
// {
//   int dlay = knob.confine(0, 500);
//   if (nonBlockDelay(dlay))
//   {
//     switch (lastColor)
//     {
//     case red:
//       complete_color(green);
//       lastColor = green;
//       break;
//     case green:
//       complete_color(blue);
//       lastColor = blue;
//       break;
//     case blue:
//       complete_color(red);
//       lastColor = red;
//       break;
//     }
//   }
// }

// uint32_t lastStrobeColor = white;
// void strobe()
// {
//   int dlay = knob.confine(0, 1000);
//   if (nonBlockDelay(dlay))
//   {
//     switch (lastStrobeColor)
//     {
//     case off:
//       complete_color(white);
//       lastStrobeColor = white;
//       break;
//     case white:
//       complete_color(off);
//       lastStrobeColor = off;
//       break;
//     }
//   }
// }


// BUG CAUTION
// never follow one animation function immediately with itself in the the
// next case
// DEBUG NOTE
// Using strobe is an easy way to see how many pixels you have since this value
// must be set manually.
void playAnimation()
{
  // strobe();
  if (animationIndex > 5)
    animationIndex = 0;
    // diamond_necklace.run();
    crossfade.run();
  // color_chooser();
  // switch (animationIndex)
  // {
  // case 0:
  //   crossfade();
  //   break;
  // case 1:
  //   race();
  //   break;
  // case 2:
  //   dazzle();
  //   break;
  // case 3:
  //   shimmer();
  //   break;
  // case 4:
  //   strobe();
  //   break;
  // case 5:
  //   find_my_bike();
  //   break;
  // }
}

void setup()
{
  FastLED.addLeds<DOTSTAR, DATAPIN, CLOCKPIN, RGB>(leds, NUMPIXELS);
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
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
// Simple strand test for Adafruit Dot Star RGB LED strip.
// This is a basic diagnostic tool, NOT a graphics demo...helps confirm
// correct wiring and tests each pixel's ability to display red, green
// and blue and to forward data down the line.  By limiting the number
// and color of LEDs, it's reasonably safe to power a couple meters off
// the Arduino's 5V pin.  DON'T try that with other code!

#include <Adafruit_DotStar.h>
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
#include <Bounce2.h>

#define NUMPIXELS 60 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    4
#define CLOCKPIN   5

Bounce debouncer = Bounce();

Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
// The last parameter is optional -- this is the color data order of the
// DotStar strip, which has changed over time in different production runs.
// Your code just uses R,G,B colors, the library then reassigns as needed.
// Default is DOTSTAR_BRG, so change this if you have an earlier strip.

// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
//Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BRG);
int buttonPin =  A0;
int      head  = 0, tail = 59; // Index of first 'on' and 'off' pixels
uint32_t red = 0x00FF00;
uint32_t green = 0xFF0000;
uint32_t blue = 0x0000FF;
uint32_t off = 0x000000;
int animationIndex = 0;


void setup() {
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
  Serial.begin(9600);
  
  pinMode(buttonPin, INPUT_PULLUP);
  debouncer.attach(buttonPin);
  debouncer.interval(5);
}

void loop() {
  debouncer.update();
  playAnimation();
  checkButton();
}

void playAnimation(){
  if (animationIndex > 3) animationIndex = 0;
  switch (animationIndex) {
    case 0:
      strand_off();
      break;
    case 1:
      strobe();
      break;
    case 2:
      dazzle();
      break;
    case 3:
      race();
      break;
  }
}


int lastPressTime = 0;
int checkButton(){
  int buttonState = debouncer.read();
  Serial.println(buttonState);
  int currentPressTime = millis();
  int difference = currentPressTime - lastPressTime;
  if((buttonState == 0) && (difference > 1000)){
    lastPressTime = millis();
    /* Serial.println("CHANGECHANGECHNGGCHHHCH"); */
    animationIndex++;
  }
}

void strand_off(){
  complete_color(off);
}

void race(){
 complete_color(off); 
 head++;
 if(head > NUMPIXELS) head = 0;
 strip.setPixelColor(head, blue);
 strip.show();
 delay(10);
}

void dazzle(){
  complete_color(red);
  delay(10);
  complete_color(green);
  delay(10);
  complete_color(blue);
  delay(10);
}

void strobe(){
  complete_color(0xFFFFFF);
  delay(50);
  complete_color(off);
  delay(50);
}

void complete_color(uint32_t color){
  for (int leds = 0 ; leds < NUMPIXELS; leds++){
    strip.setPixelColor(leds, color);
  }
  strip.show();
}

#include "FastLED.h"
#include <Adafruit_DotStar.h>
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
#include <Bounce2.h>
#include <Encoder.h>

//#define NUMPIXELS 150
#define NUMPIXELS 256
#define DATAPIN    4
#define CLOCKPIN   5

Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

int buttonPin =  A0;
int rotary1 = 2;
int rotary2 = 3;
long posRotary = -999;
int      head  = 0; // Index of first 'on' and 'off' pixels
const uint32_t white = 0xFFFFFF;
const uint32_t red = 0x00FF00;
const uint32_t green = 0xFF0000;
const uint32_t blue = 0x0000FF;
const uint32_t off = 0x000000;
int animationIndex = 0;

int buttonPressCount = 0;

Bounce button_debouncer = Bounce();
Encoder knob(rotary1, rotary2);
CRGB leds[NUMPIXELS];

void setup() {
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

void loop() {
  button_debouncer.update();
  playAnimation();
  checkRotary(millis());
  checkButton();
}

void playAnimation(){
  if (animationIndex > 6) animationIndex = 0;
  switch (animationIndex) {
    case 0:
      shimmer();
      break;
//    case 1:
//      strobe();
//      break;
    case 1:
      dazzle();
      break;
    case 2:
      find_my_bike();
      break;
		case 3:
			crossfade();
			break;
    case 4:
      race();
      break;
		case 5:
			color_slide();
			break;
  }
}

void checkRotary(int loopTime){
  long newPos = knob.read();
  if(newPos != posRotary){
   posRotary = newPos;
   /* Serial.println(newPos); */
  }
}

long lastPressTime = 0;

int checkButton(){
  int buttonState = button_debouncer.read();
  long currentPressTime = millis();
  long difference = currentPressTime - lastPressTime;
  if((buttonState == 0) && (difference > 400)){
    buttonPressCount++;
    /* Serial.print("Button press count: "); */
    /* Serial.println(buttonPressCount); */
		/* Serial.println("BUTTON PRESSED CALLING SWITCH"); */
    /* Serial.println(lastPressTime); */
    /* Serial.println(currentPressTime); */
    lastPressTime = millis();
    animationIndex++;
  }
}

void strand_off(){
  complete_color(off);
}

void crossfade(){
	int dlay = confineRotary(1, 500);
  static uint8_t hue = 0;
  if(nonBlockDelay(dlay)) {
    FastLED.showColor(CHSV(hue++, 255, 255));
  }
}

int r_head = 0;
int g_head = 0;
int b_head = 0;

void shimmer(){
  int dlay = confineRotary(5, 500);
  if(nonBlockDelay(dlay)) {
    complete_color(off);
    r_head++;
    if(r_head > NUMPIXELS) r_head = 0;
    // green, red, blue
    draw_snake(r_head, 30, 255, 100);

    b_head++;
    if(b_head > NUMPIXELS) b_head = 0;
    draw_snake((NUMPIXELS - b_head), 30, 0, 255);

    g_head = g_head + 2;
    if(g_head > NUMPIXELS) g_head = 0;
    draw_snake((NUMPIXELS - g_head), 255, 130, 0);

    strip.show();
  }
}

void draw_snake(int head, int green, int red, int blue) {
  int head_minus_5  = remapInRange(head - 5);
  int head_minus_4  = remapInRange(head - 4);
  int head_minus_3  = remapInRange(head - 3);
  int head_minus_2  = remapInRange(head - 2);
  int head_minus_1  = remapInRange(head - 1);
  uint32_t myColor = strip.getPixelColor(head_minus_5);
  Serial.println(String(myColor));
  strip.setPixelColor(head_minus_5, green, red, max((blue - 255), 0));
  strip.setPixelColor(head_minus_4, green, red, max((blue - 100), 0));
  strip.setPixelColor(head_minus_3, green, red, max((blue - 50), 0));
  strip.setPixelColor(head_minus_2, green, red, max((blue - 20), 0));
  strip.setPixelColor(head_minus_1, green, red, blue);
}



void find_my_bike(){
  complete_color(off);
  if(nonBlockDelay(3000)) {
    complete_color(white);
    delay(20);
  }
}

void color_slide(){
	int dlay = confineRotary(5, 500);
  if(nonBlockDelay(dlay)) {
    static uint8_t hue = 0;
    head++;
    head = remapInRange(head);
    leds[head] = CHSV(hue++, 255, 255);
    FastLED.show();
  }
}

void race(){
 	int dlay = confineRotary(5, 500);
  if(nonBlockDelay(dlay)) {
    static uint8_t hue = 0;
    leds[head - 1] = CRGB::Black;
    leds[head] = CHSV(hue++, 255, 150);
    head++;
    leds[head] = CHSV(hue++, 255, 255);
    if(head > NUMPIXELS) head = 0;
    head = remapInRange(head);
    leds[head] = CHSV(hue++, 255, 255);
    FastLED.show();
  }
}

uint32_t lastColor = red;
uint32_t currentColor = red;
void dazzle(){
  int dlay = confineRotary(0, 500);
  if(nonBlockDelay(dlay)) {
    switch (lastColor) {
      case red:
        complete_color(green);
        lastColor = green;
        break;
      case green:
        complete_color(blue);
        lastColor = blue;
        break;
      case blue:
        complete_color(red);
        lastColor = red;
        break;
    }
  }
}

uint32_t lastStrobeColor = white;
void strobe(){
  int dlay = confineRotary(0, 1000);
  if(nonBlockDelay(dlay)) {
    switch (lastStrobeColor) {
      case off:
        complete_color(white);
        lastStrobeColor = white;
        break;
      case white:
        complete_color(off);
        lastStrobeColor = off;
        break;
    }
  }
}

void complete_color(uint32_t color){
  for (int leds = 0 ; leds < NUMPIXELS; leds++){
    strip.setPixelColor(leds, color);
  }
  strip.show();
}

int confineRotary(int start, int finish) {
  if(posRotary < start) {
    posRotary = start;
    knob.write(start);
  }
  if(posRotary > finish) {
    posRotary = finish;
    knob.write(finish);
  }
  return posRotary;
}

int remapInRange(int index){
  if (index < 0) index = (NUMPIXELS - 1) + index;
  if (index > NUMPIXELS) index = 0;
  return index;
}

long previousMillis = 0;
bool nonBlockDelay(int interval){
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
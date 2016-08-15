#include "FastLED.h"
#include <Adafruit_DotStar.h>
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
#include <Bounce2.h>
#include <Encoder.h>

#define NUMPIXELS 60
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
    case 1:
      strobe();
      break;
    case 2:
      dazzle();
      break;
    case 3:
      find_my_bike();
      break;
		case 4:
			color_slide();
			break;
    case 5:
      race();
      break;
		case 6:
			color_slide();
			break;
  }
}

void checkRotary(int loopTime){
  long newPos = knob.read();
  if(newPos != posRotary){
   posRotary = newPos;
   Serial.println(newPos);
  }
}

long lastPressTime = 0;

int checkButton(){
  int buttonState = button_debouncer.read();
  long currentPressTime = millis();
  long difference = currentPressTime - lastPressTime;
  if((buttonState == 0) && (difference > 400)){
    buttonPressCount++;
    Serial.print("Button press count: ");
    Serial.println(buttonPressCount);
		Serial.println("BUTTON PRESSED CALLING SWITCH");
    Serial.println(lastPressTime);
    Serial.println(currentPressTime);
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

void shimmer(){
  int dlay = confineRotary(5, 500);
  if(nonBlockDelay(dlay)) {
    complete_color(off); 
    head++;
    if(head > NUMPIXELS) head = 0;
    int leader = remapInRange(head - 1);
    int follower = remapInRange(head + 1);
    strip.setPixelColor(leader, red);
    strip.setPixelColor(head, blue);
    strip.setPixelColor(follower, white);
    strip.show();
  }
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

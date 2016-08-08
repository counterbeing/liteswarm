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
int      head  = 0, tail = 59; // Index of first 'on' and 'off' pixels
uint32_t white = 0xFFFFFF;
uint32_t red = 0x00FF00;
uint32_t green = 0xFF0000;
uint32_t blue = 0x0000FF;
uint32_t off = 0x000000;
int animationIndex = 0;

Bounce button_debouncer = Bounce();
Encoder knob(rotary1, rotary2);
CRGB leds[NUMPIXELS];

void setup() {
	FastLED.addLeds<DOTSTAR, DATAPIN, CLOCKPIN, RGB>(leds, NUMPIXELS);
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
  Serial.begin(9600);
  
  pinMode(buttonPin, INPUT_PULLUP);
  button_debouncer.attach(buttonPin);
  button_debouncer.interval(5);

  pinMode(rotary1, INPUT_PULLUP);
  pinMode(rotary2, INPUT_PULLUP);
}

void loop() {
  button_debouncer.update();
  playAnimation();
  checkButton();
  checkRotary(millis());
}

void playAnimation(){
  if (animationIndex > 5) animationIndex = 0;
  switch (animationIndex) {
    case 0:
      race();
      break;
    case 1:
      strobe();
      break;
    case 2:
      dazzle();
      break;
    case 3:
      crossfade();
      break;
    case 4:
      shimmer();
      break;
		case 5:
			color_slide();
			break;
  }
}

unsigned char encoder_A;
unsigned char encoder_B;
unsigned char encoder_A_prev=0;
void checkRotary(int loopTime){
  long newPos;
  newPos = knob.read();
  if(newPos != posRotary){
   posRotary = newPos;
   Serial.println(newPos);
  }
}


int lastPressTime = 0;
int checkButton(){
  int buttonState = button_debouncer.read();
  int currentPressTime = millis();
  int difference = currentPressTime - lastPressTime;
  if((buttonState == 0) && (difference > 1000)){
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
  FastLED.showColor(CHSV(hue++, 255, 255)); 
  delay(dlay);
}

void shimmer(){
 int dlay = confineRotary(5, 500);
 complete_color(off); 
 head++;
 if(head > NUMPIXELS) head = 0;
 int leader = remapInRange(head - 1);
 int follower = remapInRange(head + 1);
 strip.setPixelColor(leader, red);
 strip.setPixelColor(head, blue);
 strip.setPixelColor(follower, white);
 strip.show();
 delay(dlay);
}

/* void race(){ */
/*  int dlay = confineRotary(5, 500); */
/*  complete_color(off);  */
/*  head++; */
/*  if(head > NUMPIXELS) head = 0; */
/*  strip.setPixelColor(head, blue); */
/*  strip.show(); */
/*  delay(dlay); */
/* } */

void color_slide(){
	int dlay = confineRotary(5, 500);
	complete_color(off); 
	head++;
	if(head > NUMPIXELS) head = 0;
	static uint8_t hue = 0;
	leds[head] = CHSV(hue++, 255, 255);
	FastLED.show();
	delay(dlay);
}


void race(){
	int dlay = confineRotary(5, 500);
	static uint8_t hue = 0;
	leds[head - 1] = CRGB::Black;
	leds[head] = CHSV(hue++, 255, 150);
	head++;
	leds[head] = CHSV(hue++, 255, 255);
	if(head > NUMPIXELS) head = 0;
	leds[head] = CHSV(hue++, 255, 255);
	FastLED.show();
	delay(dlay);
}


void dazzle(){
  int dlay = confineRotary(5, 500);
  complete_color(red);
  delay(dlay);
  complete_color(green);
  delay(dlay);
  complete_color(blue);
  delay(dlay);
}

void strobe(){
   int dlay = confineRotary(5, 500);
  complete_color(0xFFFFFF);
  delay(dlay);
  complete_color(off);
  delay(dlay);
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
  /* Serial.println(index); */
  return index;
}

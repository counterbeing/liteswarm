#include <Adafruit_DotStar.h>
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
#include <Bounce2.h>
#include <Encoder.h>

#define NUMPIXELS 60 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    4
#define CLOCKPIN   5

Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

int buttonPin =  A0;
int rotary1 = 2;
int rotary2 = 3;
long posRotary = -999;
int      head  = 0, tail = 59; // Index of first 'on' and 'off' pixels
uint32_t red = 0x00FF00;
uint32_t green = 0xFF0000;
uint32_t blue = 0x0000FF;
uint32_t off = 0x000000;
int animationIndex = 0;

Bounce button_debouncer = Bounce();
Encoder knob(rotary1, rotary2);

void setup() {
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
  /* Serial.println(buttonState); */
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

void race(){
 int dlay = confineRotary(5, 500);
 complete_color(off); 
 head++;
 if(head > NUMPIXELS) head = 0;
 strip.setPixelColor(head, blue);
 strip.show();
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

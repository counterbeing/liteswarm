#ifndef CONFIG_H
#define CONFIG_H

#define KNOB_DEBUG false  // set flag to false to disable serial logging in knob
#define RADIO_DEBUG false // set flag to false to disable logging for Radio
#define ANIM_DEBUG false

// One of WS2811 SK9822 DOTSTAR WS2815
#define STRIP_TYPE WS2811

const int NUMPIXELS = 75;

#if BOARD == nano
const int CLOCKPIN = 4;
const int DATAPIN = 5;
const int buttonPin = A0;
uint8_t rotary1 = 2;
uint8_t rotary2 = 3;
// PIN_RADIO_CE = 6; // will be 6 on PCBs, was 9
// PIN_RADIO_CSN = 7; // will be 7 on PCBs, was 10
#elif BOARD == esp32
const int CLOCKPIN = 4;
const int DATAPIN = 5;
const int buttonPin = A0;
uint8_t rotary1 = 2;
uint8_t rotary2 = 3;
#else
#  error "Unexpected value of BOARD."
#endif

#endif
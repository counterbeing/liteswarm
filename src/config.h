#pragma once
#include <cstdint>

#define KNOB_DEBUG false  // set flag to false to disable serial logging in knob
#define RADIO_DEBUG false // set flag to false to disable logging for Radio
#define ANIM_DEBUG false

// One of WS2811 SK9822 DOTSTAR WS2815
#define STRIP_TYPE WS2811

const int NUMPIXELS = 50;

// Here we define boards as integers to be used in `platform.ini` passed build
// flags. This way we can use the easier to reference varaiable name to keep the
// code readable when conditionally compiling blocks of code.
#define BOARD_NANO 1
#define BOARD_ESP32 2

#if BOARD == BOARD_NANO
const int CLOCKPIN = 4;
const int DATAPIN = 5;
const int buttonPin = A0;
uint8_t rotary1 = 2;
uint8_t rotary2 = 3;
// PIN_RADIO_CE = 6; // will be 6 on PCBs, was 9
// PIN_RADIO_CSN = 7; // will be 7 on PCBs, was 10
#elif BOARD == BOARD_ESP32
const int CLOCKPIN = 4;
const int DATAPIN = 5;
const int buttonPin = 15;
uint8_t rotary1 = 2;
uint8_t rotary2 = 3;
#else
#  error "Unexpected value of BOARD."
#endif

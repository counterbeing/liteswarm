#pragma once
#include <cstdint>

/* #TODO OUT OF DATE 2023-08-03

Radio    ESP32 module
CE    -> 4
CSN   -> 5
MOSI  -> 23
MISO  -> 19
SCK   -> 18
IRQ   -> No connection
VCC   -> No more than 3.6 volts
GND   -> GND
*/

// #define NUMPIXELS 60
#define NUMPIXELS 75
#define FRAMES_PER_SECOND 60
#define LED_spiClk 18
#define LED_spiMosi 23

// #define PIN_RADIO_CE 4
// #define PIN_RADIO_CSN 5
// #define PIN_RADIO_MOSI 23
// #define PIN_RADIO_MISO 19
// #define PIN_RADIO_SCK 18

// maybe 16,17,xx? feather
#define buttonPin 39
#define rotary1 34
#define rotary2 36

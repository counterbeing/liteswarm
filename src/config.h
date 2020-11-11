#ifndef CONFIG_H
#define CONFIG_H

#define KNOB_DEBUG false  // set flag to false to disable serial logging in knob
#define RADIO_DEBUG false // set flag to false to disable logging for Radio
#define ANIM_DEBUG false

// #define SCARF_DOTSTAR
// #define SCARF_WS2811     // IC has pink dot
// #define SCARF_SK9822 // IC has green dot
// #define BIGRED_WS2815    // IC has purple dot?

#if BOARD == nano
#  define DATAPIN 5  // default for SCARFYMCSARFACE pcb
#  define CLOCKPIN 4 // Not applicable to 4 wire APA102 type strands
const int NUMPIXELS = 75;
const int buttonPin = A0;
uint8_t rotary1 = 2;
uint8_t rotary2 = 3;
#elif BOARD == esp32
#  define DATAPIN 5 // default for SCARFYMCSARFACE pcb
#  define CLOCKPIN 4 // Not applicable to 4 wire APA102 type strands
const int NUMPIXELS = 75;
// buttonPin = A0
// rotary1 = 2
// rotary2 = 3
#else
#  error "Unexpected value of BOARD."
#endif

// PIN_RADIO_CE = 6; // will be 6 on PCBs, was 9
// PIN_RADIO_CSN = 7; // will be 7 on PCBs, was 10

//////////////////////////////////////////////////
// WS2811
// #ifdef SCARF_WS2811
// const int NUMPIXELS = 75;
// #endif

//////////////////////////////////////////////////
// SK9822
// #ifdef SCARF_SK9822
// const int NUMPIXELS = 75;
// #endif

//////////////////////////////////////////////////
// DOTSTAR
// #ifdef SCARF_DOTSTAR
// const int NUMPIXELS = 75;
// #endif

//////////////////////////////////////////////////
// BIGRED 12v WS2815 strip
//   provide 12v via strip's power barrel-connector plugs
//   connect strip ground to controller ground (strip blue wire to controller
//   black wire) connect strip data to controller datapin (strip green wire to
//   controller green wire)
//
// #define CLOCKPIN 5
// #ifdef BIGRED_WS2815
// const int NUMPIXELS = 375;
// #endif

#endif
#ifndef CONFIG_H
#define CONFIG_H

#define KNOBDEBUG true     // flag to enable/disable serial logging in knob
#define RADIODEBUG false     // flag to enable/disable serial logging for Radio

////////////////////////////////////////////////////
// DEFAULTS
// All that matters to a 3pin WS* strand
#define DATAPIN 5 // default for SCARFYMCSARFACE pcb
#define CLOCKPIN 4 // Not applicable to 4 wire APA102 type strands
///////////////////////////////////////////////////



//////////////////////////////////////////////////
// mac's SK9822
//   oddly flipping the order of the default CLOCKPIN & DATAPIN in  constructor fixed strip
//   FastLED.addLeds<SK9822, CLOCKPIN, DATAPIN, BGR>(leds, NUMPIXELS);
// #define DATAPIN 5 // mac's SK9822
// #define CLOCKPIN 4 // mac's SK9822
//
// mac's protoboard
// #define DATAPIN 5 // mac's protoboard
// #define CLOCKPIN 7 // mac protoboard
////////////////////////////////////////


#define STRIP WS2811 // TODO figure this out - looks unused
const int NUMPIXELS = 75;

// knob defaults
// buttonPin = A0
// rotary1 = 2
// rotary2 = 3
// PIN_RADIO_CE = 6; // will be 6 on PCBs, was 9
// PIN_RADIO_CSN = 7; // will be 7 on PCBs, was 10

#endif
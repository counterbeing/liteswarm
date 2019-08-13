#ifndef CONFIG_H
#define CONFIG_H

// All that matters to a 3pin WS* strand
// #define DATAPIN 4 // default for SCARFYMCSARFACE pcb
#define DATAPIN 5 // mac's protoboard
// Not applicable to 4 wire APA102 type strands
// #define CLOCKPIN 5
#define CLOCKPIN 7 // mac protoboard

#define STRIP WS2811
const int NUMPIXELS = 75;

// knob defaults
// buttonPin = A0
// rotary1 = 2
// rotary2 = 3
// PIN_RADIO_CE = 6; // will be 6 on PCBs, was 9
// PIN_RADIO_CSN = 7; // will be 7 on PCBs, was 10

#endif
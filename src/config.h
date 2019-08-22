#ifndef CONFIG_H
#define CONFIG_H

#define KNOBDEBUG false      // set flag to false to disable serial logging in knob
#define RADIODEBUG false     // set flag to false to disable logging for Radio

#define WS2811
// #define SK9822
// #define WS2815

////////////////////////////////////////////////////
// DEFAULTS
// All that matters to a 3pin WS* strand
#define DATAPIN 4    // default for SCARFYMCSARFACE pcb
#define CLOCKPIN 5   // Not applicable to 4 wire APA102 type strands
#define STRIP WS2811 // TODO figure this out - looks unused
// const int NUMPIXELS = 75;
//
// knob defaults
// buttonPin = A0
// rotary1 = 2
// rotary2 = 3
// PIN_RADIO_CE = 6; // will be 6 on PCBs, was 9
// PIN_RADIO_CSN = 7; // will be 7 on PCBs, was 10
///////////////////////////////////////////////////


//////////////////////////////////////////////////
// SK9822
//   oddly flipping the order of the default CLOCKPIN & DATAPIN in  constructor fixed strip
//   FastLED.addLeds<SK9822, CLOCKPIN, DATAPIN, BGR>(leds, NUMPIXELS);
// #define DATAPIN 5 // mac's SK9822
// #define CLOCKPIN 4 // mac's SK9822
//
// mac's protoboard
// #define DATAPIN 5 // mac's protoboard
// #define CLOCKPIN 7 // mac protoboard
//////////////////////////////////////////////////


//////////////////////////////////////////////////
// 12v WS2815 strip 
//   provide 12v via strip's power barrel-connector plugs
//   connect strip ground to controller ground (strip blue wire to controller black wire)
//   connect strip data to controller datapin (strip green wire to controller green wire)
// 
// #define CLOCKPIN 5
const int NUMPIXELS = 375;
//////////////////////////////////////////////////

#endif


//////////////////////////////////////////////////
// 12v WS2815 strip troubleshooting
//
// works, but only first 75 pixels
//    const int NUMPIXELS = 75;
//    FastLED.addLeds<WS2811, CLOCKPIN, GRB>(leds, 150);  // 5m
// 
// doesn't work; changing NUMPIXELS seems to crash the main loop somehow
//    const int NUMPIXELS = 150;
//    FastLED.addLeds<WS2811, CLOCKPIN, GRB>(leds, 150);  // 5m

// which animations work w/ NUMPIXELS = 150:
// <ok?> - <animation>
// y - crossfade
// y - color_chooser
// y - race
// y - stars
// y - rainbow
// y - fuck_my_eyes
// y - stripes
// y - diamond_necklace
// y - dimmer

// which animations work w/ NUMPIXELS = 375 ?
// serial strings + only crossfade
// UH OH....
// DATA:    [==========]  132.2% (used 2707 bytes from 2048 bytes)
//
// commenting out all serial.print with strings saves 406 bytes
// DATA:    [========  ]  80.2% (used 1642 bytes from 2048 bytes)
//
// add colorchooser
// DATA:    [========  ]  81.9% (used 1677 bytes from 2048 bytes)
// 
// add race
// DATA:    [========  ]  83.6% (used 1713 bytes from 2048 bytes)
//
// add stars
// DATA:    [========= ]  85.4% (used 1750 bytes from 2048 bytes)
//
// add rainbow
// DATA:    [========= ]  87.1% (used 1783 bytes from 2048 bytes)
// 
// add fuck_my_eyes
// DATA:    [========= ]  88.8% (used 1818 bytes from 2048 bytes)
//
// add stripes
// DATA:    [========= ]  90.6% (used 1855 bytes from 2048 bytes)
//
// add diamond_necklace
// DATA:    [========= ]  92.3% (used 1890 bytes from 2048 bytes)
//
// add dimmer
// DATA:    [========= ]  94.1% (used 1927 bytes from 2048 bytes)

// WS2815 power draw w/ NUMPIXELS = 375:
//
// <animation>   -     <amps>
// crossfade:          3.0 - 4.2
// color_chooser:      3.0 - 4.2
// race:               2.6 
// stars:              1.0
// rainbow:            3.4
// fuck_my_eyes:       3.8
// stripes:            3.5
// diamond_necklace:   1.0
// dimmer:             0.78 - 4.25
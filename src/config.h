#ifndef CONFIG_H
#define CONFIG_H

#define KNOB_DEBUG false   // set flag to false to disable serial logging in knob
#define RADIO_DEBUG false  // set flag to false to disable logging for Radio
#define ANIM_DEBUG false 

// #define SCARF_DOTSTAR
// #define SCARF_WS2811     // IC has pink dot
#define MATRIX_WS2811     // IC has pink dot
// #define VEST_WS2812B     // IC has pink dot
// #define SCARF_SK9822  // IC has green dot
// #define BIGRED_WS2815    // IC has purple dot?


////////////////////////////////////////////////////
// DEFAULTS
//   switch LED strand type & pintouts here for nano platform
#ifndef PIOENV_TEENSY
  #define DATAPIN 5   // default for SCARFYMCSARFACE pcb
  #define CLOCKPIN 4  // Not applicable to 4 wire APA102 type strands
  // #define CONF_BUTTON_PIN A0
  const int buttonPin = A0;
  #define SCARF_WS2811
  // const int NUMPIXELS = 75;
  
  // knob defaults
  // buttonPin = A0
  // rotary1 = 2
  // rotary2 = 3
  
  // RADIO
  const int PIN_RADIO_CE = 7;   // 7 on PCBs 1.3, was 6 on 1.1
  const int PIN_RADIO_CSN = 6;  // 6 on PCBs 1.3, was 7 on 1.1

////////////////////////////////////////////////////
// TEENSY MODE!
//  set pinouts and LED strand type for teensy
#else
  // LEDs ------------------------
  #define DATAPIN 17 // provides Vin voltage 8mA max to LED logic
  // const int NUMPIXELS = 20;
  const int NUMPIXELS = 75;
  // #define SCARF_WS2811 // hardcoded in main.cpp for teensy

  // RADIO ------------------------
  const int PIN_RADIO_CE = 9;
  const int PIN_RADIO_CSN = 10;

  // KNOB ------------------------
  // rotary1 = 0
  // rotary2 = 1
  const int buttonPin = A9;
  
#endif
//========================================================


//////////////////////////////////////////////////
// WS2811
#ifdef SCARF_WS2811
const int NUMPIXELS = 75;
#endif


//////////////////////////////////////////////////
// SK9822
#ifdef SCARF_SK9822
const int NUMPIXELS = 75;
#endif


//////////////////////////////////////////////////
// DOTSTAR
#ifdef SCARF_DOTSTAR
const int NUMPIXELS = 75;
#endif


//////////////////////////////////////////////////
// MATRIX WS2811
// Helper functions for a two-dimensional XY matrix of pixels.
#ifdef MATRIX_WS2811
const uint8_t kMatrixWidth = 75;
const uint8_t kMatrixHeight = 1;

#define NUMPIXELS (kMatrixWidth * kMatrixHeight)

// This function will return the right 'led index number' for 
// a given set of X and Y coordinates on DiscoBandCamp
// This code, plus the supporting 80-byte table is much smaller 
// and much faster than trying to calculate the pixel ID with code.
#define LAST_VISIBLE_LED 74
uint8_t XY( uint8_t x, uint8_t y)
{
  // any out of bounds address maps to the first hidden pixel
  if( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) {
    return (LAST_VISIBLE_LED + 1);
  }
  
  const uint8_t MatrixTable[] = {
      0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
     10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
     20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
     30,  31,  32,  33,  34,  35,  36,  37,  38,  39,
     40,  41,  42,  43,  44,  45,  46,  47,  48,  49,
     50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
     60,  61,  62,  63,  64,  65,  66,  67,  68,  69,
     70,  71,  72,  73,  74
    };

  uint8_t i = (y * kMatrixWidth) + x;
  uint8_t j = MatrixTable[i];
  return j;
}

#endif

//////////////////////////////////////////////////
// Vest WS2812B
// Helper functions for a two-dimensional XY matrix of pixels.
#ifdef VEST_WS2812B
const uint8_t kMatrixWidth = 24;
const uint8_t kMatrixHeight = 8;

#define NUMPIXELS (kMatrixWidth * kMatrixHeight)
#define NUM_LEFT 60
#define NUM_RIGHT 60

// This function will return the right 'led index number' for 
// a given set of X and Y coordinates on DiscoBandCamp
// This code, plus the supporting 80-byte table is much smaller 
// and much faster than trying to calculate the pixel ID with code.
#define LAST_VISIBLE_LED 119
uint8_t XY( uint8_t x, uint8_t y)
{
  // any out of bounds address maps to the first hidden pixel
  if( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) {
    return (LAST_VISIBLE_LED + 1);
  }
  
  const uint8_t MatrixTable[] = {
    10, 9,  8,  7,  6,  5,  4,  3,  2,  1,  0,  145,
    153,60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 
    120,11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 146,  
    154,80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 182, 
    121,127,21, 22, 23, 24, 25, 26, 27, 28, 29, 147, 
    155,89, 88, 87, 86, 85, 84, 83, 82, 81, 176,183, 
    122,128,133,30, 31, 32, 33, 34, 35, 36, 37, 148,  
    156,97, 96, 95, 94, 93, 92, 91, 90, 171,177,184, 
    123,129,134,135,38, 39, 40, 41, 42, 43, 44, 149,  
    157,104,103,102,101,100,99, 98, 167,172,178,185, 
    124,130,134,136,139,45, 46, 47, 48, 49, 50, 150,  
    158,110,109,108,107,106,105,164,168,173,179,186, 
    125,131,134,137,140,142,51, 52, 53, 54, 55, 151,  
    159,115,114,113,112,111,162,165,169,174,180,187, 
    126,132,134,138,141,143,144,56, 57, 58, 59, 152,  
    160,119,118,117,116,161,163,166,170,175,181,188,
  };

  uint8_t i = (y * kMatrixWidth) + x;
  uint8_t j = MatrixTable[i];
  return j;
}

#endif


//////////////////////////////////////////////////
// BIGRED 12v WS2815 strip
//   provide 12v via strip's power barrel-connector plugs
//   connect strip ground to controller ground (strip blue wire to controller
//   black wire) connect strip data to controller datapin (strip green wire to
//   controller green wire)
//
// #define CLOCKPIN 5
#ifdef BIGRED_WS2815
const int NUMPIXELS = 375;
#endif


//////////////////////////////////////////////////
// mac's protoboard
// #define DATAPIN 5 // mac's protoboard
// #define CLOCKPIN 7 // mac protoboard
//////////////////////////////////////////////////



#endif
#include "config.h"
#include "SPI.h"
#include <Arduino.h>
#include <NRFLite.h>
// #include <ESP32Encoder.h>
// #include <Bounce2.h>
#include "MyKnob.h"

MyKnob knob;

// this version is port of the early ledswarm 2019 code
// https://github.com/counterbeing/liteswarm/blob/dc4e0e53ea9b77c02f954a00a81d340e21b89678/src/main.cpp

///////////////////////////////////////////////////////////////////////////////////////////
// init FastLED
/* FastLED ESP32 Hardware SPI Driver
 * https://github.com/FastLED/FastLED/blob/master/src/platforms/esp/32/fastspi_esp32.h
 *
 * This hardware SPI implementation can drive clocked LEDs from either the
 * VSPI or HSPI bus (aka SPI2 & SPI3). No support is provided for SPI1, because it is 
 * shared among devices and the cache for data (code) in the Flash as well as the PSRAM.
 *
 * To enable the hardware SPI driver, add the following line *before* including
 * FastLED.h:
 *
 * #define FASTLED_ALL_PINS_HARDWARE_SPI
 *
 * This driver uses the VSPI bus by default (GPIO 18, 19, 23, & 5). To use the 
 * HSPI bus (GPIO 14, 12, 13, & 15) add the following line *before* including
 * FastLED.h:
 * 
 * #define FASTLED_ESP32_SPI_BUS HSPI
 * 
 */

// may not be necessary, but FastLED.h kicks off an include to led_sysdefs.h, which checks for ESP32,
// then includes https://github.com/FastLED/FastLED/blob/master/src/led_sysdefs.h#L41
#ifndef ESP32
#define ESP32
#endif

#define FASTLED_ALL_PINS_HARDWARE_SPI // <-- must be defined BEFORE FastLED.h included
#define FASTLED_ESP32_SPI_BUS VSPI
#include <FastLED.h>
CRGB leds[NUMPIXELS];


//////////////////////////////////////////////////////////////////////////////////////////
// init radio
/* https://github.com/dparson55/NRFLite/blob/4e425d742ca8879d654a270c7c02c13440476e7a/examples/Basic_RX_ESP32/Basic_RX_ESP32.ino
 * 
 * Demonstrates simple RX operation with an ESP32.
 * Any of the Basic_TX examples can be used as a transmitter.
 * 
 * ESP's require the use of '__attribute__((packed))' on the RadioPacket data structure
 * to ensure the bytes within the structure are aligned properly in memory.
 * 
 * The ESP32 SPI library supports configurable SPI pins and NRFLite's mechanism to support this is shown.
*/

struct __attribute__((packed)) RadioPacket  // Any packet up to 32 bytes can be sent.
{                                            //  index[width]:bytes so far - 256 bits max packet size
  uint8_t  SHARED_SECRET;                    //  0[8]:1
  uint8_t  senderId;                         //  8[8]:1
  uint32_t encoderPosition;                  //  16[32]:4
  uint8_t  animationId;                      //  48[8]:1
  // uint32_t keyframe;                      //  56[?]:1-25
                                             // 255[0]:0
};


NRFLite _radio;
RadioPacket _radioData;

// ezscb.com esp32 feather ~v1 SPI2/HSPI 
const static uint8_t PIN_RADIO_CE = 27;
const static uint8_t PIN_RADIO_CSN = 15;
const static uint8_t PIN_RADIO_MOSI = 13;
const static uint8_t PIN_RADIO_MISO = 12;
const static uint8_t PIN_RADIO_SCK = 14;
// PIN_RADIO_IRQ = 33
const static uint8_t RADIO_ID = (uint8_t)random();
const static uint8_t SHARED_RADIO_ID = 1; // from after litewarm master 3ea81e3f4b1211809066e6f9649927cf23428956
const static uint8_t SHARED_SECRET = 42;  // bikelight scarves use this & radio_id = 1



#include "animations/ColorChooser.h"
#include "animations/Crossfade.h"
#include "animations/DiamondNecklace.h"
#include "animations/Dimmer.h"
#include "animations/FuckMyEyes.h"
#include "animations/Race.h"
#include "animations/Rainbow.h"
#include "animations/Stars.h"
#include "animations/Stripes.h"

// Load animations...
Crossfade crossfade(knob, leds);
ColorChooser color_chooser(knob, leds);
Race race(knob, leds);
Stars stars(knob, leds);
Rainbow rainbow(knob, leds);
FuckMyEyes fuck_my_eyes(knob, leds);
Stripes stripes(knob, leds);
DiamondNecklace diamond_necklace(knob, leds);
Dimmer dimmer(knob, leds);

Animation *current_animation = &diamond_necklace;

int animation_index = 0;
int previous_animation_index = -1;
void playAnimation(){
  if (animation_index != previous_animation_index) {
    if (animation_index > 8) animation_index = 0;
    // BUG CAUTION
    // never follow one animation function immediately with itself in the the
    // next case
    switch (animation_index) {
      case 0:
        current_animation = &crossfade;
        break;
      case 1:
        current_animation = &color_chooser;
        break;
      case 2:
        current_animation = &race;
        break;
      case 3:
        current_animation = &stars;
        // current_animation = &stripes;
        break;
      case 4:
        current_animation = &rainbow;
        break;
      case 5:
        current_animation = &fuck_my_eyes;
        break;
      case 6:
        // TODO figure out why Stripes causes kernel panic
        // current_animation = &stripes;
        // temporary reuse other animation
        current_animation = &stars;
        break;
      case 7:
        current_animation = &diamond_necklace;
        break;
      case 8:
        current_animation = &dimmer;
        break;
      default:
        // Serial.println("\n\nWARN: default animation switch case");
        break;
    }
    current_animation->setup();
    previous_animation_index = animation_index;
    _radioData.animationId = animation_index;
  }
  current_animation->run();
  FastLED.show();
}



///////////////////////////////////////////////////////////////////////////////////////////
// init rotary encoder
// ESP32Encoder encoder;
// uint32_t encoderPosition = 0;

// Bounce2::Button button_debouncer = Bounce2::Button();



///////////////////////////////////////////////////////////////////////////////////////////
// setup
void setup() {
    // Initialize serial communication at 115200 bits per second:
    Serial.begin(115200);


    // FastLED
    // https://github.com/FastLED/FastLED/blob/master/src/FastLED.h#L246
    FastLED.addLeds<APA102, LED_spiMosi, LED_spiClk, BGR>(leds, NUMPIXELS);  // BGR ordering is typical
    // FastLED.setBrightness(84);
    Serial.println("main.setup(): FastLED.addLeds() complete\n");
    fill_solid(leds, NUMPIXELS, CRGB::Green);
    // fill_solid(leds, NUMPIXELS, CRGB::Blue);
    FastLED.show();
    delay(1000);



    // nrf24 radio 
    // https://nrf24.github.io/RF24/index.html#autotoc_md45
    SPI.begin(PIN_RADIO_SCK, PIN_RADIO_MISO, PIN_RADIO_MOSI, PIN_RADIO_CSN);
    // Indicate to NRFLite that it should not call SPI.begin() during initialization since it has already been done.
    uint8_t callSpiBegin = 0;
    // init radio w common ID so all are in "broadcast" mode
    // TODO NRFLite::BITRATE250KBPS compat w/ bikelites, any improvements?
    if (!_radio.init(SHARED_RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100, callSpiBegin))
    {
        Serial.println("Cannot communicate with radio");
        // while (1); // Wait here forever.
    }
    Serial.println("main.setup(): _radio.init() complete");
    _radioData.senderId = RADIO_ID;
    _radioData.encoderPosition = 255; // 255 for now to indicate init but invalid
    _radioData.animationId = 255;     // 255 for now to indicate init but invalid
    

    // CALL MyKnob.Setup()
    knob.setup();

    // Rotary Encoder Knob
    // https://github.com/madhephaestus/ESP32Encoder/blob/master/examples/Encoder/Encoder.ino
    // ESP32Encoder::useInternalWeakPullResistors=UP;
    // // use pin 19 and 18 for the first encoder
    // encoder.attachHalfQuad(17, 16);
    // // clear the encoder's raw count and set the tracked count to zero
    // encoder.clearCount();
    // Serial.println("Encoder Start = " + String((int32_t)encoder.getCount()));


    // button_debouncer.attach(buttonPin, INPUT_PULLUP);
    // button_debouncer.interval(25);



    Serial.printf("finished setup\n");
    // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}



///////////////////////////////////////////////////////////////////////////////////////////
// main loop
// TODO refactor loop() so millis() inner-loop state not needed - ugly
uint32_t now = 0;
bool localUpdate = false;
bool remoteUpdate = false;

void loop() {
  // tracking updates in superloop (not concurrent friendly!!!)
  playAnimation();
  localUpdate = false;

  // can set localUpdate true
  knob.check(&animation_index, &localUpdate);

  // (millis() will always not always be called w/ sub-ms frequency)
  now = millis();


  // enter SEND mode AT MOST every ~5 sec or so (currently 5 sec heartbeat)
  if (now % 5000 == 0 || localUpdate)
  {
      _radioData.animationId = animation_index;
      _radioData.encoderPosition = knob.get();
      String msg = "<== SENT [";
      msg += RADIO_ID;
      msg += "=>";
      msg += SHARED_RADIO_ID;
      msg += "]: ";
      msg += _radioData.animationId;
      msg += " -- ";
      msg += _radioData.encoderPosition; 
      Serial.println(msg);

      if (_radio.send(SHARED_RADIO_ID, &_radioData, sizeof(_radioData)), NRFLite::NO_ACK)
      {
          // Serial.println("...Success");
      }
      else
      {
          Serial.println("...Failed");
      }
  }
  // SIDE-EFFECT: hasData() leaves radio in receive mode
  while(_radio.hasData())
  {
      _radio.readData(&_radioData);
      
      String msg = "==> RCVD [";
      msg += _radioData.senderId;
      msg += "=>";
      msg += SHARED_RADIO_ID;
      msg += "]: ";
      msg += _radioData.animationId;
      msg += " -- ";
      msg += _radioData.encoderPosition;

      Serial.println(msg);

      animation_index = _radioData.animationId;
      knob.set(_radioData.encoderPosition);
      Serial.print(knob.get());
      Serial.println(": setting encoderPosition from incoming packet");
  }
  // playAnimation();
}
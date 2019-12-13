#include <NRFLite.h>
#include <SPI.h>
#include <stdint.h>
#include "config.h"

// switched rotaryPosition to smaller value
// uint32_t rotaryPosition;  // 48
struct RadioPacket          // Any packet up to 32 bytes can be sent.
{                           // 0 - bit count (256 max)
  uint8_t SHARED_SECRET;    // 8
  uint8_t senderId;         // 16
  int16_t rotaryPosition;   // 32
  uint8_t animationId;      // 40
                            // uint32_t keyframe;       //
                            // ... 200
};

class Radio {
 private:
  RadioPacket _incomingRadioPacket;
  RadioPacket _outboundRadioPacket;
  NRFLite _radio;
  bool radioAlive = false;
  MyKnob &knob;
  int &animation_index;
  const static uint8_t SHARED_RADIO_ID = 1;
  uint16_t RADIO_ID = random(1024,65532);
  const static uint8_t SHARED_SECRET = 42;
  // const static uint8_t PIN_RADIO_CE = 7;   // 7 on PCBs 1.3, was 6 on 1.1
  // const static uint8_t PIN_RADIO_CSN = 6;  // 6 on PCBs 1.3, was 7 on 1.1
  // const static uint8_t PIN_RADIO_CE = 9; // mac protoboard
  // const static uint8_t PIN_RADIO_CSN = 10; // mac protoboard
  // const static uint8_t PIN_RADIO_CE = CONF_PIN_RADIO_CE;
  // const static uint8_t PIN_RADIO_CSN = CONF_PIN_RADIO_CSN;
  const static uint8_t PIN_RADIO_CE = 5;
  const static uint8_t PIN_RADIO_CSN = 10; // is CSN the same as SS? pin 10 on teensylc

  int previousAnimationIndex;
  int16_t previousRotaryPosition = -1;
  long lastIntervalTime = millis();

  void checkRadioReceive() {
    while (_radio.hasData()) {
      _radio.readData(&_incomingRadioPacket);
      if (_incomingRadioPacket.SHARED_SECRET != SHARED_SECRET) {
        Serial.println("xxxxxxxxx INCOMING fail xxxxxxxxxx");
        Serial.print("incoming SHARED_SECRET: ");
        Serial.print(_incomingRadioPacket.SHARED_SECRET);
        Serial.print(" != ");
        Serial.print(SHARED_SECRET);
        Serial.print("\n\n");
        return;
      }
      if (RADIO_DEBUG) {
        Serial.println("------INCOMING---------");
        Serial.print("SHARED_SECRET: ");
        Serial.println(_incomingRadioPacket.SHARED_SECRET);
        Serial.print("rotaryPosition: ");
        Serial.println(_incomingRadioPacket.rotaryPosition);
        Serial.print("animationId: ");
        Serial.println(_incomingRadioPacket.animationId);
        Serial.print("senderId: ");
        Serial.println(_incomingRadioPacket.senderId);
      }
      if (stateChanged()) {
        knob.set(_incomingRadioPacket.rotaryPosition);
        animation_index = _incomingRadioPacket.animationId;
        lastIntervalTime = millis();
      }
    }
  }

  void checkRadioSend() {
    // Stay silent for the first moment in order to try to join a group rather
    // than telling all others to change.
    if (millis() < 2500) {
      return;
    }
    if (RADIO_DEBUG) {
      Serial.println("--- Sending Data");
    }
    _outboundRadioPacket.rotaryPosition = knob.get();
    _outboundRadioPacket.animationId = animation_index;

    _radio.send(SHARED_RADIO_ID, &_outboundRadioPacket,
                sizeof(_outboundRadioPacket), NRFLite::NO_ACK);
  }

  bool stateChanged() {
    // BUG THIS COULD BE AS LARGE AS INT32 (drop it down to INT16? -32767 - 32767 ?)
    int16_t currentRotaryPosition = knob.get();
    return !(_incomingRadioPacket.rotaryPosition == currentRotaryPosition &&
             _incomingRadioPacket.animationId == animation_index);
  }

  bool runAfterInterval(int interval) {
    long nextRunTime = lastIntervalTime + interval;
    long current = millis();
    if (current > nextRunTime) {
      lastIntervalTime = current;
      return true;
    }
    return false;
  }

 public:
  Radio(MyKnob &knob_, int &animation_index_): knob(knob_), animation_index(animation_index_) {}
  
  void setup() {
    _outboundRadioPacket.SHARED_SECRET = SHARED_SECRET;
    _outboundRadioPacket.senderId = RADIO_ID;
    if (RADIO_DEBUG) {
      Serial.print("Picking random radio id: ");
      Serial.println(RADIO_ID);
    }
    
    // MAC 12/12/19 TODO DELETE
    // digital pin 14 = A0 = buttonPin
    // button_debouncer() in main sets INPUT_PULLUP so we don't need the following line
    // pinMode(14, INPUT_PULLUP);
    pinMode(PIN_RADIO_CE, OUTPUT);
    digitalWrite(PIN_RADIO_CE, HIGH);
    pinMode(PIN_RADIO_CSN, OUTPUT);

    Serial.println("------CE & CSN---------");
    Serial.print("PIN_RADIO_CE (chip enable): ");
    Serial.println(PIN_RADIO_CE);
    Serial.print("PIN_RADIO_CSN (chip select / slave select)?: ");
    Serial.println(PIN_RADIO_CSN);
    // ========================

    if (!_radio.init(SHARED_RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN)) {
      // if (!_radio.init(SHARED_RADIO_ID, CONF_PIN_RADIO_CE, CONF_PIN_RADIO_CSN)) {
      radioAlive = false;
      if (RADIO_DEBUG) {
        Serial.println("radio fail");
      }
    } else {
      radioAlive = true;
      if (RADIO_DEBUG) {
        Serial.println("radio ok");
      }
    }
  }
  void check() {
    // int newRotaryPosition = knob.get();
    knob.get();
    if (radioAlive) {
      checkRadioReceive();
      if (knob.manuallyChanged() || runAfterInterval(2000)) {
        checkRadioSend();
      }
    }
  }
};

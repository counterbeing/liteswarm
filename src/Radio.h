#include <NRFLite.h>
#include <SPI.h>
#include <stdint.h>
#include "config.h"

struct RadioPacket        // Any packet up to 32 bytes can be sent.
{                         // 0 - bit count (256 max)
  uint8_t SHARED_SECRET;  // 8
  uint8_t senderId;       // 16
  uint8_t var1;           // 24
  uint8_t var2;           // 32
  uint8_t animationId;    // 40
                          // uint32_t keyframe;       //
                          // ... 200
};

class Radio {
 private:
  bool radioAlive = false;
  MyKnob &knob;
  int &animation_index;
  AnimationVariables &aniVars;
  RadioPacket _incomingRadioPacket;
  RadioPacket _outboundRadioPacket;
  NRFLite _radio;
  const static uint8_t SHARED_RADIO_ID = 1;
  const static uint8_t PIN_RADIO_CE = 7;   // 7 on PCBs 1.3, was 6 on 1.1
  const static uint8_t PIN_RADIO_CSN = 6;  // 6 on PCBs 1.3, was 7 on 1.1
  // const static uint8_t PIN_RADIO_CE = 9; // mac protoboard
  // const static uint8_t PIN_RADIO_CSN = 10; // mac protoboard
  const static uint8_t SHARED_SECRET = 42;
  uint8_t RADIO_ID = random();
  int previousAnimationIndex;
  long lastIntervalTime = millis();

  void checkRadioReceive() {
    while (_radio.hasData()) {
      _radio.readData(&_incomingRadioPacket);
      if (_incomingRadioPacket.SHARED_SECRET != SHARED_SECRET) {
        return;
      }
      if (RADIO_DEBUG) {
        Serial.println("------INCOMING---------");
        Serial.print("SHARED_SECRET: ");
        Serial.println(_incomingRadioPacket.SHARED_SECRET);
        Serial.print("var1: ");
        Serial.println(_incomingRadioPacket.var1);
        Serial.print("var2: ");
        Serial.println(_incomingRadioPacket.var2);
        Serial.print("animationId: ");
        Serial.println(_incomingRadioPacket.animationId);
        Serial.print("senderId: ");
        Serial.println(_incomingRadioPacket.senderId);
      }
      if (stateChanged()) {
        aniVars.var1 = _incomingRadioPacket.var1;
        aniVars.var2 = _incomingRadioPacket.var2;
        animation_index = _incomingRadioPacket.animationId;
        lastIntervalTime = millis();
      }
    }
  }

  void checkRadioSend() {
    // Stay silent for the first moment in order to try to join a group rather
    // than telling all others to change.
    if (millis() < 1500) {
      return;
    }
    if (RADIO_DEBUG) {
      Serial.println("--- Sending Data");
    }
    _outboundRadioPacket.var1 = aniVars.var1;
    _outboundRadioPacket.var2 = aniVars.var2;
    _outboundRadioPacket.animationId = animation_index;

    _radio.send(SHARED_RADIO_ID, &_outboundRadioPacket,
                sizeof(_outboundRadioPacket), NRFLite::NO_ACK);
  }

  bool stateChanged() {
    return !(_incomingRadioPacket.var1 == aniVars.var1 &&
             _incomingRadioPacket.var2 == aniVars.var2 &&
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

  // constructor declaration vvvvvvvvvvvv : vvvvvvvvvvv -member initializer
  // list- vvvvvvvv
 public:  //                                             vvvvv <--init named
          //                                             identifier (knob) with
          //                                             these params (knob_)
  Radio(MyKnob &knob_, int &animation_index_, AnimationVariables &aniVars_)
      : knob(knob_), animation_index(animation_index_), aniVars(aniVars_) {}
  void setup() {
    _outboundRadioPacket.SHARED_SECRET = SHARED_SECRET;
    _outboundRadioPacket.senderId = RADIO_ID;
    if (RADIO_DEBUG) {
      Serial.print("Picking random radio id: ");
      Serial.println(RADIO_ID);
    }
    pinMode(14, INPUT_PULLUP);
    if (!_radio.init(SHARED_RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN)) {
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
    // knob.get();
    if (radioAlive) {
      checkRadioReceive();
      if (knob.manuallyChanged() || runAfterInterval(1000)) {
        checkRadioSend();
      }
    }
  }
};

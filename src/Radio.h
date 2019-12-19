#include <NRFLite.h>
#include <SPI.h>
#include <stdint.h>
#include "config.h"

// compiler will automatically pad struct differently depending on architecture
// atmega328 is 8bit; teensy is 32bit; (can be disabled with `#pragma pack(1)`)
// the padding happens if larger members come after smaller members. so quick fix
// is to organize members from largest to smallest.
// 
// struct RadioPacket          // Any packet up to 32 bytes can be sent.
// {                           // 0 - bit count (256 max); byte count
//   uint8_t SHARED_SECRET;    // 8  : 1
//   int32_t rotaryPosition;   // 40 : 5
//   int8_t animationId;       // 48 : 6
//   uint16_t senderId;        // 64 : 8
//                             // ... 256 : 32
// };
struct RadioPacket          // Any packet up to 32 bytes can be sent.
{                           // 0 - bit count (256 max); byte count
  int32_t rotaryPosition;   // 32 : 4
  uint16_t senderId;        // 48 : 6
  uint8_t SHARED_SECRET;    // 56 : 7
  int8_t animationId;       // 64 : 8
                            // ... 256 : 32
};

class Radio {
 private:
  RadioPacket _incomingRadioPacket;
  RadioPacket _outboundRadioPacket;
  NRFLite _radio;
  bool radioAlive = false;
  MyKnob &knob;
  int8_t &animation_index;
  const static uint8_t SHARED_RADIO_ID = 1;
  uint16_t RADIO_ID = random(1024,65400);
  const static uint8_t SHARED_SECRET = 42;

  //  previousAnimationIndex; // unusued?
  int32_t previousRotaryPosition = -1;
  long lastIntervalTime = millis();

  void checkRadioReceive() {
    while (_radio.hasData()) {
      _radio.readData(&_incomingRadioPacket);
      if (_incomingRadioPacket.SHARED_SECRET != SHARED_SECRET) {
        Serial.println("\n\nxxxxxxxxx INCOMING fail xxxxxxxxxx");
        Serial.print("incoming SHARED_SECRET: ");
        Serial.print(_incomingRadioPacket.SHARED_SECRET);
        Serial.print(" != ");
        Serial.print(SHARED_SECRET);
        Serial.print("\n\n");
        // return;
      }
      if (RADIO_DEBUG) {
        Serial.println("\n------INCOMING---------");
        Serial.print("SHARED_SECRET:  ");
        Serial.println(_incomingRadioPacket.SHARED_SECRET);
        Serial.print("rotaryPosition: ");
        Serial.print(_incomingRadioPacket.rotaryPosition);
        Serial.print("  (local: ");
        Serial.print(knob.get());
        Serial.print(")   state changed?: ");
        Serial.print(stateChanged());
        Serial.print("\nanimationId:    ");
        Serial.print(_incomingRadioPacket.animationId);
        Serial.print("  (local: ");
        Serial.print(animation_index);
        Serial.print(")\nsenderId:       ");
        Serial.println(_incomingRadioPacket.senderId);
      }
      if (stateChanged()) {
        knob.set(_incomingRadioPacket.rotaryPosition);
        knob.confine(); // HACK to prevent knob.confine() bug below
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
    knob.confine(); // HACK to prevent bug below
    _outboundRadioPacket.SHARED_SECRET = SHARED_SECRET;
    _outboundRadioPacket.rotaryPosition = knob.get();
    _outboundRadioPacket.animationId = animation_index;  // extraneous?
    _outboundRadioPacket.senderId = RADIO_ID;            // extraneous?

    _radio.send(SHARED_RADIO_ID, &_outboundRadioPacket,
                sizeof(_outboundRadioPacket), NRFLite::NO_ACK);

    if (RADIO_DEBUG) {
      Serial.println("\n--- Sending Data...\n");
      // Serial.print("\t`sizeof(_outboundRadioPacket)`: ");
      // Serial.print(sizeof(_outboundRadioPacket));
    }
  }

  bool stateChanged() {
    // BUG - when local knob position is outside of current animations knob.confine() range,
    // something causes an endless ping-pong of packets b/w units; incoming position could be 4
    // but local position is 5 (because confine), so it gets changed, causing manuallyChanged()
    // to be true (not sure why), causing an update to be sent w/ knobPosition = 5... 
    knob.confine(); // HACK to prevent bug 
    int32_t currentRotaryPosition = knob.get(); 
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
  Radio(MyKnob &knob_, int8_t &animation_index_): knob(knob_), animation_index(animation_index_) {}
  
  void setup() {

    if (RADIO_DEBUG) {
      Serial.print("Picking random radio id: ");
      Serial.println(RADIO_ID);
    }
    
    // MAC 12/12/19 TODO DELETE?
    // teensy sets all pins as input by default
    // radio needs CE high otherwise it goes to sleep?
    /*********************** NRFLite.cpp#L564 ******************************************
      digitalWrite(_csnPin, LOW);              // Signal radio to list on the SPI bus.
      delayMicroseconds(CSN_DISCHARGE_MICROS); // Allow capacitor on CSN pin to discharge.
      ...
      digitalWrite(_csnPin, HIGH);             // Stop radio from listening to the SPI bus.
      ...
      digitalWrite(_csnPin, LOW); // Signal radio to list on the SPI bus.
    ************************************************************************************/
    pinMode(PIN_RADIO_CE, OUTPUT);
    // digitalWrite(PIN_RADIO_CE, HIGH);
    pinMode(PIN_RADIO_CSN, OUTPUT);

    Serial.println("------CE & CSN---------");
    Serial.print("PIN_RADIO_CE (chip enable): ");
    Serial.println(PIN_RADIO_CE);
    Serial.print("PIN_RADIO_CSN (chip select / slave select)?: ");
    Serial.println(PIN_RADIO_CSN);
    // ========================

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
    // int newRotaryPosition = knob.get();
    // knob.get();
    if (radioAlive) {
      if (knob.manuallyChanged() || runAfterInterval(3000)) {
        Serial.print("\nknob.manuallyChanged(): ");
        Serial.print(knob.manuallyChanged());
        Serial.print("\nrunAfterInterval(3000): ");
        Serial.println(runAfterInterval(3000));
        checkRadioSend();
      }
      checkRadioReceive();
    }
  }
};

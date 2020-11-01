#include "DebugLog.h"
#include "config.h"
#include <NRFLite.h>
#include <SPI.h>
#include <stdint.h>

struct RadioPacket         // Any packet up to 32 bytes can be sent.
{                          // 0 - bit count (256 max)
  uint8_t SHARED_SECRET;   // 8
  uint8_t senderId;        // 16
  uint32_t rotaryPosition; // 48
  uint8_t animationId;     // 56
                           // uint32_t keyframe;       //
                           // ... 200
};

class Radio {
 private:
  RadioPacket _incomingRadioPacket;
  RadioPacket _outboundRadioPacket;
  NRFLite _radio;
  bool radioAlive = false;
  const static uint8_t SHARED_RADIO_ID = 1;
  const static uint8_t PIN_RADIO_CE = 7;  // 7 on PCBs 1.3, was 6 on 1.1
  const static uint8_t PIN_RADIO_CSN = 6; // 6 on PCBs 1.3, was 7 on 1.1
  // const static uint8_t PIN_RADIO_CE = 9; // mac protoboard
  // const static uint8_t PIN_RADIO_CSN = 10; // mac protoboard
  const static uint8_t SHARED_SECRET = 42;
  uint8_t RADIO_ID = random();
  const uint8_t numAnimations;

 public:
  Radio(const uint8_t numAnimations_) : numAnimations(numAnimations_) {}

  bool checkRadioReceive() {
    if (!radioAlive)
      return false;

    while (_radio.hasData()) {
      _radio.readData(&_incomingRadioPacket);

      if (_incomingRadioPacket.SHARED_SECRET != SHARED_SECRET)
        continue;
      if (_incomingRadioPacket.animationId < 0 ||
          _incomingRadioPacket.animationId >= numAnimations)
        continue;

      if (RADIO_DEBUG) {
        debugLo("Radio receiving from ");
        debugLo(_incomingRadioPacket.senderId);
        debugLo(" @ ");
        debugLo(millis());
        debugLo(": animationId=");
        debugLo(_incomingRadioPacket.animationId);
        debugLog(", rotaryPosition=", _incomingRadioPacket.rotaryPosition);
      }
      return true;
    }
    return false;
  }

  uint8_t getLatestReceivedAnimationId() {
    return _incomingRadioPacket.animationId;
  }

  uint32_t getLatestReceivedRotaryPosition() {
    return _incomingRadioPacket.rotaryPosition;
  }

  void send(const uint8_t animationId, const uint32_t rotaryPosition) {
    if (!radioAlive)
      return;

    // Stay silent for the first moment in order to try to join a group rather
    // than telling all others to change.
    if (millis() < 1500)
      return;

    _outboundRadioPacket.rotaryPosition = rotaryPosition;
    _outboundRadioPacket.animationId = animationId;

    if (RADIO_DEBUG) {
      debugLo("Radio sending @ ");
      debugLo(millis());
      debugLo(": animationId=");
      debugLo(_outboundRadioPacket.animationId);
      debugLog(", rotaryPosition=", _outboundRadioPacket.rotaryPosition);
    }

    _radio.send(SHARED_RADIO_ID, &_outboundRadioPacket,
                sizeof(_outboundRadioPacket), NRFLite::NO_ACK);
  }

  void setup() {
    pinMode(14, INPUT_PULLUP);

    _outboundRadioPacket.SHARED_SECRET = SHARED_SECRET;
    _outboundRadioPacket.senderId = RADIO_ID;

    radioAlive = _radio.init(SHARED_RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN);

    if (RADIO_DEBUG) {
      debugLog("shared radio id=", SHARED_RADIO_ID);
      debugLog("random radio id=", RADIO_ID);
      debugLog(radioAlive ? "radio ok" : "radio fail");
    }
  }
};

#include <stdint.h>
#include <NRFLite.h>
#include <SPI.h>

struct RadioPacket // Any packet up to 32 bytes can be sent.
{

    uint8_t sharedSecret;
    // uint8_t senderId;
    // uint8_t action;
    uint32_t rotaryPosition;
    // uint32_t animationId;
};

class Radio
{
private:
    RadioPacket _radioData;
    NRFLite _radio;
    bool radioAlive = false;
    // int *animationIndex;
    MyKnob &knob;
    const static uint8_t SHARED_RADIO_ID = 1;
    const static uint8_t PIN_RADIO_CE = 9;           // hardware pins
    const static uint8_t PIN_RADIO_CSN = 10;         // hardware pins
    const static uint8_t SHARED_SECRET = 42;


    void checkRadioReceive()
    {
        while (_radio.hasData())
        {
            _radio.readData(&_radioData);
            Serial.println("-----------------------");
            Serial.print("sharedSecret: ");
            Serial.println(_radioData.sharedSecret);
            Serial.print("rotaryPosition: ");
            Serial.println(_radioData.rotaryPosition);
            knob.set(_radioData.rotaryPosition);
        }
    }

    void checkRadioSend()
    {
        _radioData.rotaryPosition = knob.get();
        Serial.println(_radioData.rotaryPosition);
        _radioData.sharedSecret = SHARED_SECRET;
        _radio.send(SHARED_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::NO_ACK);
    }

public:
    Radio(MyKnob &knob_) : knob(knob_) {}
    void setup()
    {
        pinMode(14, INPUT_PULLUP);
        if (!_radio.init(SHARED_RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
        {
            radioAlive = false;
            Serial.println("radio fail");
        }
        else
        {
            radioAlive = true;
            Serial.println("radio ok");
        }
    }
    void check()
    {
        if (radioAlive)
        {
            checkRadioReceive();
            checkRadioSend();
        }
    }
};

#include <stdint.h>
#include <NRFLite.h>
#include <SPI.h>

struct RadioPacket // Any packet up to 32 bytes can be sent.
{

    uint8_t sharedSecret;
    // uint8_t senderId;
    uint32_t rotaryPosition;
    uint8_t animationId;
    // uint32_t keyframe;
};

class Radio
{
private:
    RadioPacket _radioData;
    NRFLite _radio;
    bool radioAlive = false;
    // int *animationIndex;
    MyKnob &knob;
    int &animation_index;
    const static uint8_t SHARED_RADIO_ID = 1;
    const static uint8_t PIN_RADIO_CE = 9;   // hardware pins
    const static uint8_t PIN_RADIO_CSN = 10; // hardware pins
    const static uint8_t SHARED_SECRET = 42;
    int previousAnimationIndex;
    int previousRotaryPosition = -100;

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
            Serial.print("animationId: ");
            Serial.println(_radioData.animationId);
            knob.set(_radioData.rotaryPosition);
            animation_index = _radioData.animationId;
        }
    }

    void checkRadioSend()
    {
        Serial.println("--- Sending Data");
        _radioData.rotaryPosition = knob.get();
        _radioData.sharedSecret = SHARED_SECRET;
        _radioData.animationId = animation_index;
        _radio.send(SHARED_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::NO_ACK);
    }

public:
    Radio(MyKnob &knob_, int &animation_index_) : knob(knob_), animation_index(animation_index_) {}
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
        int newRotaryPosition = knob.get();
        if (radioAlive)
        {
            checkRadioReceive();
            if (!(previousRotaryPosition == newRotaryPosition))
            {
                Serial.print("Previous rotary: ");
                Serial.println(previousRotaryPosition);
                previousRotaryPosition = newRotaryPosition;
                checkRadioSend();
            }
        }
    }
};

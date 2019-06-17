#include <MyKnob.h>
#include "FastLED.h"
#include "Animation.h"

const int NUMPIXELS = 76;

class DiamondNecklace : public Animation
{
private:
    int initialPosition = 150;
    int start = 5;
    int finish = 300;
    long previousMillis = 0;
    bool initialized = false;
    int head = 0;
    bool loopRotary = false;
    MyKnob &knob;
    CRGB *leds;

    void setup()
    {
        knob.setDefaults(
            initialPosition,
            start,
            finish,
            loopRotary);
    };

    void loop()
    {
        int dlay = knob.confine();
        static uint8_t hue = 0;
        if (nonBlockDelay(dlay))
        {
            FastLED.showColor(CHSV(hue++, 255, 255));
        }
    }

public:
    DiamondNecklace(MyKnob &knob_, CRGB leds_[]) : knob(knob_), leds(leds_) {}
    void run()
    {
        if (!initialized)
        {
            setup();
            initialized = true;
        };
        loop();
    }
};

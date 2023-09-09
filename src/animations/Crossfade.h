#include <MyKnob.h>
#include "FastLED.h"
#include "Animation.h"

class Crossfade : public Animation
{
private:
    int initialPosition = 40;
    int start = 5;
    int finish = 200;
    bool initialized = false;
    int head = 0;
    bool loopRotary = false;
    MyKnob &knob;
    CRGB *leds;


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
    void setup()
    {
        knob.setDefaults(
            initialPosition,
            start,
            finish,
            loopRotary);
    };

    Crossfade(MyKnob &knob_, CRGB leds_[]) : knob(knob_), leds(leds_) {}
};
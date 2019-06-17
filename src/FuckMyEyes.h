#include <MyKnob.h>
#include "FastLED.h"
#include "Animation.h"

class FuckMyEyes : public Animation
{
private:
    int initialPosition = 0;
    int start = 0;
    int finish = 300;
    bool initialized = false;
    int head = 0;
    bool loopRotary = false;
    MyKnob &knob;
    CRGB *leds;
    int lastColor = 0;

    void setup()
    {
        Serial.println(initialPosition);
        knob.setDefaults(
            initialPosition,
            start,
            finish,
            loopRotary);
    };

    void loop()
    {
        int dlay = knob.confine();
        if (nonBlockDelay(dlay))
        {
            switch (lastColor)
            {
            case 0:
                fill_solid(leds, 76, CRGB::Red);
                lastColor = 1;
                break;
            case 1:
                fill_solid(leds, 76, CRGB::Green);
                lastColor = 2;
                break;
            case 2:
                fill_solid(leds, 76, CRGB::Blue);
                lastColor = 0;
                break;
            }
        }
        FastLED.show();
    }

public:
    FuckMyEyes(MyKnob &knob_, CRGB leds_[]) : knob(knob_), leds(leds_) {}
};

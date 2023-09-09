#include "Animation.h"

class FindMyBike : public Animation
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
        knob.setDefaults(
            initialPosition,
            start,
            finish,
            loopRotary);
    };

    void loop()
    {
        FastLED.clear();
        if (nonBlockDelay(3000))
        {
            fill_solid( leds, NUMPIXELS, CRGB::White);
        }
        FastLED.show();
    }

public:
    FindMyBike(MyKnob &knob_, CRGB leds_[]) : knob(knob_), leds(leds_) {}
};
#include "Animation.h"

class DiamondNecklace : public Animation
{
private:
    int initialPosition = 50;
    int start = 0;
    int finish = 300;
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
        if (nonBlockDelay(dlay))
        {
            for (int i = 0; i < NUMPIXELS; i++)
            {
                leds[i].fadeLightBy( 128 );
                if(random(40) == 1) {
                    leds[i] = CRGB::White;
                }

            }
            FastLED.show();
            FastLED.delay(1000 / FRAMES_PER_SECOND); // TODO this may be bad idea - research
        }
    }

public:
    DiamondNecklace(MyKnob &knob_, CRGB leds_[]) : knob(knob_), leds(leds_) {}
};
#include <MyKnob.h>
#include "FastLED.h"
#include "Animation.h"

class DiamondNecklace : public Animation
{
private:
    int initialPosition = 300;
    int start = 5;
    int finish = 300;
    bool initialized = false;
    int head = 0;
    bool loopRotary = false;
    MyKnob &knob;
    CRGB *leds;

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
        static uint8_t hue = 0;

        if (nonBlockDelay(dlay))
        {
            for (int i = 0; i < 76; i++)
            {
                leds[i].fadeLightBy( 128 );
                if(random(40) == 1) {
                    leds[i] = CRGB::White;
                }

            }
            FastLED.show();
        }
    }

public:
    DiamondNecklace(MyKnob &knob_, CRGB leds_[]) : knob(knob_), leds(leds_) {}
};

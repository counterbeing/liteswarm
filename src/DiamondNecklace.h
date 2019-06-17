#include <MyKnob.h>
#include "FastLED.h"

const int NUMPIXELS = 76;

class DiamondNecklace
{
private:
    int start = 100;
    int finish = 200;
    int initialSetting = 150;
    long previousMillis = 0;
    bool initialized = false;
    int head = 0;
    void setup(){};
    MyKnob &knob;
    CRGB *leds;

    void loop()
    {
        int dlay = knob.confine(1, 500);
        static uint8_t hue = 0;
        if (nonBlockDelay(dlay))
        {
            FastLED.showColor(CHSV(hue++, 255, 255));
        }
    }

    bool nonBlockDelay(unsigned int interval)
    {
        unsigned long currentMillis = millis();
        if ((currentMillis - previousMillis) > interval)
        {
            previousMillis = currentMillis;
            return true;
        }
        return false;
    }

public:
    DiamondNecklace(MyKnob &knob_, CRGB leds_[]) : knob(knob_), leds(leds_) {}
    void run()
    {
        if (!initialized)
        {
            setup();
        };
        loop();
    }
};

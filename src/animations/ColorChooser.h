#include "Animation.h"

class ColorChooser : public Animation
{
private:
    int initialPosition = 180;
    int start = 0;
    int finish = 255;
    bool initialized = false;
    int head = 0;
    bool loopRotary = true;
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
        int color = knob.confine();
        fill_solid(leds, NUMPIXELS, CHSV(color, 255, 255));
        FastLED.show();
    }

public:
    ColorChooser(MyKnob &knob_, CRGB leds_[]) : knob(knob_), leds(leds_) {}
};
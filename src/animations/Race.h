#include "Animation.h"

class Race : public Animation
{
private:
    int initialPosition = 60;
    int start = 5;
    int finish = 400;
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

    // Associativity specification is redundant for unary operators and is only
    // shown for completeness: unary prefix operators always associate right-to-left
    // (delete ++*p is delete(++(*p))) and unary postfix operators always associate
    // left-to-right (a[1][2]++ is ((a[1])[2])++). Note that the associativity is
    // meaningful for member access operators, even though they are grouped with
    // unary postfix operators: a.b++ is parsed (a.b)++ and not a.(b++).
    void loop()
    {
        int dlay = knob.confine();
        if (nonBlockDelay(dlay))
        {
            static uint8_t hue = 0;
            head = remapInRange(++head);
            leds[head] = CRGB::Black;
            leds[head] = CHSV(hue++, 255, 150);
            head = remapInRange(++head);
            leds[head] = CHSV(hue++, 255, 255);
            FastLED.show();
        }
    }

public:
    Race(MyKnob &knob_, CRGB leds_[]) : knob(knob_), leds(leds_) {}
};
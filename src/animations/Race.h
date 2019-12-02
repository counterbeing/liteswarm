#include "Animation.h"
#include "MilliTimer.h"

class Race : public Animation {
 private:
  uint8_t hue = 0;
  uint32_t pixelIndex = 0;
  int32_t delay = 60;
  CRGB *leds;
  MilliTimer timer{};
  KnobControl knobControl{5, 400, false};

  uint16_t nextPixelIndex() {
    if (++pixelIndex > NUMPIXELS)
      pixelIndex = 0;
    return pixelIndex;
  }

 public:
  Race(CRGB leds_[]) : leds(leds_) {}

  void wakeUp() {
    knobControl.setPosition(delay);
  }

  // Associativity specification is redundant for unary operators and is only
  // shown for completeness: unary prefix operators always associate
  // right-to-left (delete ++*p is delete(++(*p))) and unary postfix operators
  // always associate left-to-right (a[1][2]++ is ((a[1])[2])++). Note that the
  // associativity is meaningful for member access operators, even though they
  // are grouped with unary postfix operators: a.b++ is parsed (a.b)++ and not
  // a.(b++).
  void update() {
    if (knobControl.updateSettingOnChange(delay)) {
      configChangeFlag = true;
      if (ANIM_DEBUG) debugLog("config change: Race::delay = ", delay);
    }

    if (timer.hasElapsedWithReset(delay)) {
      // leds[nextPixelIndex()] = CHSV(hue++, 255, 150);
      leds[nextPixelIndex()] = CHSV(hue++, 255, 255);
      
    }
  }

};

#include "Animation.h"
#include "Knob.h"
#include "MilliTimer.h"

class Race : public Animation {
 private:
  Knob delay{60, 5, 400, false};
  MilliTimer timer{};
  uint8_t hue = 0;
  uint32_t pixelIndex = 0;

  uint16_t nextPixelIndex() {
    if (++pixelIndex > NUMPIXELS)
      pixelIndex = 0;
    return pixelIndex;
  }

 public:
  Race(CRGB leds_[]) : Animation(leds_) {}

 protected:
  void activate() override { delay.activate(); }

  // Associativity specification is redundant for unary operators and is only
  // shown for completeness: unary prefix operators always associate
  // right-to-left (delete ++*p is delete(++(*p))) and unary postfix operators
  // always associate left-to-right (a[1][2]++ is ((a[1])[2])++). Note that the
  // associativity is meaningful for member access operators, even though they
  // are grouped with unary postfix operators: a.b++ is parsed (a.b)++ and not
  // a.(b++).
  bool updateAnimation(const bool justActivated) override {
    delay.update();

    if (timer.hasElapsedWithReset(delay.get())) {
      // leds[nextPixelIndex()] = CHSV(hue++, 255, 150);
      leds[nextPixelIndex()] = CHSV(hue++, 255, 255);
      return true;
    }

    return false;
  }

  uint32_t getKnobPosition() override { return delay.get(); }

  void setKnobPosition(const uint32_t newPosition) override {
    delay.set(newPosition);
  }
};

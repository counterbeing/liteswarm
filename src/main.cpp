#include <Arduino.h>
#define FASTLED_INTERNAL
#include "BaseControllers.h"
#include "DebugLog.h"
#include "FastLED.h"
#include "MilliTimer.h"
#include "Radio.h"
#include "config.h"
#include <Encoder.h>
#include <MyKnob.h>
#include <SPI.h>

#include "animations/ColorChooser.h"
#include "animations/Crossfade.h"
#include "animations/DiamondNecklace.h"
#include "animations/Dimmer.h"
#include "animations/FuckMyEyes.h"
#include "animations/Race.h"
#include "animations/Rainbow.h"
#include "animations/Stars.h"
#include "animations/Stripes.h"
// #include "animations/Strobe.h"

// Pins for the rotary
uint8_t rotary1 = 2;
uint8_t rotary2 = 3;

int buttonPin = A0;

CRGB leds[NUMPIXELS];

// Load animations...
Crossfade crossfade(leds);
ColorChooser color_chooser(leds);
Race race(leds);
Stars stars(leds);
Rainbow rainbow(leds);
FuckMyEyes fuck_my_eyes(leds);
Stripes stripes(leds);
DiamondNecklace diamond_necklace(leds);
Dimmer dimmer(leds);
// Strobe strobe(leds);

#define NUM_ANIMATONS 9
Animation * animations[NUM_ANIMATONS] = {
    &crossfade, &color_chooser,    &race,  &stars, &rainbow, &fuck_my_eyes,
    &stripes,   &diamond_necklace, &dimmer};

Radio radio{};
ButtonControl buttonControl{};

class OffModeController : public BaseController {
 protected:
  virtual void activate() override {}

  virtual void loop(bool justActivated) override {
    if (justActivated) {
      fill_solid(leds, NUMPIXELS, CRGB::Black);
      FastLED.show();
    }
  }
};

class AnimationModeController : public BaseController {
 private:
  ButtonControl & buttonControl;
  uint8_t animationIndex = 0;
  MilliTimer radioTimer{};

 public:
  AnimationModeController(ButtonControl & buttonControl_)
      : buttonControl(buttonControl_) {}

 protected:
  virtual void activate() override {}

  virtual void loop(bool justActivated) override {
    // TODO: add this back in later
    radio.checkRadioReceive();

    bool animationChanged =
        buttonControl.hasClickEventOccurred(ClickEvent::CLICK);

    if (animationChanged)
      animationIndex =
          (animationIndex == NUM_ANIMATONS - 1) ? 1 : animationIndex + 1;

    Animation * currentAnimation = animations[animationIndex];

    if (radioTimer.hasElapsedWithReset(1000))
      radio.send(animationIndex, currentAnimation->getKnobPosition());

    if (animationChanged || justActivated) {
      currentAnimation->setAsActive();
    }

    currentAnimation->run();
  }
};

class MasterController {
 private:
  ButtonControl & buttonControl;
  AnimationModeController animationModeController;
  OffModeController offModeController{};
  MilliTimer radioTimer{};
  bool offMode = false;
  bool firstLoop = true;

  void activateMode() {
    firstLoop = false;
    if (offMode) {
      offModeController.setAsActive();
    } else {
      animationModeController.setAsActive();
    }
  }

 public:
  MasterController(ButtonControl & buttonControl_)
      : buttonControl(buttonControl_), animationModeController{buttonControl_} {
  }

  void startupLoop(unsigned long now) {
    uint32_t counter = (now / 333) % 11;
    switch (counter) {
      case 1:
        fill_solid(leds, NUMPIXELS, CRGB::Red);
        break;
      case 3:
        fill_solid(leds, NUMPIXELS, CRGB::Yellow);
        break;
      case 5:
        fill_solid(leds, NUMPIXELS, CRGB::Green);
        break;
      case 7:
        fill_solid(leds, NUMPIXELS, CRGB::Blue);
        break;
      case 9:
        fill_solid(leds, NUMPIXELS, CRGB::Indigo);
        break;
      default:
        fill_solid(leds, NUMPIXELS, CRGB::Black);
        break;
    }
    FastLED.show();
  }

  void loop() {
    unsigned long now = millis();
    if (now <= 3663) { // 333 * 11 = 3663
      startupLoop(now);
      return;
    }

    button_debouncer.update();
    buttonControl.checkButton();

    bool modeChange =
        buttonControl.hasClickEventOccurred(ClickEvent::LONG_CLICK);
    if (modeChange)
      offMode = !offMode;

    if (modeChange || firstLoop) {
      firstLoop = false;
      activateMode();
    }

    if (offMode) {
      offModeController.run();
    } else {
      animationModeController.run();
    }
  }

} masterController{buttonControl};

void setup() {
#ifdef SCARF_WS2811
  FastLED.addLeds<WS2811, DATAPIN, GRB>(leds, NUMPIXELS);
#endif
#ifdef SCARF_SK9822
  FastLED.addLeds<SK9822, DATAPIN, CLOCKPIN, BGR>(leds, NUMPIXELS);
#endif

#ifdef SCARF_DOTSTAR
  FastLED.addLeds<DOTSTAR, DATAPIN, CLOCKPIN, BGR>(leds, NUMPIXELS);
#endif

#ifdef BIGRED_WS2815
  FastLED.addLeds<WS2811, DATAPIN, BGR>(leds, NUMPIXELS);
#endif
  Serial.begin(57600);

  button_debouncer.attach(buttonPin, INPUT_PULLUP);
  button_debouncer.interval(5);
  radio.setup();
}

void loop() { masterController.loop(); }
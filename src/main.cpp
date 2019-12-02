#include <Arduino.h>
#define FASTLED_INTERNAL
#include <Encoder.h>
#include <MyKnob.h>
#include <SPI.h>
#include "FastLED.h"
#include "Radio.h"
#include "config.h"
#include "DebugLog.h"
#include "MilliTimer.h"

#include "animations/ColorChooser.h"
#include "animations/Crossfade.h"
#include "animations/DiamondNecklace.h"
#include "animations/Dimmer.h"
#include "animations/FuckMyEyes.h"
#include "animations/Race.h"
#include "animations/Rainbow.h"
#include "animations/Stars.h"
#include "animations/Stripes.h"
#include "animations/Strobe.h"

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
Strobe strobe(leds);

const int NUM_ANIMATONS = 9;
// const int NUM_ANIMATONS = 2;
Animation* animations[NUM_ANIMATONS] = {
    // &crossfade, &color_chooser};
    &crossfade, &color_chooser,    &race,  &stars, &rainbow, &fuck_my_eyes,
    &stripes,   &diamond_necklace, &dimmer};

int old_animation_index = 0;
MyKnob knob{};
Radio radio(knob, old_animation_index);

class BaseController {
 private:
   bool activeFlag = false;

 public:
  void setAsActive() { activeFlag = true; }

  virtual void run() {
    if (activeFlag) {
      activeFlag = false;
      activate();
    }
    loop();
  }

 protected:
  virtual void activate() = 0;
  virtual void loop() = 0;
};

class OffModeController : public BaseController {
 protected:
  virtual void activate() {
    debugLog("OffModeController::activate()");
  }

  virtual void loop() {
      fill_solid(leds, NUMPIXELS, CRGB::Black);
      FastLED.show();
  }
};

class AnimationModeController : public BaseController {
 private:
  uint8_t animationIndex = 0;
  bool animationChanged = false;
  bool configChangeFlag = false;

 public:
  bool hasConfigChanged() {
    return configChangeFlag;
  }
  void nextAnimation() {
    animationIndex++;
    if (animationIndex == NUM_ANIMATONS) {
      animationIndex = 0;
    }
    animationChanged = true;
  }

 protected:
  virtual void activate() {
    debugLog("AnimationModeController::activate()");
  }

  virtual void loop() {
    configChangeFlag = animationChanged;

    if (animationChanged) {

      if (ANIM_DEBUG) {
        // debugLog("----------------------------------------------");
        debugLog("Animation Index = ", animationIndex);
        // debugLog("----------------------------------------------");
      }
    }

    Animation *currentAnimation = animations[animationIndex];

    if (animationChanged) {
      currentAnimation->wakeUp();
    }

    currentAnimation->loop();
    configChangeFlag = configChangeFlag || currentAnimation->hasConfigChanged();

    animationChanged = false;
    FastLED.show();
  }
};

ButtonControl buttonControl{};

class MasterController {
 private:
  OffModeController offModeController{};
  AnimationModeController animationModeController{};
  MilliTimer radioTimer{};
  bool offMode = false;

 public:
  void loop() {
    button_debouncer.update();
    buttonControl.checkButton();
    bool modeChange =
        buttonControl.hasClickEventOccurred() &&
        buttonControl.getLatestClickEvent() == ClickEvent::DOUBLE_CLICK;
    if (modeChange)
      offMode = !offMode;

    if (offMode) {
      if (modeChange)
        offModeController.setAsActive();
      offModeController.run();
    }
    else {
      if (modeChange)
        animationModeController.setAsActive();
      // radio.check();  // TODO: add this back in later
      if (buttonControl.hasClickEventOccurred() &&
          buttonControl.getLatestClickEvent() == ClickEvent::CLICK) {
        animationModeController.nextAnimation();
      }
      animationModeController.run();
    }
  }

} masterController;

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

void loop() {
  masterController.loop();
}
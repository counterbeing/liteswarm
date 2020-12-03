#define NUMBER_OF_ANMIATIONS 1

#include "config.h"
#include <Arduino.h>
#define FASTLED_INTERNAL
// #include "./Radio.h"
#include "BaseControllers.h"
#include "DebugLog.h"
#include "FastLED.h"
#include "KnobSettings.h"
#include "MilliTimer.h"
#include "animations/Crossfade.h"
#include <AnimationModeController.h>
#include <Button.h>
#include <Knob.h>
#include <MasterState.h>
#include <SPI.h>

CRGB leds[NUMPIXELS];

Knob * knob{};


MasterState masterState{knob, leds};
Crossfade crossfade(&masterState);

Animation * animations[NUMBER_OF_ANMIATIONS] = {&crossfade};

LSButton button{};
// Radio radio{*masterState};
AnimationModeController animationModeController{button, &masterState};

class OffModeController : public BaseController {
 protected:
  virtual void activate() override {}

  virtual void loop(const bool justActivated) override {
    if (justActivated) {
      fill_solid(leds, NUMPIXELS, CRGB::Black);
      FastLED.show();
    }
  }
};

class MasterController {
 private:
  LSButton & button;
  AnimationModeController animationModeController;
  OffModeController offModeController{};
  // MilliTimer radioTimer{};
  bool offMode = false;
  bool firstLoop = true;

 public:
  MasterController(LSButton & button,
                   AnimationModeController & AnimationModeController)
      : button(button)
      , animationModeController{animationModeController} {}

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
      Serial.println(now);
      startupLoop(now);
      return;
    }

    button.loop();
    button.checkButton();

    bool modeChange = button.hasClickEventOccurred(ClickEvent::LONG_CLICK);
    if (modeChange)
      offMode = !offMode;

    BaseController * activeController =
        offMode ? dynamic_cast<BaseController *>(&offModeController)
                : dynamic_cast<BaseController *>(&animationModeController);

    if (modeChange || firstLoop) {
      firstLoop = false;
      activeController->setAsActive();
    }

    activeController->run();
  }

} masterController{button, animationModeController};

void setup() {
#if STRIP_TYPE == WS2811
  FastLED.addLeds<WS2811, DATAPIN, GRB>(leds, NUMPIXELS);
#elif STRIP_TYPE == SK9822
  FastLED.addLeds<SK9822, DATAPIN, CLOCKPIN, BGR>(leds, NUMPIXELS);
#elif STRIP_TYPE == DOTSTAR
  FastLED.addLeds<DOTSTAR, DATAPIN, CLOCKPIN, BGR>(leds, NUMPIXELS);
#elif STRIP_TYPE == WS2815
  FastLED.addLeds<WS2815, DATAPIN, BGR>(leds, NUMPIXELS);
#else
#  error "Strip type provided is unknown. \
Either add it to main.cpp, or adjust it in config.h."
#endif

  Serial.begin(115200);

  button_debouncer.attach(buttonPin, INPUT_PULLUP);
  button_debouncer.interval(5);
  // radio.setup();
}

void loop() { masterController.loop(); }
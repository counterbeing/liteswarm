#include <Arduino.h>
#include <Bounce2.h>
#include <Encoder.h>
#include "config.h"
#include "DebugLog.h"

#ifndef MyKnob_H
#define MyKnob_H

Bounce button_debouncer = Bounce();

Encoder encoder_knob(2, 3);

enum ClickEvent : uint8_t { CLICK, LONG_CLICK, DOUBLE_CLICK, OTHER };

class MyKnob {
 private:
  static const unsigned int DOUBLE_CLICK_MAX_GAP = 250;  // max ms between clicks for single event
  static const unsigned int SHORT_CLICK_MAX_DURATION = 250;
  static const unsigned int LONG_CLICK_MIN_DURATION = 2750;
  bool possibleDoubleClick = false;
  ClickEvent latestClick = ClickEvent::OTHER;

  int pinA;
  int pinB;
  int position;
  int start = 0;
  int finish = 10;
  int buttonPressCount = 0;
  int loopRotary = false;
  bool manualChange = false;
  bool &offMode;
  int &feedbackPattern;
  void checkRotary() {
    long newPos = encoder_knob.read();
    if (newPos == position) {
      return;
    }
    manualChange = true;
    position = newPos;
  }

  bool cmdMode = false;

  void checkButton(int *_aiIndex) {
    if (button_debouncer.rose()) {
      unsigned long buttonDownDuration = button_debouncer.previousDuration();
      debugLog("button released, buttonDownDuration = ", buttonDownDuration);
      if (buttonDownDuration <= SHORT_CLICK_MAX_DURATION) {
        if (possibleDoubleClick) {
          latestClick = ClickEvent::DOUBLE_CLICK;
          possibleDoubleClick = false;
          // return true;
          if (KNOB_DEBUG)
            debugLog("Event occurred: DOUBLE_CLICK");
          offMode = !offMode;
        }
        else {
          possibleDoubleClick = true;
          // return false;
          if (KNOB_DEBUG)
            debugLog("possible double click");
        }
      }
      else {
        possibleDoubleClick = false;
        if (buttonDownDuration >= LONG_CLICK_MIN_DURATION) {
          latestClick = ClickEvent::LONG_CLICK;
          if (KNOB_DEBUG)
            debugLog("Event occurred: LONG_CLICK");
          // return true;
        }
      }
    }

    else if (!button_debouncer.fell()) {  // no button state change since neither rose nor fell
      if (possibleDoubleClick &&
          button_debouncer.duration() > DOUBLE_CLICK_MAX_GAP) {
        latestClick = ClickEvent::CLICK;
        possibleDoubleClick = false;
        if (KNOB_DEBUG)
          debugLog("Event occurred: CLICK");
        // return true;
        (*_aiIndex)++;
      }
    }

  }

 public:
  MyKnob(uint8_t a, uint8_t b, bool &offMode_, int &feedbackPattern_)
      : offMode(offMode_), feedbackPattern(feedbackPattern_) {
    pinA = a;
    pinB = b;
    // Encoder knob(pinA, pinB);
    // encoder_knob = Encoder(a, b);
  }
  void set(int position) { encoder_knob.write(position); }
  uint32_t get() { return encoder_knob.read(); }
  void check(int *_animationIndex) {
    manualChange = false;
    checkButton(_animationIndex);
    checkRotary();
  }
  bool manuallyChanged() { return manualChange; }

  // Set these variables once so they don't need to be set repeatedly
  void setDefaults(int position_, int start_, int finish_,
                   bool loopRotary_ = false) {
    position = position_;
    encoder_knob.write(position);
    start = start_;
    finish = finish_;
    loopRotary = loopRotary_;
  }

  // Sets the value for the rotary encoder to someething reasonable for the
  // animation. It returns that value.
  int confine() {
    if (position < start) {
      position = loopRotary ? finish : start;
      encoder_knob.write(position);
    }
    if (position > finish) {
      position = loopRotary ? start : finish;
      encoder_knob.write(position);
    }
    return position;
  }
};

#endif

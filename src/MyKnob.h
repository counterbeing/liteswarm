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
  int32_t position;
  bool manualChange = false;

 public:
  MyKnob() {}
  void set(int position) { encoder_knob.write(position); }
  uint32_t get() { return encoder_knob.read(); }
  bool manuallyChanged() { return manualChange; }
};

class ButtonControl {
 private:
  static const unsigned int DOUBLE_CLICK_MAX_GAP = 250;  // max ms between clicks for single event
  static const unsigned int SHORT_CLICK_MAX_DURATION = 250;
  static const unsigned int LONG_CLICK_MIN_DURATION = 1750;
  bool possibleDoubleClick = false;
  ClickEvent latestClick = ClickEvent::OTHER;
  bool clickEventOccurredFlag = false;
 public:

  bool hasClickEventOccurred() { return clickEventOccurredFlag; }

  void checkButton() {
    clickEventOccurredFlag = false;

    if (button_debouncer.rose()) {
      unsigned long buttonDownDuration = button_debouncer.previousDuration();
      // if (KNOB_DEBUG)
        // debugLog("button up, down duration = ", buttonDownDuration);
      if (buttonDownDuration <= SHORT_CLICK_MAX_DURATION) {
        if (possibleDoubleClick) {
          latestClick = ClickEvent::DOUBLE_CLICK;
          possibleDoubleClick = false;
          if (KNOB_DEBUG)
            debugLog("Event occurred: DOUBLE_CLICK");
          clickEventOccurredFlag = true;
        }
        else {
          possibleDoubleClick = true;
        }
      }
      else {
        possibleDoubleClick = false;
        if (buttonDownDuration >= LONG_CLICK_MIN_DURATION) {
          latestClick = ClickEvent::LONG_CLICK;
          if (KNOB_DEBUG)
            debugLog("Event occurred: LONG_CLICK");
          clickEventOccurredFlag = true;
        }
      }
    }

    else if (!button_debouncer.fell()) {  // no button state change since neither rose nor fell
      if (possibleDoubleClick && button_debouncer.duration() > DOUBLE_CLICK_MAX_GAP) {
        latestClick = ClickEvent::CLICK;
        possibleDoubleClick = false;
        if (KNOB_DEBUG)
          debugLog("Event occurred: CLICK");
        clickEventOccurredFlag = true;
      }
    }

  }

  ClickEvent getLatestClickEvent() {
    return latestClick;
  }
};

class KnobControl {
 private:
  int32_t minValue;
  int32_t maxValue;
  bool loopRotary;

 public:
  KnobControl(int32_t minValue_, int32_t maxValue_, bool loopRotary_)
      : minValue(minValue_), maxValue(maxValue_), loopRotary(loopRotary_) {}

  void setPosition(int32_t position) {
    if (KNOB_DEBUG)
      debugLog("KnobControl::setPosition to ", position);
    encoder_knob.write(position);
  }

  int32_t getPosition() {
    int32_t position = encoder_knob.read();
    if (position < minValue) {
      debugLog("KnobControl::position below min, position=", position);
      position = loopRotary ? maxValue : minValue;
      debugLog("KnobControl::position below min, now ", position);
      encoder_knob.write(position);
    }
    else if (position > maxValue) {
      position = loopRotary ? minValue : maxValue;
      debugLog("KnobControl::getPosition() above max, set to ", position);
      encoder_knob.write(position);
    }
    return position;
  }

  bool updateSettingOnChange(int32_t &setting) {
    int32_t newSetting = getPosition();
    if (newSetting != setting) {
      setting = newSetting;
      return true;
    }
    return false;
  }
};

#endif

#include "DebugLog.h"
#include "config.h"
#include <Arduino.h>
#include <Bounce2.h>
#include <Encoder.h>

#ifndef MyKnob_H
#define MyKnob_H

Bounce button_debouncer = Bounce();

Encoder encoder_knob(2, 3);

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

enum ClickEvent : uint8_t { CLICK, LONG_CLICK, DOUBLE_CLICK, OTHER };

class ButtonControl {
 private:
  static const unsigned int DOUBLE_CLICK_MAX_GAP =
      250; // max ms between clicks for single event
  static const unsigned int SHORT_CLICK_MAX_DURATION = 250;
  static const unsigned int LONG_CLICK_MIN_DURATION = 1750;
  bool possibleDoubleClick = false;
  bool possibleLongClick = false;
  ClickEvent latestClickEvent = ClickEvent::OTHER;
  bool clickEventOccurredFlag = false;

  void setLatestClickEvent(ClickEvent clickEvent) {
    latestClickEvent = clickEvent;
    clickEventOccurredFlag = true;
    if (KNOB_DEBUG) {
      switch (clickEvent) {
        case ClickEvent::CLICK:
          debugLog("Event occurred: CLICK");
          break;
        case ClickEvent::LONG_CLICK:
          debugLog("Event occurred: LONG_CLICK");
          break;
        case ClickEvent::DOUBLE_CLICK:
          debugLog("Event occurred: DOUBLE_CLICK");
          break;
        default:
          debugLog("Event occurred: OTHER");
          break;
      }
    }
  }

 public:
  ClickEvent getLatestClickEvent() { return latestClickEvent; }

  bool hasClickEventOccurred() { return clickEventOccurredFlag; }

  bool hasClickEventOccurred(ClickEvent clickEvent) {
    return clickEventOccurredFlag && clickEvent == latestClickEvent;
  }

  void checkButton() {
    clickEventOccurredFlag = false;

    if (button_debouncer.rose()) {
      unsigned long buttonDownDuration = button_debouncer.previousDuration();
      if (KNOB_DEBUG)
        debugLog("button up, down duration = ", buttonDownDuration);

      possibleLongClick = false;

      if (buttonDownDuration <= SHORT_CLICK_MAX_DURATION) {
        if (possibleDoubleClick) {
          possibleDoubleClick = false;
          setLatestClickEvent(ClickEvent::DOUBLE_CLICK);
          return;
        }

        possibleDoubleClick = true;
      }

      else {
        possibleDoubleClick = false;
      }

    }

    else if (button_debouncer.fell()) {
      if (KNOB_DEBUG)
        debugLog("button down, up duration = ",
                 button_debouncer.previousDuration());
      possibleLongClick = true;
    }

    else { // no button state change since neither rose nor fell
      if (possibleLongClick &&
          button_debouncer.duration() >= LONG_CLICK_MIN_DURATION) {
        setLatestClickEvent(ClickEvent::LONG_CLICK);
        possibleLongClick = false;
        return;
      }

      if (possibleDoubleClick &&
          button_debouncer.duration() > DOUBLE_CLICK_MAX_GAP) {
        setLatestClickEvent(ClickEvent::CLICK);
        possibleDoubleClick = false;
        return;
      }
    }
  }
};

class KnobSetting {
 private:
  int32_t currentValue;
  int32_t minValue;
  int32_t maxValue;
  bool loopRotary;

 public:
  KnobSetting(int32_t initialValue, int32_t minValue_, int32_t maxValue_,
              bool loopRotary_)
      : currentValue(initialValue), minValue(minValue_), maxValue(maxValue_),
        loopRotary(loopRotary_) {}

  int32_t get() { return currentValue; }

  int32_t getCorrectedValue() {
    int32_t position = encoder_knob.read();
    if (position < minValue) {
      position = loopRotary ? maxValue : minValue;
      encoder_knob.write(position);
    } else if (position > maxValue) {
      position = loopRotary ? minValue : maxValue;
      encoder_knob.write(position);
    }
    return position;
  }

  bool update() {
    int32_t newValue = getCorrectedValue();
    if (newValue != currentValue) {
      currentValue = newValue;
      return true;
    }
    return false;
  }

  void activate() { encoder_knob.write(currentValue); }
};

#endif

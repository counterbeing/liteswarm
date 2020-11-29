#include "DebugLog.h"
#include "config.h"
#include <Bounce2.h>
Bounce button_debouncer = Bounce();

enum ClickEvent : uint8_t { CLICK, LONG_CLICK, DOUBLE_CLICK, OTHER };

class Button {
 private:
  static const unsigned int DOUBLE_CLICK_MAX_GAP =
      250; // max ms between clicks for single event
  static const unsigned int SHORT_CLICK_MAX_DURATION = 250;
  static const unsigned int LONG_CLICK_MIN_DURATION = 1750;
  ClickEvent latestClickEvent = ClickEvent::OTHER;
  bool possibleDoubleClick = false;
  bool possibleLongClick = false;
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

  bool hasClickEventOccurred(const ClickEvent clickEvent) {
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

  void setup() {
    button_debouncer.attach(buttonPin, INPUT_PULLUP);
    button_debouncer.interval(5);
  }

  void loop() { button_debouncer.update(); }
};
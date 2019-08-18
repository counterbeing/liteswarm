#include <Arduino.h>
#include <Encoder.h>
#include <Bounce2.h>
#include "config.h"

// #include "store.h"

#ifndef MyKnob_H
#define MyKnob_H


Bounce button_debouncer = Bounce();

Encoder encoder_knob(2, 3);

class MyKnob
{
private:
    // TODO: want to be able to parameterize the pins passed to Encoder when we
    // create a new MyKnob
    // Encoder encoder_knob;
    // Encoder encoder_knob(2,3);

    /////////////////////////////////////////////////
    // button check logic trace
    //
    // button not being pressed: 
    //   lastButtonState = 1, lastPressTime = 000400
    //   buttonState     = 1, holdingSince  = 0
    //
    // button being pressed, first frame of loop (say millis() returns 10000)
    //   lastButtonState = 1, lastPressTime = 000400
    //   buttonState     = 0, holdingSince  = 010000
    //
    // button being pressed, second frame of loop
    //   lastButtonState = 0, lastPressTime = 000400
    //   buttonState     = 0, holdingSince  = 010000

    // button released, nth frame of loop; enter main if() block
    //   lastButtonState = 0, lastPressTime = 000400
    //   buttonState     = 1, holdingSince  = 010000
    //                     currentPressTime = 010399 | 011001
    //                             holdTime = 399    | 1001
    //                           difference = 009999 | 010601
    // short press...
    //                        lastPressTime = 010399
    // long press...
    //                        lastPressTime = 011001
    //      holdingSince = 0
    //   lastButtonState = 1

    int pinA;
    int pinB;
    int position;
    int start = 0;
    int finish = 10;
    int buttonPressCount = 0;
    int loopRotary = false;
    long lastPressTime = 0;
    long holdingSince = 0;
    bool manualChange = false;
    int lastButtonState = 1;
    bool &offMode;
    void checkRotary()
    {
        long newPos = encoder_knob.read();
        if (newPos == position)
        {
            return;
        }
        manualChange = true;
        position = newPos;
    }
    
    // #define KNOBDEBUG true     // set in config.h
    #define COMBO_MAX_ITEMS 5 // recognize combo sequences of up to 5 presses
    const unsigned int shortPress = 200;
    const unsigned int mediumPress = 500;
    const unsigned int longPress = 1000;
    const unsigned int sleepPress = 2200;
    const unsigned long comboInterval = 1200;
    int comboLength = 0;  // index for comboPattern array
    char comboPattern[COMBO_MAX_ITEMS]; // combos can be 2-4 presses in sequence
    bool cmdMode = false;
    bool lastPressWasShort  = false;
    bool lastPressWasMedium = false;
    bool lastPressWasLong   = false;
    // enum lastPressWas { SHORT MEDIUM LONG EXTRALONG }
    // bool theCorrectPressCouldActivateCmdMode = false; // unused


    // DONE wrap press duration logic in descriptive variables
    // DONE swap order of cmdMode pattern from short-med to med-short

    // TODO animation fx return FastLed[], then main fx optionally combines
    //   overlay led[] before writing to strip

    // TODO add visual feedback (use amimationCombinator w/ strobe)

    // TODO lowpower mode triggered by 3000 ms long press

    void checkButton(int *_aiIndex)
    {
        int buttonState = button_debouncer.read();
        if (lastButtonState == 1 && buttonState == 0) holdingSince = millis();  // started press

        // if (millis() - holdingSince > sleepPress && lastButtonState == 0 && buttonState == 0) {
        //   offMode = !offMode;
        //   lastButtonState = 1;  // virtual relase of button
        //   holdingSince = 0;
        //   Serial.print("\n\n ====== intercepted VERY LONG PRESS! fliping offMode...  ======");
        
        // } else if (lastButtonState == 0 && buttonState == 1) {                         // after first frame of press interval lastButtonState == 0
        
        if (lastButtonState == 0 && buttonState == 1) {                         // after first frame of press interval lastButtonState == 0
          // A press or hold was finished
          unsigned long currentPressTime = millis();
          unsigned long holdTime         = currentPressTime - holdingSince;
          unsigned long difference       = currentPressTime - lastPressTime - holdTime;  // compensate for duration of push interval

          bool thisPressIsShort = holdTime < shortPress;
          bool thisPressIsMedium = holdTime > shortPress && holdTime < longPress;
          bool thisPressIsLong = holdTime >= longPress && holdTime < sleepPress;
          bool thisPressIsVeryLong = holdTime >= sleepPress;
                    
          bool firstPressInCmdMode = cmdMode && comboLength == 0;
          // difference < comboInterval implies previous press just happened
          bool stillTimeToCombo = difference < comboInterval;

          // useful only for combo detection - based on indirect timing comparison
          //   BUG can't use difference for first press in combo since it could be really large
          //   and therefore always indicate lastPressWasLong
          // 
          // bool lastPressWasShort  = difference < shortPress;
          // bool lastPressWasMedium = difference < longPress && difference > shortPress;
          // bool lastPressWasLong   = difference >= longPress;
          
          if (KNOBDEBUG) {
            Serial.print("\ncmdMode: ");
            Serial.print(cmdMode ? "TRUE" : "FALSE");
            Serial.print("\tcomboLength: ");
            Serial.print(comboLength);
            
            Serial.print("\n\tthisPressIsShort: ");
            Serial.print(thisPressIsShort);
            Serial.print("\tlastPressWasShort: ");
            Serial.print(lastPressWasShort);
            Serial.print("\n\tthisPressIsMedium: ");
            Serial.print(thisPressIsMedium);
            Serial.print("\tlastPressWasMedium: ");
            Serial.print(lastPressWasMedium);
            Serial.print("\n\tthisPressIsLong: ");
            Serial.print(thisPressIsLong);
            Serial.print("\tlastPressWasLong: ");
            Serial.print(lastPressWasLong);

            Serial.print("\n\tfirstPressInCmdMode: ");
            Serial.print(firstPressInCmdMode);
            Serial.print("\tstillTimeToCombo: ");
            Serial.print(stillTimeToCombo);

            Serial.print("\noffMode: ");
            Serial.print(offMode);
            // Serial.print("\t*offMode: ");
            // Serial.print(*offMode);            
          }

          // momentary press typically takes 90-130ms, but possible to get as fast as 30ms
          if (thisPressIsShort) {
            if (firstPressInCmdMode || (cmdMode && stillTimeToCombo)) addToCombo('.');

            if (!cmdMode && lastPressWasMedium) {
              if (stillTimeToCombo) {
                cmdMode = true;
                if (KNOBDEBUG) { 
                  Serial.print("\nCOMBOed! (med->short) CMDMODE now ");
                  Serial.print(cmdMode ? "TRUE" : "FALSE");
                }
              }
            } else if (!cmdMode){
              manualChange = true;
              (*_aiIndex)++;;  
              // TODO buffer rapid presses - see readme 
            }
            lastPressWasShort = true;
            lastPressWasMedium = false;
            lastPressWasLong = false;
          
          } else if (thisPressIsMedium) {
            if (firstPressInCmdMode || (cmdMode && stillTimeToCombo)) addToCombo('-');
            lastPressWasShort = false;
            lastPressWasMedium = true;
            lastPressWasLong = false;
              
          } else if (thisPressIsLong) {
            cmdMode = false;
            Serial.print("\nexiting CMDMODE... ");
            lastPressWasShort = false;
            lastPressWasMedium = false;
            lastPressWasLong = true;
          
          } else if (thisPressIsVeryLong) {
            Serial.print("\nVERY LONG PRESS! sleepytime ");
            cmdMode = false;
            resetCombo();
            // addToCombo('s');
            
            offMode = !offMode; // now handled in press-down logic
            
            lastPressWasShort = false;
            lastPressWasMedium = false;
            lastPressWasLong = false;
          }

          // finished button processing!
          lastPressTime = currentPressTime;
          holdingSince = 0;
          
          if (KNOBDEBUG) {
            Serial.print("\nheld: ");
            Serial.print(holdTime);
            Serial.print(" ms\tdifference: ");
            Serial.print(difference);
            Serial.print(" ms\n");
            Serial.print("combopattern:\t");
            Serial.print(comboPattern);
            Serial.print("\n-------------------------------------");
          }
        };
        lastButtonState = buttonState;

        // dispatch & reset combo
        bool tooManyCombos      = comboLength > COMBO_MAX_ITEMS - 1;
        bool stillTryingToCombo = comboLength > 0 && lastPressTime > 0;
        bool butItsTooLate      = millis() - lastPressTime > comboInterval;
        
        if (tooManyCombos || (stillTryingToCombo && butItsTooLate)) {
            dispatchAndResetCombo();
        }
    }

    void resetCombo() {
        if (KNOBDEBUG) Serial.println("\n================= resetting combopattern =================");
        comboLength = 0;
        // memset(comboPattern, 0, sizeof(comboPattern));
        // another way to clear it
        for (int i = 0; i < COMBO_MAX_ITEMS - 1; i++) {
            comboPattern[i] = 0;
        }
        // cmdMode = false; // exits too quickly
    }

    void addToCombo(char comboChar) {
        // uncomment to prove that weird ascii in serial console
        // is due to c_string requiring special terminal char
        //
        // if(comboLength == COMBO_MAX_ITEMS - 1) comboChar = '\0';
        
        comboPattern[comboLength] = comboChar;
        comboLength++;
    }

    void dispatchAndResetCombo() {
        dispatch();
        resetCombo();
    }

    void dispatch() {
        Serial.print("\nMyKnob::dispatch() getting cmd for combo");
        switch (getCaseForCombo())
        {
        case 0:
            Serial.print("\ncase 0: calling lowPowerMode()");
            break;
        case 1:
            Serial.print("\ncase 1: calling strobeMode()");
            break;
        case 2:
            Serial.print("\ncase 2: calling debugMode()");
            break;
        case 3:
            Serial.print("\ncase 3: calling sneakyStrobeMode()");
            break;
        case 4:
            Serial.print("\ncase 4: calling TBD() ");
            break;
        default:
            Serial.println("\n\nWARN: default switch case; couldn't find fx for pattern ");
            break;
        }
    }

    /////////////////////////////////////////////////////////
    // command functions & str lookup
    const char *patternList[4] = {
      {"s"},      // case 0: lowPowerMode
      {"--"},     // case 1: strobeMode
      {"-.-"},    // case 2: debugMode
      {"--.."}    // case 3: sneakyStrobeMode
      // {"--.."},// case 4: TBD
      // {"---"}, // case 5: TBD
      // {"..--"} // case 6: TBD
    };

    void lowPowerMode() {
      // if(_lowPowerMode) {
      //   Serial.println("toggling lowPowerMode OFF");
      // } else {
      //   Serial.println("toggling lowPowerMode ON");
      // }
      // _lowPowerMode = !lowPowerMode;
    }
    void strobeMode() {
      Serial.println("entering strobeMode()");
    }
    void debugMode() {
      Serial.println("entering debugMode()");
    }
    void sneakyStrobeMode() {
      Serial.println("entering sneakyStrobeMode()");
    }
    /////////////////////////////////////////////////////////

    int getCaseForCombo() {
        int patternMatch = -1;
        for (int patternIdx = 0; patternIdx < 3; patternIdx++) 
        {
            const char *candidate = patternList[patternIdx];
            patternMatch = patternIdx;
            Serial.print("\ncandidate: ");
            Serial.print(candidate);
            for (int i = 0; i < COMBO_MAX_ITEMS - 1; i++)
            {
                if (comboPattern[i] != candidate[i]) patternMatch = -1;
            }

            if (patternMatch > -1) {
                if (KNOBDEBUG) { 
                    Serial.print("\n found it! return case #");
                    Serial.print(patternMatch);
                }
                return patternMatch;
            } else {
                // if (KNOBDEBUG) Serial.print("\n couldnt find it - returning case #-1");
            }
        }
        return patternMatch;
    }


    /////////////////////////////////////////////////////////
    // hashmap implementation instead of switch statement (allows string->fx() mapping)
    //
    // typedef void (*funcPointer)();
    // unordered_map<string, funcPointer> modeMap = {
    //     {"....", &strobeMode},
    //     {".-.-", &debugMode},
    //     {"---",  &lowPowerMode},
    //     {"..--", &sneakyStrobeMode}
    // };

    // void dispatch(string pat){
    //     unordered_map<string, funcPointer>::const_iterator got = modeMap.find (pat);
    //     if( got == stringFunctionMap.end() ) {
    //         cout << "Error: patternDispatcher() unable to find pattern " << pat << " in stringFunctionMap\n";
    //     } else {
    //         stringFunctionMap[pat]();
    //     }
    // }
    /////////////////////////////////////////////////////////

public:
    MyKnob(uint8_t a, uint8_t b, bool &offMode_) : offMode(offMode_)
    {
        pinA = a;
        pinB = b;
        // Encoder knob(pinA, pinB);
        // encoder_knob = Encoder(a, b);
        Serial.print("\n\noffMode_: ");
        Serial.print(offMode);
        
    }
    void set(int position)
    {
        encoder_knob.write(position);
    }
    uint32_t get()
    {
        return encoder_knob.read();
    }
    void check(int *_animationIndex)
    {
        manualChange = false;
        checkButton(_animationIndex);
        checkRotary();
    }
    bool manuallyChanged()
    {
        return manualChange;
    }

    // Set these variables once so they don't need to be set repeatedly
    void setDefaults(int position_, int start_, int finish_, bool loopRotary_ = false)
    {
        position = position_;
        encoder_knob.write(position);
        start = start_;
        finish = finish_;
        loopRotary = loopRotary_;
    }

    // Sets the value for the rotary encoder to someething reasonable for the
    // animation. It returns that value.
    int confine()
    {
        if (position < start)
        {
            if (loopRotary)
            {
                position = finish;
            }
            else
            {
                position = start;
            }
            encoder_knob.write(position);
        }
        if (position > finish)
        {
            if (loopRotary)
            {
                position = start;
            }
            else
            {
                position = finish;
            }
            encoder_knob.write(position);
        }
        return position;
    }
};

#endif

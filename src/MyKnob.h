#include <Arduino.h>
#include <Encoder.h>
#include <Bounce2.h>

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
    
    #define DEBUGLOG true     // flag to enable/disable serial logging
    #define COMBO_MAX_ITEMS 5 // recognize combo sequences of up to 5 presses
    const unsigned int shortPress = 200;
    const unsigned int mediumPress = 500;
    const unsigned int longPress = 1000;
    const unsigned long comboInterval = 1200;
    int comboLength = 0;  // index for comboPattern array
    char comboPattern[COMBO_MAX_ITEMS]; // combos can be 2-4 presses in sequence
    bool cmdMode = false;


    void checkButton(int *_aiIndex)
    {
        int buttonState = button_debouncer.read();
        if (lastButtonState == 1 && buttonState == 0) holdingSince = millis();  // started press
        if (lastButtonState == 0 && buttonState == 1) {  // after first frame of press interval lastButtonState == 0
            // A press or hold was finished
            unsigned long currentPressTime = millis();
            unsigned long holdTime = currentPressTime - holdingSince;
            unsigned long difference = currentPressTime - lastPressTime - holdTime;  // compensate for duration of push interval

            if (DEBUGLOG) {
                Serial.print("\ncmdMode: ");
                Serial.print(cmdMode ? "TRUE" : "FALSE");
                Serial.print("\tcomboLength: ");
                Serial.print(comboLength);
            }

            // TODO swap order of cmdMode pattern from short-med to med-short
            
            // TODO animation fx return FastLed[], then main fx optionally combines
            // overlay led[] before writing to strip

            // TODO add visual feedback (use amimationCombinator w/ strobe)

            // TODO lowpower mode triggered by 3000 ms long press

            // short press
            // momentary press typically takes 90-130ms, but possible to get as fast as 30ms
            // if (difference > 400 && holdTime < 400)  // no spamming the button so we don't spam the swarm?
            if (holdTime < shortPress)  {
                // is it the first combo press in cmdMode? only check difference if its a subsequent combo press
                if(cmdMode && (comboLength == 0 || difference < comboInterval)){
                    addToCombo('.');
                    // comboPattern[comboLength] = '.';
                    // comboLength++;
                }
                if(!cmdMode && difference > shortPress) {
                    manualChange = true;
                    (*_aiIndex)++;
                } // TODO buffer rapid presses 
                  //   IF rapid presses (where difference < shortPress):
                  //     skip (*_aiIndex)++;
                  //     instead increment rapidPressCount++, once per rapid press
                  //   IF rapid presses over (buttonState == 1 && rapidPressCount > 0)
                  //     (*_aiIndex) += rapidPressCount;
                  //     rapidPressCount == 0;
                
                lastPressTime = currentPressTime;  // records when the button was RELEASED
            }


            // medium press
            if (holdTime > shortPress && holdTime < longPress) {
                // enter cmdMode via special short-long press combo: 
                // difference < comboInterval implies short press just happened
                if(!cmdMode && difference < comboInterval) {
                    cmdMode = true;
                    Serial.print("\nCOMBOed! CMDMODE now ");
                    Serial.print(cmdMode ? "TRUE" : "FALSE");
                // don't check difference if this is the first combo press in cmdMode
                } else if (cmdMode && (comboLength == 0 || difference < comboInterval)) {
                    // comboPattern[comboLength] = '-';
                    // comboLength++;
                    addToCombo('-');
                }
                lastPressTime = currentPressTime;
            }


            // long press - how we exit cmdMode
            if (holdTime > 1000) {
                cmdMode = false;
                Serial.print("\nexiting CMDMODE... ");
            }


            // finished button processing!
            holdingSince = 0;
            
            if (DEBUGLOG) {
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
        if (DEBUGLOG) Serial.println("\n================= resetting combopattern =================");
        comboLength = 0;
        // memset(comboPattern, 0, sizeof(comboPattern));
        // another way to clear it
        for (int i = 0; i < COMBO_MAX_ITEMS - 1; i++) {
            comboPattern[i] = 0;
        }
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
        {"...."},   // case 0: lowPowerMode
        {".-.-"},   // case 1: strobeMode
        {"---"},    // case 2: debugMode
        {"..--"}    // case 3: sneakyStrobeMode
        // {"--.."},   // case 4: TBD
        // {"---"},    // case 5: TBD
        // {"..--"}    // case 6: TBD
    };

    void lowPowerMode(){
        Serial.println("entering lowPowerMode()");
    }
    void strobeMode(){
        Serial.println("entering strobeMode()");
    }
    void debugMode() {
        Serial.println("entering debugMode()");
    }
    void sneakyStrobeMode(){
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
                if (DEBUGLOG) { 
                    Serial.print("\n found it! return case #");
                    Serial.print(patternMatch);
                }
                return patternMatch;
            } else {
                // if (DEBUGLOG) Serial.print("\n couldnt find it - returning case #-1");
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

public:
    MyKnob(uint8_t a, uint8_t b, bool &offMode_) : offMode(offMode_)
    {
        pinA = a;
        pinB = b;
        // Encoder knob(pinA, pinB);
        // encoder_knob = Encoder(a, b);
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

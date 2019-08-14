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

    int shortPress = 300;
    int mediumPress = 500;
    int longPress = 1000;
    int comboInterval = 600;
    char comboPattern[4]; // combos can be 2-4 presses in sequence
    int comboLength = 0;  // index for comboPattern array
    bool cmdMode = false;

    void checkButton(int *_aiIndex)
    {
        int buttonState = button_debouncer.read();
        if (lastButtonState == 1 && buttonState == 0) holdingSince = millis();  // started press
        if(lastButtonState == 0 && buttonState == 1) {  // after first frame of press interval lastButtonState == 0
            // A press or hold was finished
            long currentPressTime = millis();
            // long holdTime = millis() - holdingSince; // save a call to millis()?
            long holdTime = currentPressTime - holdingSince;
            // long difference = currentPressTime - lastPressTime;  
            long difference = currentPressTime - lastPressTime - holdTime;  // compensate for duration of push interval

            // reset combo accumulator if too long between presses
            if ( difference > comboInterval || comboLength >= 3)
            {
                Serial.println("\n================= resetting combopattern =================");
                comboLength = 0;
                // memset(comboPattern, 0, sizeof(comboPattern));
                // Serial.println(comboPattern);
                // Serial.println("=============================\n");
                // another way to clear it
                for (int i = 0; i < 4; ++i)
                {
                    comboPattern[i] = 0;
                }
            }

            Serial.print("\ncmdMode: ");
            Serial.print(cmdMode);
            Serial.print("\tcomboLength: ");
            Serial.println(comboLength);
            Serial.print("combopattern: ");
            Serial.println(comboPattern);

            // short press
            // momentary press typically takes 110ms +- 20ms, but possible to get as fast as 30ms
            // if (difference > 400 && holdTime < 400)  // no spamming the button so we don't spam the swarm?
            if (holdTime < shortPress) 
            {
                // first press in cmdMode so don't check difference, or its a subsequent combo press
                if(cmdMode && (comboInterval == 0 || difference < comboInterval)){
                    comboPattern[comboLength] = '.';
                    comboLength++;
                }
                // subsequent presses in cmdMode
                // if(cmdMode && difference < comboInterval) {
                //     comboPattern[comboLength] = '.';
                //     comboLength++;
                // }

                // if(offMode) {
                //     offMode = false;
                //     Serial.println("exiting OFFMODE");
                //     return;
                // }

                // lastPressTime = millis(); // save a call to millis()?
                lastPressTime = currentPressTime;  // records when the button was RELEASED
                if(!cmdMode && difference > shortPress) {
                    manualChange = true;
                    (*_aiIndex)++;
                } // TODO buffer rapid presses 
                  //   IF rapdid presses:
                  //   count sequence ("seq") of rapid presses where difference > shortPress
                  //   don't 
            }
            // combo detection - medium press following short press, like this: ".-"
            // difference < comboInterval implies short press just happened
            if (holdTime > shortPress && holdTime < longPress && difference < comboInterval) {
                if(!cmdMode) {
                    cmdMode = true;
                    Serial.print("\nCOMBOed into CMDMODE ");
                } else {
                    comboPattern[comboLength] = '-';
                    comboLength ++;
                }
            }

            if (holdTime > 1000) {
                // offMode = true;
                cmdMode = false;
                Serial.print("\nexiting CMDMODE... ");
                // lastPressTime = currentPressTime;
                // Serial.println("entering OFFMODE; button was held (ms) ");
                // Serial.print(holdTime);
            }
            holdingSince = 0;
            Serial.print("\nheld: ");
            Serial.print(holdTime);
            Serial.print(" ms\tdifference: ");
            Serial.print(difference);
            Serial.print(" ms\n");

            Serial.print("combopattern: ");
            Serial.println(comboPattern);
        };
        lastButtonState = buttonState;
    }

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

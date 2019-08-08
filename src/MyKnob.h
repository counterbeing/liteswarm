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

    void checkButton(int *_aiIndex)
    {
        int buttonState = button_debouncer.read();
        if (lastButtonState == 1 && buttonState == 0) holdingSince = millis();
        if(lastButtonState == 0 && buttonState == 1) {
            // A press or hold was finished
            long currentPressTime = millis();
            long difference = currentPressTime - lastPressTime;
            long holdTime = millis() - holdingSince;
            if (difference > 400 && holdTime < 400)
            {
                if(offMode) {
                    offMode = false;
                    return;
                }
                lastPressTime = millis();
                manualChange = true;
                (*_aiIndex)++;
            }
            if (holdTime > 2000) {
                offMode = true;
                Serial.println(holdTime);
            }
            holdingSince = 0;
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

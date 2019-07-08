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
    bool manualChange = false;
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
        if (buttonState != 0)
            return;
        long currentPressTime = millis();
        long difference = currentPressTime - lastPressTime;
        if (difference > 400)
        {
            // buttonPressCount++;
            // Serial.print("Button press count: ");
            // Serial.println(buttonPressCount);
            // Serial.println(currentPressTime);
            // Serial.println("-----------------");
            // Serial.println(difference);
            // // Serial.println(&_aiIndex);
            // Serial.print("value: ");
            // Serial.println(reinterpret_cast<int>(*_aiIndex));
            // Serial.print("addr: ");
            // Serial.println(reinterpret_cast<int>(_aiIndex));
            lastPressTime = millis();
            manualChange = true;
            (*_aiIndex)++;
            // Serial.print("new *_aiIndex: ");
            // Serial.println(reinterpret_cast<int>(*_aiIndex));
        }
    }

public:
    MyKnob(uint8_t a, uint8_t b)
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

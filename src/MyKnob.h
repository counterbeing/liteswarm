#include <Arduino.h>
#include <Encoder.h>
#include <Bounce2.h>

Bounce button_debouncer = Bounce();

#ifndef MyKnob_H
#define MyKnob_H

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
    int position = 5;
    int start = 0;
    int finish = 10;
    int buttonPressCount = 0;
    long lastPressTime = 0;
    void checkRotary()
    {
        long newPos = encoder_knob.read();
        if (newPos != position)
        {
            position = newPos;
            // Serial.println(newPos);
        }
    }

    void checkButton(int *_aiIndex)
    {
        int buttonState = button_debouncer.read();
        if(buttonState != 0) return;
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
    void check(int *_animationIndex) {
        checkButton(_animationIndex);
        checkRotary();
    }
    // Sets the value for the rotary encoder to someething reasonable for the
    // animation. It returns that value.
    int confine(int start, int finish)
    {
        if (position < start)
        {
            position = start;
            encoder_knob.write(start);
        }
        if (position > finish)
        {
            position = finish;
            encoder_knob.write(finish);
        }
        return position;
    }
};

#endif


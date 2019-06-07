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
    // class knob;
    // int knob;
    // Encoder knob;
    // Encoder knob(2,3);

    int pinA;
    int pinB;
    int position;
    int start;
    int finish;
    int buttonPressCount = 0;
    long lastPressTime = 0;
    void checkRotary(int loopTime)
    {
        long newPos = encoder_knob.read();
        if (newPos != position)
        {
            position = newPos;
            /* Serial.println(newPos); */
        }
    }

    void checkButton(int &animationIndex)
    {
        int buttonState = button_debouncer.read();
        long currentPressTime = millis();
        long difference = currentPressTime - lastPressTime;
        if ((buttonState == 0) && (difference > 400))
        {
            buttonPressCount++;
            /* Serial.print("Button press count: "); */
            /* Serial.println(buttonPressCount); */
            /* Serial.println("BUTTON PRESSED CALLING SWITCH"); */
            /* Serial.println(lastPressTime); */
            /* Serial.println(currentPressTime); */
            lastPressTime = millis();
            animationIndex++;
        }
    }

public:
    MyKnob(uint8_t a, uint8_t b)
    {
        pinA = a;
        pinB = b;
        // Encoder knob(pinA, pinB);
        // knob = Encoder(a, b);
    }
    void check(unsigned long m, int &animationIndex) {
        checkButton(animationIndex);
        checkRotary(m);
    }
    int getPosition()
    {
        return position;
    }
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


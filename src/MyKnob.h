#include "config.h"
// #include <Arduino.h>
// #include <Encoder.h>
#include <ESP32Encoder.h>
#include <Bounce2.h>

#ifndef MyKnob_H
#define MyKnob_H

// Bounce button_debouncer = Bounce();
Bounce2::Button button_debouncer = Bounce2::Button();

// Encoder encoder_knob(rotray1, rotary2);
ESP32Encoder encoder_knob;

class MyKnob
{
private:
    // TODO: want to be able to parameterize the pins passed to Encoder when we
    // create a new MyKnob
    // Encoder encoder_knob;
    // Encoder encoder_knob(2,3);

    // int position;
    // uint32_t position = 0;
    // uint32_t newPos = 0;
    int position = 0;
    int newPos = 0;
    int start = 0;
    int finish = 10;
    int buttonPressCount = 0;
    int loopRotary = false;
    long lastPressTime = 0;
    void checkRotary(bool *_localUpdate)
    {
        // long newPos = (uint32_t)encoder_knob.getCount();
        // newPos = (uint32_t)encoder_knob.getCount();
        newPos = (int)encoder_knob.getCount();
        if (newPos == position)
        {
            return;
        }
        position = newPos;
        (*_localUpdate) = true;
        Serial.println("\n(localupdate) Encoder count = " + String(position) + "\n");
    }

    void checkButton(int *_aiIndex, bool *_localUpdate)
    {
      if ( button_debouncer.pressed() ) {
        Serial.println("\n####### BUTTON PRESS\n");
        (*_aiIndex)++;
        (*_localUpdate) = true;
      }
    }

public:
    MyKnob(){}
    void setup(){
      
      // Rotary Encoder Knob
      // https://github.com/madhephaestus/ESP32Encoder/blob/master/examples/Encoder/Encoder.ino
      ESP32Encoder::useInternalWeakPullResistors=UP;
      encoder_knob.attachHalfQuad(17, 16);
      // clear the encoder's raw count and set the tracked count to zero
      encoder_knob.clearCount();
      // encoder_knob.setCount(128); // init in middle 0-255
      Serial.println("Encoder Start = " + String((uint32_t)encoder_knob.getCount()));

      button_debouncer.attach(21, INPUT_PULLUP);
      button_debouncer.interval(25);
    }
    void set(int _position)
    {
        // very important - set used by incoming packet to update state
        // by setting knob AND class variable to incoming value, short-
        // circuits localUpdate logic (which leads to constant back-forth radio updates)
        encoder_knob.setCount(_position);
        position = _position;
    }
    // uint32_t get()
    int get()
    {
        return position;
        // return (uint32_t)encoder_knob.getCount();
    }
    void check(int *_animationIndex, bool *_localUpdate)
    {
        button_debouncer.update();
        checkButton(_animationIndex, _localUpdate);
        checkRotary(_localUpdate);
        // 50 hz guard to check encoder position changed
        // if(!(millis() % 20)){
        //   checkRotary();
        // }
    }

    // Set these variables once so they don't need to be set repeatedly
    void setDefaults(int position_, int start_, int finish_, bool loopRotary_ = false)
    {
        Serial.println("MyKnob setDefaults");
        position = position_;
        encoder_knob.setCount(position);
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
            encoder_knob.setCount(position);
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
            encoder_knob.setCount(position);
        }
        return position;
    }
};

#endif
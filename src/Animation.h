#ifndef ANIMATION_H
#define ANIMATION_H

class Animation
{
protected:
    int start;
    int finish;
    int initialSetting;
    long previousMillis = 0;
    bool initialized = false;

    // int remapInRange(int index)
    // {
    //     if (index < 0)
    //         return NUMPIXELS;
    //     if (index > NUMPIXELS)
    //         return 0;
    //     return index;
    // }
    bool nonBlockDelay(unsigned int interval)
    {
        unsigned long currentMillis = millis();
        if ((currentMillis - previousMillis) > interval)
        {
            previousMillis = currentMillis;
            return true;
        }
        return false;
    }
    virtual void setup() = 0;
    virtual void loop() = 0;

public:
    Animation(){};
    void run()
    {
        if (!initialized)
        {
            setup();
            initialized = true;
        };
        loop();
    }
};
#endif
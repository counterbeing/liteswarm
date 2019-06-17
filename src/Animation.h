#ifndef ANIMATION_H
#define ANIMATION_H

class Animation
{
private:
    int start;
    int finish;
    int initialSetting;
    long previousMillis = 0;
    bool initialized;
public:
    Animation(){};
    void run();
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
};
#endif
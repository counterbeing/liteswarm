#ifndef ANIMATION_H
#define ANIMATION_H

class Animation
{
private:
    int start;
    int finish;
    int initialSetting;
    bool initialized;
    void setup();
    void loop();
public:
    Animation(/* args */);
    void run();
};

#endif
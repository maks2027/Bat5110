#ifndef LITETIMER_H
#define LITETIMER_H

#include <Arduino.h>

class LiteCycleTimer
{
public:
    LiteCycleTimer();

    void resetTimeout();
    bool isReady(uint32_t t);
private:
    uint32_t time;
};


class LiteOneTimer
{
public:
    LiteOneTimer();

    void start(uint16_t t);
    void stop();
    bool isReady();
private:
    uint32_t time;
    uint16_t startTime = 0;
};

#endif // LITETIMER_H

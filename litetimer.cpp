#include "litetimer.h"

LiteCycleTimer::LiteCycleTimer()
{

}

void LiteCycleTimer::resetTimeout()
{
    time = millis();
}

bool LiteCycleTimer::isReady(uint32_t t)
{
    if(millis() - time >= t)
    {
        time = millis();

        return true;
    }
    else
        return false;
}

LiteOneTimer::LiteOneTimer()
{

}

void LiteOneTimer::start(uint16_t t)
{
    time = millis();
    startTime = t;
}

void LiteOneTimer::stop()
{
    startTime = 0;
}

bool LiteOneTimer::isReady()
{
    if(startTime == 0) return false;

    if(millis() - time >= startTime)
    {
        startTime = 0;
        return true;
    }
    else
        return false;
}

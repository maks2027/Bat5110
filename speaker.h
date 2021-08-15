#ifndef SPEAKER_H
#define SPEAKER_H

#include <Arduino.h>
#include <GyverTimer.h>

#define SPEK 6


class Speaker
{
public:
    Speaker();

    void alarm(int time = -1);//частое пищание
    void danger(int time = -1);//редкое пищание
    void mess();//одиночный пик

    void noAlarm();
    void noDanger();


    void work();

private:
    GTimer of;
    GTimer freq;
    GTimer timerAlarm;
    GTimer timerDanger;
    int8_t type = 0;

    int8_t _alarm = 0;//тревога
    int8_t _danger = 0;//

    void signal();
};

#endif // SPEAKER_H

#include "speaker.h"

Speaker::Speaker()
{

}

void Speaker::alarm(int time)
{
    if(time > 0)
    {
        timerAlarm.setTimeout(time);
    }
    else
        _alarm++;

    signal();
}

///  если сигнализация запущенна обновим время (нужно обновлять если осталось меньше чем новое, но и так сойдёт)
///  если уровень сигнализации выше то нечего не делаем
///  если сигнализация выключенна - запустим

void Speaker::danger(int time)
{
    if(type > 2) return;

    if(time > 0)
    {
        timerDanger.setTimeout(time);
    }
    else
        _danger++;

    signal();
}

void Speaker::mess()
{
    if(type >= 1) return;

    type = 1;

    signal();
}

void Speaker::noAlarm()
{
    _alarm--;

    if(_alarm <= 0)
        _alarm = 0;

    signal();
}

void Speaker::noDanger()
{
    _danger--;

    if(_danger <= 0)
        _danger = 0;

    signal();
}

void Speaker::signal()
{
    if(_alarm > 0 || timerAlarm.isEnabled())
    {
        if(type < 3)
        {
            type = 3;
            freq.setInterval(500);
        }
    }
    else if(_danger > 0 || timerDanger.isEnabled())
    {
        if(type < 2)
        {
            type = 2;
            freq.setInterval(3000);
        }
    }
    else if(type == 1)
    {
        freq.setInterval(1000);
    }
    else
        freq.stop();
}

void Speaker::work()
{
    if(freq.isReady())
    {
        analogWrite(SPEK,128);
       // tone(SPEK, 2000);
        of.setTimeout(50);
    }

    if(of.isReady())
    {
        analogWrite(SPEK,0);
        //noTone(SPEK);

        if(type == 1)
        {
            freq.stop();
            type = 0;
        }
    }

    if(timerAlarm.isReady())
    {
        if(_alarm > 0)
        {

        }
        else if(_danger > 0 || timerDanger.isEnabled())
        {
            type = 2;
            freq.setInterval(3000);
        }
        else
        {
           type = 0;
           freq.stop();
        }
    }

    if(timerDanger.isReady())
    {
       if(type == 3) return;

       if(_danger > 0)
       {

       }
       else
       {
           type = 0;
           freq.stop();
       }
    }

}

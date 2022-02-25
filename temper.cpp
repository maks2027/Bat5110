#include "temper.h"


Temper::Temper()
{

}

void Temper::begin(uint8_t pin)
{
    oneWire.begin(pin);

    sensors.setOneWire(&oneWire);

    sensors.setResolution(12);

    sensors.setWaitForConversion(false);

    sensors.begin();

    pool.setTimeout(1000);
}

void Temper::work()
{
    if(pool.isReady())
    {
        sensors.requestTemperatures();

        newData = false;

        wait.setTimeout(1000);
    }

    if(wait.isReady())
    {
        for(int i = 0; i < 5; i++)
            temps[i] = sensors.getTempC(address[i]);

        analysis();

        newData = true;

        pool.setTimeout(4000);
    }
}

float Temper::temp(uint8_t ind)
{
    if(ind > 5) return 0.0;

    newData = false;

    return temps[ind];
}

float Temper::tempMax()
{
    return _max;
}

int8_t Temper::getTempDanger() const
{
    return tempDanger;
}

void Temper::analysis()
{
    //максимальная
    _max = temps[0];
    for(int i = 1; i < 5;i++)
    {
        if(_max < temps[i])
            _max = temps[i];
    }

    if(tempDanger == 3) return;//третий уровень не сбрасываемый


    if(temps[0] > 45 || temps[1] > 45 || temps[2] > 45 || temps[3] > 65 || temps[4] > 65)
        tempDanger = 3;
    else if(temps[0] > 40 || temps[1] > 40 || temps[2] > 40 || temps[3] > 60 || temps[4] > 60)
        tempDanger = 2;
    else
        tempDanger = 0;


}

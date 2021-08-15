#ifndef TEMPER_H
#define TEMPER_H

#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <GyverTimer.h>

class Temper
{
public:
    Temper();

    void begin(uint8_t pin);

    void work();

    float temp(uint8_t ind);
    float tempMax();

    int8_t getTempDanger() const;

private:    
    float temps[5] = {0.0};
    float _max = 0;

    int8_t tempDanger = 0;

    DeviceAddress address[5] =
    {
        { 0x28, 0x27, 0x63, 0x56, 0xB5, 0x01, 0x3C, 0xF3 },
        { 0x28, 0x1D, 0x9E, 0x16, 0xA8, 0x01, 0x3C, 0x21 },
        { 0x28, 0xEB, 0xA7, 0x56, 0xB5, 0x01, 0x3C, 0x2C },
        { 0x28, 0x20, 0xD8, 0x56, 0xB5, 0x01, 0x3C, 0x49 },
        { 0x28, 0x33, 0x9E, 0x56, 0xB5, 0x01, 0x3C, 0x3E }
    };

    OneWire oneWire;
    DallasTemperature sensors;

    GTimer pool;
    GTimer wait;

    bool newData = false;

    void analysis();
};

#endif // TEMPER_H

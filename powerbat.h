#ifndef POWERBAT_H
#define POWERBAT_H

#include <Arduino.h>
#include <EEPROM.h>
#include "litetimer.h"
#include <util/atomic.h>

class PowerBat
{
public:
    PowerBat();

    uint16_t begin(uint16_t startByte);
    void init();

    void setVolt(float V);
    void setAmper(float A);    

    void work();

    uint8_t overLoad();//уровни перегрузки (0 в норме)
    bool discharged();
    bool charge();

    void save();

    uint8_t getPercent() const;
    float getW() const;
    float getWh() const;
    float getV() const;
    float getA() const;

    float getWMax() const;
    float getOmBat() const;

private:
    const float minV = 9.9;
    const float errV = 9.0;
    const float maxV = 12.6;

    uint8_t _percent = 0;
    float _W = 0;//остаток ваттов
    float _Wh = 0;//нагрузка
    float _V = 0;
    float _A = 0;

    uint8_t _overLoad = 0;

    bool _discharged = false;
    bool _charge = false;

    float _WMax = 0;
    float _omBat = 0.05;

    float downA = 0;
    float downV = 0;


    bool isA = false;
    bool isV = false;
    unsigned long prevMillis = 0;        
    float _lastValueV = 0.0;
    float _lastValueA = 0.0;

    uint16_t _startByte = 0;    
    uint8_t _WMax_s;
    uint8_t _omBat_s;

    LiteOneTimer errorTime;
    LiteOneTimer dischargedTime;
    LiteOneTimer chargeTime;

    void calculW();

    ///падение напряжения
    float downVolt();

    ///расчёт сопротевление аккумулятора
    float calculOm(float &V1, float &A1, float &V2, float &A2);

    void calculPercent();

    void isDischarged();


    float filterV(float value);
    float filterA(float value);

};

#endif // POWERBAT_H

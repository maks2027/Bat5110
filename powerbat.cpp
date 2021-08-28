#include "powerbat.h"

PowerBat::PowerBat()
{

}

uint16_t PowerBat::begin(uint16_t startByte)
{
    _startByte = startByte;

    uint8_t s = 0;

    s += sizeof(_W);
    _WMax_s = s;
    s += sizeof(_WMax);
    _omBat_s = s;
    s += sizeof(_omBat);

    EEPROM.get(_startByte, _W);
    EEPROM.get(_startByte + _WMax_s, _WMax);
    //EEPROM.get(_startByte + _omBat_s, _omBat);

    return _startByte + s;
}

void PowerBat::init()
{
    _W = 0;
    _WMax = 0;
    _omBat = 0;

    save();
}

void PowerBat::setVolt(float V)
{
    _V = filterV(V);
}

void PowerBat::setAmper(float A)
{
    _A = filterA(A);

    //нужно как-кто по красивше упоковать
    calculW();
    isDischarged();
    calculPercent();
}

void PowerBat::work()
{
    if(errorTime.isReady())
    {
        if(_A >= 5.5)
        {
            _overLoad = 3;
        }
    }

    if(dischargedTime.isReady())
    {
        if(_discharged)
            _W = 0;
    }

    if(chargeTime.isReady())
    {
       _charge = false;
    }
}

uint8_t PowerBat::overLoad()
{
    return _overLoad;
}

bool PowerBat::discharged()
{
    return _discharged;
}

bool PowerBat::charge()
{
    return _charge;
}

void PowerBat::save()
{
    EEPROM.put(_startByte, _W);
    EEPROM.put(_startByte + _WMax_s, _WMax);
    //EEPROM.put(_startByte + _omBat_s, _omBat);
}

uint8_t PowerBat::getPercent() const
{
    return _percent;
}

float PowerBat::getW() const
{
    return _W;
}

float PowerBat::getWh() const
{
    return _Wh;
}

float PowerBat::getV() const
{
    return _V;
}

float PowerBat::getA() const
{
    return _A;
}

float PowerBat::getWMax() const
{
    return _WMax;
}

float PowerBat::getOmBat() const
{
    return _omBat;
}

void PowerBat::calculW()
{
    _W += _A * _V * (millis() - prevMillis) / 3600000; //расчет емкости АКБ в ВтЧ
    prevMillis = millis();

    if(_W < 0)
        _W = 0;

    _Wh = _A * _V;

    if(_Wh > 0)
    {
        _charge = true;
        chargeTime.start(1000);
    }

    if(_charge)
    {
        if(_W > _WMax)
            _WMax = _W;

        if(_V >= maxV && _W < _WMax)
            _WMax = _W;
    }


    if(_overLoad == 3) return;

    if(_A <= -6)
        _overLoad = 3;
    else if(_A <= -5.5)
    {
        if(_overLoad != 2)
            errorTime.start(3000);

        _overLoad = 2;
    }
    else if(_A <= -5)
        _overLoad = 1;
    else
        _overLoad = 0;
}

float PowerBat::downVolt()
{
    return _A * _omBat;
}

float PowerBat::calculOm(float &V1, float &A1, float &V2, float &A2)
{
    return _omBat = V1 - V2 / A1 - A2;
}

void PowerBat::calculPercent()
{
    if(_W <= 0)
        _percent = 0;
    else if(_W >= _WMax)
        _percent = 100;
    else
        _percent = (100 / _WMax) * _W;
}

void PowerBat::isDischarged()
{
    if(!_discharged)
    {
        if(_V < errV)
        {
            dischargedTime.start(2000);
            _discharged = true;
        }

        if(_V - downVolt() < minV)
        {
            dischargedTime.start(2000);
            _discharged = true;
        }
    }
    else
    {
        if(_V > 10.5 + downVolt())
            _discharged = false;
    }
}

float PowerBat::filterV(float value)
{
    if(!isV)
        _lastValueV = value;
    isV = true;

    _lastValueV += (float)(value - _lastValueV) * 0.1;
    return _lastValueV;
}

float PowerBat::filterA(float value)
{
    if(!isA)
        _lastValueA = value;
    isA = true;

    _lastValueA += (float)(value - _lastValueA) * 0.1;
    return _lastValueA;
}

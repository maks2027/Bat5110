#include "keymanager.h"

KeyManager::KeyManager()
{

}

uint16_t KeyManager::begin(uint16_t startByte)
{
    _startByte = startByte;

    for(uint8_t i = 0; i < SIZE; i++)
    {
        pinMode(keys[i], OUTPUT);
    }

    return _startByte + 4;
}

void KeyManager::loadStatus()
{
    for(uint8_t i = 0; i < SIZE; i++)
    {
        keyStat[i] = EEPROM.read(_startByte + i);

        if(!err)
           digitalWrite(keys[i], keyStat[i]);
    }
}

void KeyManager::setKey(uint8_t key, bool stat, bool save)
{
    if(key >= SIZE) return;

    keyStat[key] = stat;

    if(!err)
        digitalWrite(keys[key], keyStat[key]);

    if(save)
        EEPROM.update(_startByte + key, stat);
}

void KeyManager::setKeyInvert(uint8_t key, bool save)
{
    if(key >= SIZE) return;

    setKey(key, !keyStat[key], save);
}

void KeyManager::setKeyAll(bool stat, bool save)
{
    for(uint8_t i = 0; i < SIZE; i++)
    {
        keyStat[i] = stat;

        if(!err)
            digitalWrite(keys[i], keyStat[i]);

        if(save)
            EEPROM.update(_startByte + i, stat);
    }
}

void KeyManager::keyErrOn()
{
    if(err == true) return;

    err = true;

    for(uint8_t i = 0; i < SIZE; i++)
    {
        digitalWrite(keys[i], false);
    }
}

void KeyManager::keyErrOff()
{
    if(err == false) return;

    err = false;

    for(uint8_t i = 0; i < SIZE; i++)
    {
        digitalWrite(keys[i], keyStat[i]);
    }
}

bool KeyManager::getKeyStat(uint8_t key)
{
    if(key >= SIZE) return  false;

    return keyStat[key];
}

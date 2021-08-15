#ifndef KEYMANAGER_H
#define KEYMANAGER_H

#define SIZE 4

#include <Arduino.h>
#include <EEPROM.h>

class KeyManager
{
public:
    KeyManager();

    uint16_t begin(uint16_t startByte);
    void loadStatus();

    void setKey(uint8_t _key, bool stat, bool save = true);
    void setKeyInvert(uint8_t key, bool save = true);
    void setKeyAll(bool stat, bool save = true);
    void keyErrOn();
    void keyErrOff();


    String getKeyName(uint8_t key);
    bool getKeyStat(uint8_t key);

private:
    const uint8_t keys[4] = {8,7,5,4};
    bool keyStat[4] = {false,false,false,false};


    bool err = false;

    uint16_t _startByte = 0;

};

#endif // KEYMANAGER_H

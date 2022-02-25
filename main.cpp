#include <avr/io.h>

#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>
#include <ADS1115_WE.h>
#include "RTClib.h"
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "directADC.h"

#include "GyverWDT.h"
#include "GyverButton.h"
#include <GyverPower.h>

#include "temper.h"
#include "powerbat.h"
#include "keymanager.h"
#include "utf8rus.h"
#include "function.h"
#include "speaker.h"
#include "litetimer.h"

//ключи
#define K0 8
#define K1 7
#define K2 5
#define K3 4

//экран
#define LCD0 A1 //DC
#define LCD1 A3 //CS
#define LCD2 A2 //RST
#define LED 9 //яркость

//кнопки
#define BUT A6

//Температура
#define TEM 2

Adafruit_PCD8544 display = Adafruit_PCD8544(LCD0, LCD1, LCD2);//экран
LiteCycleTimer LedTimer;
LiteCycleTimer ADCTimer;
LiteCycleTimer RTCTimer;
LiteOneTimer SettingsTimer;
LiteOneTimer StartTimer;

Temper sensor;
PowerBat bat;
KeyManager keys;
Speaker spek;

GButton butList;
GButton butEdit;
GButton butUp;
GButton butDown;

int8_t overLoad = 0;
int8_t tempDanger = 0;
//ацп
ADS1115_WE adc = ADS1115_WE();
bool errorAdc = false;
bool channel = false;
unsigned long prevMillis = 0;

bool rtcIsWork = false;
RTC_DS1307 rtc;
DateTime now;
String curTime;
uint8_t cinStep;

volatile bool adcType = true;//флаг какой пин считвается
volatile int vcc = 1023;
volatile int but = 0;

//общее меню
const int8_t sizeMenu = 5;
int8_t selectMenu = 0;
int8_t selectItemMenu[sizeMenu] = {0};
int8_t selectItemMenuSize[sizeMenu] = {0, 0, 0, 4, 3};
bool isEdit = false;

//меню 5
int8_t settingsMenu[3] = {5, 5, 5}; //яркость, контраст, гасить подсветку
int8_t settingsMenuMin[3] = {0, 0, 0};
int8_t settingsMenuMax[3] = {10, 10, 60};
bool settingsMenuEdit[3] = {false};

//extern int __bss_end;
//extern void *__brkval;
//int memoryFree()
//{
//   int freeValue;
//   if((int)__brkval == 0)
//      freeValue = ((int)&freeValue) - ((int)&__bss_end);
//   else
//      freeValue = ((int)&freeValue) - ((int)__brkval);
//   return freeValue;
//}

void dispPos(const float &val, uint8_t n = 3)
{
    if (val >= 0)
        display.print(" ");
    if (val < 10.0 && n >= 2)
        display.print(" ");
    if (val < 100.0 && n >= 3)
        display.print(" ");
}

void menu_0_update()//основные данные
{
    //время
    display.setCursor(0 , 0);
    display.println(curTime);

    display.setCursor(0 , 41);
    if (sensor.tempMax() >= 0) display.print(F("+")); //чтоб не упалзало при минусе
    display.print(sensor.tempMax(), 1);


    display.setCursor(32 , 0);
    if(bat.getPercent() < 100) display.print(" ");
    display.print(bat.getPercent());
    display.setCursor(52 , 0);
    display.print("%");

    display.drawRect(60, 0, 22, 7, BLACK);
    display.fillRect(61, 1, bat.getPercent() / 5, 5, BLACK);
    display.drawRect(82, 2, 2, 3, BLACK);


    //    display.setCursor(0 , 11);
    //    display.println(memoryFree());


    //    if (bat.overLoad() == 3)
    //    {
    //        display.setCursor(3 , 11);
    //        display.print(utf8rus(F("ошибка")));
    //    }
    //    if (bat.discharged())
    //    {
    //        display.setCursor(3 , 18);
    //        display.print(utf8rus(F("разряжен")));
    //    }


    display.setCursor(78 , 23);
    display.print(F("V"));

    display.setCursor(78 , 32);
    display.print(F("A"));

    display.setCursor(78 , 41);
    display.print(F("W"));


    display.setCursor(34 , 23);
    dispPos(bat.getV(),3);
    display.print(bat.getV(), 2);

    display.setCursor(34 , 32);
    dispPos(bat.getA(),3);
    display.print(bat.getA(), 2);

    display.setCursor(34 , 41);
    dispPos(bat.getWh(),3);
    display.print(bat.getWh(), 2);
}

void menu_1_update()// Подробные данные о акумуляторе
{
    display.setCursor(1 , 0);
    display.print(F("V"));

    display.setCursor(1 , 8);
    display.print(F("A"));

    display.setCursor(1 , 16);
    display.print(F("W"));

    display.setCursor(1 , 24);
    display.print(F("Wh"));

    display.setCursor(1 , 32);
    display.print(F("WMax"));

    display.setCursor(1 , 40);
    display.print(F("Om"));


    display.setCursor(30 , 0);
    dispPos(bat.getV());
    display.print(bat.getV(), 3);

    display.setCursor(30 , 8);
    dispPos(bat.getA());
    display.print(bat.getA(), 3);

    display.setCursor(30 , 16);
    dispPos(bat.getW());
    display.print(bat.getW(), 3);

    display.setCursor(30 , 24);
    dispPos(bat.getWh());
    display.print(bat.getWh(), 3);

    display.setCursor(30 , 32);
    dispPos(bat.getWMax());
    display.print(bat.getWMax(), 3);

    display.setCursor(30 , 40);
    dispPos(bat.getOmBat());
    display.print(bat.getOmBat(), 3);
}

void menu_2_update()// Подробные данные о датчиках
{
    display.setCursor(1 , 0);
    display.print(F("BAT 1"));

    display.setCursor(1 , 8);
    display.print(F("BAT 2"));

    display.setCursor(1 , 16);
    display.print(F("BAT 3"));

    display.setCursor(1 , 24);
    display.print(F("USB"));

    display.setCursor(1 , 32);
    display.print(F("DC-DC"));

    for(uint8_t i = 0; i < 5; i++)
    {
        display.setCursor(55 , 8 * i);
        display.print(sensor.temp(i), 1);
    }
}

void menu_3_update()
{
    display.setCursor(13 , 0);
    display.print(F("19V"));

    display.setCursor(13 , 8);
    display.print(F("12V"));

    display.setCursor(13 , 16);
    display.print(F("USB"));

    display.setCursor(13 , 24);
    display.print(F("-"));

    for (uint8_t i = 0; i < selectItemMenuSize[3]; i++)
    {
        //стрелка
        if (i == selectItemMenu[3])
        {
            display.setCursor(1 , 8 * i);
            display.print(F(">"));
        }

        //состояние
        display.setCursor(55 , 8 * i);
        if (keys.getKeyStat(i))
            display.print(F("ON"));
        else
            display.print(F("OFF"));
    }
}

void menu_4_update()//настройки
{
    display.setCursor(13 , 0);
    display.print(F("Light"));

    display.setCursor(13 , 8);
    display.print(F("Contrast"));

    display.setCursor(13 , 16);
    display.print(F("Off light"));

    for (uint8_t i = 0; i < 3; i++)
    {
        //стрелка
        if (i == selectItemMenu[4])
        {
            if (isEdit)
            {
                display.setCursor(1 , 8 * i);
                display.print(F(">>"));
            }
            else
            {
                display.setCursor(1 , 8 * i);
                display.print(F(">"));
            }
        }

        //состояние
        display.setCursor(75 , 8 * i);
        display.println(settingsMenu[i]);
    }
}

void menuUpdate(int8_t i = -1)
{
    int menuInd;

    if(i < 0)
        menuInd = selectMenu;
    else
        menuInd = i;

    display.clearDisplay();

    if (menuInd == 0)
        menu_0_update();
    else if (menuInd == 1)
        menu_1_update();
    else if (menuInd == 2)
        menu_2_update();
    else if (menuInd == 3)
        menu_3_update();
    else if (menuInd == 4)
        menu_4_update();

    display.display();
}

void menuList()
{
    selectMenu++;
    if(selectMenu >= sizeMenu)
        selectMenu = 0;

    menuUpdate();
}

void menuEdit()//клавиша edit
{
    if (selectMenu == 3)//ключи
    {
        int temp_number = selectItemMenu[selectMenu];
        keys.setKeyInvert(temp_number);
    }
    else if (selectMenu == 4)
    {
        isEdit = !isEdit;
    }
}

void menuEditUp()
{
    if (isEdit)
    {
        if (selectMenu == 4)
        {
            settingsMenu[selectItemMenu[selectMenu]]++;

            if (settingsMenu[selectItemMenu[selectMenu]] > settingsMenuMax[selectItemMenu[selectMenu]])
                settingsMenu[selectItemMenu[selectMenu]] = settingsMenuMax[selectItemMenu[selectMenu]];

            if (selectItemMenu[selectMenu] == 0)//яркость
                analogWrite(LED, showToVal(settingsMenu[0]));

            else if (selectItemMenu[selectMenu] == 1)//контраст
                display.setContrast(showToValCon(settingsMenu[1]));

            SettingsTimer.start(3000);
        }
    }
    else
    {
        selectItemMenu[selectMenu]--;
        if (selectItemMenu[selectMenu] < 0)
            selectItemMenu[selectMenu] = selectItemMenuSize[selectMenu] - 1;
    }
}
void menuEditDown()
{
    if (isEdit)
    {
        if (selectMenu == 4)
        {
            settingsMenu[selectItemMenu[selectMenu]]--;

            if (settingsMenu[selectItemMenu[selectMenu]] < settingsMenuMin[selectItemMenu[selectMenu]])
                settingsMenu[selectItemMenu[selectMenu]] = settingsMenuMin[selectItemMenu[selectMenu]];

            if (selectItemMenu[selectMenu] == 0)//яркость
                analogWrite(LED, showToVal(settingsMenu[0]));

            else if (selectItemMenu[selectMenu] == 1)//контраст
                display.setContrast(showToValCon(settingsMenu[1]));

            SettingsTimer.start(3000);
        }
    }
    else
    {
        selectItemMenu[selectMenu]++;
        if(selectItemMenu[selectMenu] >= selectItemMenuSize[selectMenu])
            selectItemMenu[selectMenu] = 0;
    }
}

void ledLight()//обновление состояния подсветки экрана
{
    analogWrite(LED, showToVal(settingsMenu[0]));
    if (settingsMenu[2] != 0) //погасить экран через
        LedTimer.resetTimeout();
}

void adcReady()
{
    if (adcType)
    {
        but = ADC_read();
        setAnalogMux(ADC_A6);
    }
    else
    {
        vcc = ADC_read();

        if(vcc != 1023)//неожиданая пропажа питания
        {
            power.hardwareDisable(PWR_ADC | PWR_I2C | PWR_SPI /*| PWR_TIMER0 | PWR_TIMER1 | PWR_TIMER2 | PWR_UART0*/);

            //регистры uart загрузчик вроде сам натсраивает...
            DDRD = B00000000;
            DDRC = B00000000;
            DDRB = B00000000;

            bat.save();

            while(true){}//сработает вачдог если питание вернулось
        }

        setAnalogMux(ADC_A7);
    }

    adcType = !adcType;
}

void setup()
{
    Watchdog.enable(RESET_MODE, WDT_PRESCALER_128);

    Wire.begin();
    Wire.setWireTimeout();

    //загрузка натсроек
    EEPROM.get(10,settingsMenu[0]);
    EEPROM.get(11,settingsMenu[1]);
    EEPROM.get(12,settingsMenu[2]);

    int eepromAdr = 20;
    eepromAdr = bat.begin(eepromAdr);
    eepromAdr = keys.begin(eepromAdr);

    //экрна
    display.begin();
    display.clearDisplay();
    display.cp437(true);
    display.setContrast(showToValCon(settingsMenu[1]));
    display.setTextSize(1);
    display.setTextColor(BLACK);

    //время
    if (!rtc.begin())
    {
        rtcIsWork = false;
        curTime = "err";
        spek.danger(3000);
    }
    else
    {
        rtcIsWork = true;
        //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        now = rtc.now();
        curTime = now.timestamp(DateTime::TIMESTAMP_STIME);
    }

    cinStep = 0;

    //внешний ацп
    if (!adc.init())
    {
        errorAdc = true;
        spek.danger(3000);
    }
    else
    {
        adc.setVoltageRange_mV(ADS1115_RANGE_0256);
        adc.setMeasureMode(ADS1115_CONTINUOUS);
        adc.setCompareChannels(ADS1115_COMP_0_1);
    }

    Watchdog.reset();

    //клавиши
    butUp.setStepTimeout(100);
    butDown.setStepTimeout(100);

    butList.setType(LOW_PULL);
    butEdit.setType(LOW_PULL);
    butUp.setType(LOW_PULL);
    butDown.setType(LOW_PULL);


    //датчики температуры
    sensor.begin(TEM);

    //подсветка
    pinMode(LED, OUTPUT);
    analogWrite(LED, showToVal(settingsMenu[0]));

    //внутрений ацп
    ADC_enable();
    ADC_setPrescaler(128);
    ADC_setReference(ADC_VCC);
    setAnalogMux(ADC_A7);
    ADC_attachInterrupt(adcReady);
    ADC_autoTriggerEnable(FREE_RUN);
    ADC_startConvert();

    Watchdog.reset();

    menuUpdate();

    StartTimer.start(1000);
}

void loop()
{    
    Watchdog.reset();

    butUp.tick(but < 260 && but > 200);
    butDown.tick(but < 500 && but > 400);
    butEdit.tick(but < 800 && but > 600);
    butList.tick(but > 1000);

    if (butList.isPress())
    {
        ledLight();
        isEdit = false;

        menuList();
    }

    if (butUp.isPress())
    {
        ledLight();
        menuEditUp();
        menuUpdate();
    }

    if (butDown.isPress())
    {
        ledLight();
        menuEditDown();
        menuUpdate();
    }

    if (butEdit.isPress())
    {
        ledLight();
        menuEdit();
        menuUpdate();
    }

    if (butUp.isStep())
    {
        ledLight();
        if (isEdit)
        {
            menuEditUp();
            menuUpdate();
        }
    }
    if (butDown.isStep())
    {
        ledLight();
        if (isEdit)
        {
            menuEditDown();
            menuUpdate();
        }
    }

    //ключи открыть не сразу, а через время
    if(StartTimer.isReady())
    {
        keys.loadStatus();
    }

    //погасить подсветку экрана
    if (LedTimer.isReady(settingsMenu[2] * 1000))
    {
        if (settingsMenu[2] != 0)
            analogWrite(LED, 255);
        else
            analogWrite(LED, showToVal(settingsMenu[0]));
    }

    sensor.work();
    bat.work();
    spek.work();

    //АЦП
    if (ADCTimer.isReady(20) && !errorAdc)
    {
        if (channel) //Ток
        {
            bat.setAmper(adc.getResult_mV() / 1000 / 0.02275);
            adc.setCompareChannels(ADS1115_COMP_0_1);
        }
        else//напряжение
        {
            bat.setVolt(adc.getResult_mV() / 1000 * 84.670);
            adc.setCompareChannels(ADS1115_COMP_2_3);
        }

        channel = !channel;
    }

    //перегрузка
    if(overLoad != bat.overLoad())
    {
        if(bat.overLoad() == 3)
        {
            keys.keyErrOn();
            spek.alarm(5000);
        }
        else if(bat.overLoad() == 2)
        {
            spek.danger(3000);
        }
        else if(bat.overLoad() == 1)
        {

        }
        else
        {

        }

        overLoad = bat.overLoad();
    }

    if(bat.discharged())
    {
        keys.keyErrOn();
    }

    //температура
    if(tempDanger != sensor.getTempDanger())
    {
        if(sensor.getTempDanger() == 3)
        {
            spek.alarm(5000);

            keys.keyErrOn();

            if(tempDanger == 2)//предыдущие состояние было предупреждение (смахивает на костыль..)
                spek.noDanger();
        }
        else if(sensor.getTempDanger() == 2)
        {
            spek.danger();
        }
        else
        {
            if(tempDanger == 2)//предыдущие состояние было предупреждение
                spek.noDanger();
        }

        tempDanger = sensor.getTempDanger();
    }

    if(bat.overLoad() < 2 && sensor.getTempDanger() < 2 && !bat.discharged())
        keys.keyErrOff();

    //время + обновление главного экрана
    if (RTCTimer.isReady(1000))
    {
        if (rtcIsWork)
        {
            now = rtc.now();
            String tempTime = now.timestamp(DateTime::TIMESTAMP_STIME);
            cinStep++;

            if (curTime != tempTime) // если получили новую минуту
            {
                cinStep = 0;
            }

            if (cinStep >= 70) //если время не обновляется то ошибка
            {
                curTime = "error";
                rtcIsWork = false;
                cinStep = 0;
                spek.danger(3000);
            }

            curTime = tempTime;
        }

        menuUpdate();

    }

    //сохранение настроек
    if(SettingsTimer.isReady())
    {
        EEPROM.put(10,settingsMenu[0]);
        EEPROM.put(11,settingsMenu[1]);
        EEPROM.put(12,settingsMenu[2]);
    }
}



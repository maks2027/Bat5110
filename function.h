#include <Arduino.h>

int showToVal(int v)
{
  return (10 - v) / 10.0 * 255; //(-255 + 255)/255*100
}

int showToValCon(int v)
{
  return v * 2 + 50;//(-255 + 255)/255*100
}

//float _lastValueV = 0.0;
//float _lastValueA = 0.0;

//float filterV(float value) {
//  _lastValueV += (float)(value - _lastValueV) * 0.2;
//  return _lastValueV;
//}

//float filterA(float value) {
//  _lastValueA += (float)(value - _lastValueA) * 0.4;
//  return _lastValueA;
//}

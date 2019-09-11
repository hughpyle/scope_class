#ifndef _teensydac_h_
#define _teensydac_h_

#include <Arduino.h>


// #define LOREZ


#define DAC_X_PIN  A21   // Teensy 3.5/3.6 DAC 0
#define DAC_Y_PIN  A22   // Teensy 3.5/3.6 DAC 1


#ifdef LOREZ
const uint16_t  dacRes = 10;
#else
const uint16_t  dacRes = 12;
#endif


#define DAC_MAX_X 4095
#define DAC_MAX_Y 4095

inline void writeDAC(int xx, int yy)
{
    if(xx >= 0 && xx <= DAC_MAX_X)
    {
      if(yy >= 0 && yy <= DAC_MAX_Y)
      {
#ifdef LOREZ
    uint16_t x = (uint16_t)xx >> 2;
    uint16_t y = (uint16_t)yy >> 2;
#else
    int x = xx;
    int y = yy;
#endif
        analogWrite(DAC_X_PIN, x);
        analogWrite(DAC_Y_PIN, y);
      }
    }
}

#endif

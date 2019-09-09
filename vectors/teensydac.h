#ifndef _teensydac_h_
#define _teensydac_h_

#include <Arduino.h>


#define DAC_X_PIN  A21   // Teensy 3.5/3.6 DAC 0
#define DAC_Y_PIN  A22   // Teensy 3.5/3.6 DAC 1


const uint16_t  dacRes = 12;

#define DAC_MAX_X 4095
#define DAC_MAX_Y 4095


inline void writeDAC(int xx, int yy)
{
    if(xx >= 0 && xx <= DAC_MAX_X)
    {
      if(yy >= 0 && yy <= DAC_MAX_Y)
      {
        analogWrite(DAC_X_PIN, xx);
        analogWrite(DAC_Y_PIN, yy);
      }
    }
}

#endif

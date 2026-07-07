#ifndef FIXEDPOINT32_H
#define FIXEDPOINT32_H

#include <stdint.h>

/*
    Q16.16 fixed-point format

    16 bits -> integer part
    16 bits -> fractional part

    Example:

    1.0  -> 65536
    2.5  -> 163840
    0.5  -> 32768
*/

typedef int32_t fixed;  // 32-bit signed fixed-point value in Q16.16 format

#define FIX_SHIFT 16
#define FIX_SCALE (1 << FIX_SHIFT) //FIX_SCALE = 65536
#define FIX_HALF (1 << 15)       //FIX_HALF = 32768 is 0.5, used for rounding when converting fixed to integer.

// Define the conversion 
fixed fixFromInt(int x);
int   fixToInt(fixed x);

fixed fixFromFraction(int numerator, int denominator);  //avoid the use of float using int fractions

// Definition of operations
fixed fixMul(fixed a, fixed b);  //Multiplication
fixed fixDiv(fixed a, fixed b); //Division

#endif // FIXEDPOINT32_H
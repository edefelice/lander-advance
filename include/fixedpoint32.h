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

typedef int32_t fixed;                                                              // 32-bit signed fixed-point value in Q16.16 format

#define FIX_SHIFT32 16
#define FIX_SCALE32 (1 << FIX_SHIFT32)                                                  //FIX_SCALE32 = 65536
#define FIX_HALF (1 << 15)                                                          //FIX_HALF = 32768 is 0.5, used for rounding when converting fixed to integer.

// Define the conversion in macro since we use them for inizialise global constant
#define FIX_FROM_INT(x)             ((int64_t)(x) << FIX_SHIFT32)
#define FIX_TO_INT(x)               (((x) + FIX_HALF) >> FIX_SHIFT32)
#define FIX_FROM_FRACTION(n,d)      ((fixed)(((int64_t)(n) << FIX_SHIFT32) / (d)))    //avoid the use of float using int fractions

// Definition of operations
fixed fixMul(fixed a, fixed b);                                                     //Multiplication
fixed fixDiv(fixed a, fixed b);                                                     //Division
fixed fixAbs(fixed a);                                                              //Absolute

#endif // FIXEDPOINT32_H
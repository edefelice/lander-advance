#include "fixedpoint32.h"

#include <stdint.h>
#include <assert.h>

//Initialization
fixed fixFromInt(int x)
{
    return ((fixed)x << FIX_SHIFT);                            // To convert a int to a fixed <<16 multiply int * 65536
}

int   fixToInt(fixed x)
{
    return ((x + FIX_HALF) >> FIX_SHIFT);                 // >>16 does the division of the fixed number, giving the int number with approximation
}  

// Convert a rational number (numerator/denominator) to Q16.16
fixed fixFromFraction(int numerator, int denominator)
{
    assert(denominator != 0);                            // If denominator is 0, the game will block in debug mode
    return (((fixed)numerator << FIX_SHIFT) / denominator);
}     

//Multiplication
fixed fixMul(fixed a, fixed b)                           //this is needed to go from int64 to int32. We need int64_t to avoid overflow
{
    return (fixed)(((int64_t)a * b) >> FIX_SHIFT);
}

//Division
fixed fixDiv(fixed a, fixed b)
{
    return ((int64_t)a << FIX_SHIFT) / b;  
}
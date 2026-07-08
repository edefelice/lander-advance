#include "fixedpoint32.h"

#include <stdint.h>
#include <assert.h>
#include <limits.h>

/*
    Fixed-point math library (Q16.16)
 
    Uses int64_t internally to prevent intermediate overflow.
    Division by zero is handled by saturation:
        positive numerator -> INT_MAX
        negative numerator -> INT_MIN
 */

//Multiplication
fixed fixMul(fixed a, fixed b)                           //this is needed to go from int64 to int32. We need int64_t to avoid overflow
{
    return (fixed)(((int64_t)a * b) >> FIX_SHIFT);
}

//Division
fixed fixDiv(fixed a, fixed b)
{
    if (b == 0)
    {
        // If 'a' is >= 0, simulate +infinity
        // If 'a' is < 0, simulate -infinity
        return (a >= 0) ? INT_MAX : INT_MIN;
    }
    return (fixed)((((int64_t)a) << FIX_SHIFT) / b);  
}
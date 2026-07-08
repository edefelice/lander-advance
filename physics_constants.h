#ifndef PHYSICS_CONSTANT_H
#define PHYSICS_CONSTANT_H

#include "fixedpoint32.h"
#include <stdint.h>

/* 
    Is defined the constants of the physics divided in block
    
    V1: definition of moon info
        definition of LEM info
        
*/

//time
const fixed dt = fixFromFraction(1674 , 100000);            //definition of dt 1/59,73


//MOON

//gravity
#define MOON_G              FIX_FROM_FRACTION(81,50)        //Moon gravity 1.62 m/s^2, readed value 106168


//LEM info

//Thrust main and RCS
#define MAIN_THRUST         FIX_FROM_INT(30000)             //Thrust main engine 30000N
#define RCS_THRUST          FIX_FROM_INT(440)               //Thrust RCS engines 440N

//mass
#define DRY_MASS            FIX_FROM_INT(6713)              //Dry mass 6713 Kg
#define PROP_MASS           FIX_FROM_INT(8487)              //Propellant mass 8487 Kg
#define CREW_MASS           FIX_FROM_INT(160)               // Crew mass (2 astronauts, 80 kg each)

//mass flow main and RCS (Consumption)
#define MAIN_CONSUMPTION    FIX_FROM_FRACTION(17,100)       // kg/frame
#define RCS_CONSUMPTION     FIX_FROM_FRACTION(13,5000)      // kg/frame



#endif // PHYSICS_CONSTANT_H
#ifndef PHYSICS_CONSTANT_H
#define PHYSICS_CONSTANT_H

#include "fixedpoint32.h"
#include <stdint.h>

/* 
    Definition of physics constants divided in blocks
    
    V1: definition of moon info
        definition of LEM info
    
    Definition of simulation constants
        
*/


// ---------------------------
// MOON
// ---------------------------

//gravity
#define MOON_G              FIX_FROM_FRACTION(81,50)        // Moon gravity (1.62 m/s^2)
//altitude
#define MOON_H              FIX_FROM_INT(3000)              // Moon starting altitude (3000 m)


// ---------------------------
// LEM
// ---------------------------

//Dimention
#define LEM_RADIUS          FIX_FROM_FRACTION(21,10)        //Diameter ≈ 4,2 m -> Radius 2,1 m

//Thrust main and RCS
#define MAIN_THRUST         FIX_FROM_INT(30000)                 //Thrust main engine 30000N
#define RCS_THRUST          FIX_FROM_INT(440)                   //Thrust RCS engines 440N

//mass
#define DRY_MASS            FIX_FROM_INT(6713)                  //Dry mass 6713 Kg
#define PROP_MASS           FIX_FROM_INT(4244)                  // Full Propellant mass 8487 Kg
#define CREW_MASS           FIX_FROM_INT(160)                   // Crew mass (2 astronauts, 80 kg each)

//mass flow main and RCS (Consumption)
#define MAIN_CONSUMPTION    FIX_FROM_FRACTION(17,100)           // kg/frame
#define RCS_CONSUMPTION     FIX_FROM_FRACTION(13,5000)          // kg/frame

// Rotational inertia coefficient
#define INERTIA_FACTOR      FIX_FROM_FRACTION(1,2)              //Approximation of the LEM to a cilinder , I = 1/2 m r²

// Angolar normalization
#define FIX_TWO_PI          FIX_FROM_FRACTION(628319,100000)    //2π = 6.28319 rad    

// Power source
#define P_USES              6                                   // Number of available uses of the power source     
// ---------------------------
// SIMULATION
// ---------------------------

//time
#define SIM_DT              FIX_FROM_FRACTION(1674,100000)  // Simulation timestep (≈1/59.73 s)



#endif // PHYSICS_CONSTANT_H
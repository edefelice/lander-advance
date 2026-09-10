#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <tonc.h>

#include "fixedpoint32.h"
#include "cockpit.h"        
#include "landing_area.h"
#include "game_result.h"


/*
------------------------------------------------------------
Gameplay Module

This module implements the physical simulation of the Lunar
Module (LEM).

Responsibilities:
- Initialize the lander state.
- Update translational dynamics.
- Update rotational dynamics.
- Manage propellant consumption.
- Detect landing and crash conditions.

Rendering and HUD management are handled by other modules.
------------------------------------------------------------
*/


//enumerator define the lander status
typedef enum
{
    LANDER_FLYING,
    LANDER_LANDED,
    LANDER_CRASHED
} LanderStatus;


//Lander struct
typedef struct {

    //Environment
    fixed gravity;              //Gravity of the planet (Q16.16)

    //Position
    fixed x;                    // Global position x (Q16.16)
    fixed y;                    // Global position y (Q16.16)
    
    //Altitude
    fixed z;                    // Altitude (Q16.16)
    
    //Velocity
    fixed vx;                   // velocity x axis (Q16.16)
    fixed vy;                   // velocity y axis (Q16.16)
    fixed vz;                   // velocity z axis (Q16.16)
    
    //Rotation
    fixed theta;                //Rotation angle (Q16.16 radians)
    fixed omega;                //Angular velocity (rad/s, Q16.16)

    //Propellant
    fixed propellant;           //current propellant mass (Q16.16)

    //Remaining power
    u8 available_power;         //current power available

    //Touchdown data save
    fixed touchdown_vx;         //velocity x axis during the touchdown (Q16.16)
    fixed touchdown_vy;         //velocity y axis during the touchdown (Q16.16)
    fixed touchdown_vz;         //velocity z axis during the touchdown (Q16.16)
    fixed touchdown_omega;      //Angular velocity during the touchdown (Q16.16)
    fixed pad_d_sqr;            //Distance from the closest pad at landing (!=0 when land on a pad) (Q16.16)

    //Crash information
    GrReason crash_reason;
    bool light_on;
    u16 light_timer;

    //Lander state
    LanderStatus state;

} Lander;


// ---------------------------
// INITIALIZATION
// ---------------------------

void GameplayInit(Lander *lander);                              //initialization of lander data

// ---------------------------
// READ FUNCTIONS
// ---------------------------

fixed GameplayGetMass(const Lander *lander);                    //read the total current mass 

fixed GameplayGetPropPercent(const Lander *lander);             //percentage of propellant available

bool GameplayHasPropellant(const Lander *lander);               //return if there is propellant

// ---------------------------
// UPDATE FUNCTIONS
// ---------------------------

void GameplayUpdate(Lander *lander, const PlayerInput *input, bool fast_mode, int area, const Sites *moon_sites);                              //manage the update functions

void UpdateMainEngine(Lander *lander, const PlayerInput *input, fixed mass);                //main engine physics

void UpdateRCS(Lander *lander, const PlayerInput *input, fixed mass, bool fast_mode);       //RCS engine physics

void UpdateLinearPhysics(Lander *lander);                                                   //position update

void UpdateRotation(Lander *lander, const PlayerInput *input, fixed mass, bool fast_mode);  //rotation physics

void UpdateCollision(Lander *lander, int area, const Sites *moon_sites);                                                       //check the lander state


#endif // GAMEPLAY_H
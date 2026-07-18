#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <tonc.h>

#include "fixedpoint32.h"
#include "cockpit.h"        //ci sta lo struct input, non essendoci nella cartella ora, lascio lo struct nel codice (RICORDATI DI CANCELLARLO ALLA FINE)


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
    fixed gravity;      //Gravity of the planet (Q16.16)

    //Position
    fixed x;            // Global position x (Q16.16)
    fixed y;            // Global position y (Q16.16)
    
    //Altitude
    fixed z;            // Altitude (Q16.16)
    
    //Velocity
    fixed vx;           // velocity x axis (Q16.16)
    fixed vy;           // velocity y axis (Q16.16)
    fixed vz;           // velocity z axis (Q16.16)
    
    //Rotation
    fixed theta;        //Rotation angle (Q16.16 radians)
    fixed omega;        //Angular velocity (rad/s, Q16.16)

    //propellant
    fixed propellant;   //current propellant mass (Q16.16)

    //Remaining power
    u8 available_power; //current power available

    //Lander state
    LanderStatus state;

} Lander;


// Player input place holder - now available in cockpit.h
/* typedef struct {
    bool thrust_main;       // 1 if A is pressed (main engine), 0 (no buttons)
    int8_t rcs_x;           // -1 (left), 1 (right), 0 (no buttons)
    int8_t rcs_y;           // -1 (down), 1 (up), 0 (no buttons)
    int8_t rotate;          // -1 (L, sx rotation), 1 (R, dx rotation), 0 (no buttons)
} PlayerInput;
*/

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

void GameplayUpdate(Lander *lander, const PlayerInput *input);                              //manage the update functions

void UpdateMainEngine(Lander *lander, const PlayerInput *input, fixed mass);                //main engine physics

void UpdateRCS(Lander *lander, const PlayerInput *input, fixed mass);                       //RCS engine physics

void UpdateLinearPhysics(Lander *lander);                                                   //position update

void UpdateRotation(Lander *lander, const PlayerInput *input, fixed mass);                  //rotation physics

void UpdateCollision(Lander *lander);                                                       //check the lander state


#endif // GAMEPLAY_H
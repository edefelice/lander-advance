#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <tonc.h>

#include "fixedpoint32.h"
#include "physics_constants.h"



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
    fixed gravity;      //Gravity of the planet (16.16)

    //Position
    fixed x;            // Global position x (16.16)
    fixed y;            // Global position y (16.16)
    
    //Altitude
    fixed z;            // Altitude (16.16)
    
    //Velocity
    fixed vx;           // velocity x axis (16.16)
    fixed vy;           // velocity y axis (16.16)
    fixed vz;           // velocity z axis (16.16)
    
    //Rotation
    uint16_t theta;     //Yaw rotation angle (0-511 GBA units)

    //propellant
    fixed propellant;   //current prepellant mass (16.16)

    //Lander state
    LanderStatus state;

} Lander;


// Player input place holder
typedef struct {
    bool thrust_main;       // 1 if A is pressed (main engine), 0 (no buttons)
    int8_t rcs_x;           // -1 (left), 1 (right), 0 (no buttons)
    int8_t rcs_y;           // -1 (down), 1 (up), 0 (no buttons)
    int8_t rotate;          // -1 (L, sx rotation), 1 (R, dx rotation), 0 (no buttons)
} PlayerInput;


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

void GameplayUpdate(Lander *lander, const PlayerInput *input);                                  //manage the update functions

void UpdateMainEngine(Lander *lander, const PlayerInput *input, fixed mass);     //main engine physics

void UpdateRCS(Lander *lander, const PlayerInput *input, fixed mass);                           //RCS engine physics

void UpdateLinearPhysics(Lander *lander);                                                       //position update

void UpdateRotation(Lander *lander, const PlayerInput *input);                                  //aggiorna theta, cosnumo RCS di rotazione e normalizzazione dell'angolo

void UpdateCollision(Lander *lander);                                                            //check the lander state


#endif // GAMEPLAY_H
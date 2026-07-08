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

    //Position
    fixed x;            // Global position x (16.16)
    fixed y;            // Global position y (16.16)
    fixed z;            // Altitude (16.16)
    
    //Velocity
    fixed vx;           // velocity x axis (16.16)
    fixed vy;           // velocity y axis (16.16)
    fixed vz;           // velocity z axis (16.16)
    
    //Rotation
    uint16_t theta;     //Yaw rotation angle (0-511 GBA units)

    //propellant
    fixed propellant;   //current prepellant mass (16.16)
/*
    //Engine state
    bool mainEngine;    //Thrust state
    bool rcsTop;        // Upper RCS firing
    bool rcsBottom;     // Lower RCS firing
    bool rcsLeft;       // Left RCS firing
    bool rcsRight;      // Right RCS firing
*/
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



// A PIERLUCA SERVE LA PERCENTUALE DI PROPELLANT MASS RIMANENTE

void GameplayInit(Lander *lander);  //inizializzazione del gioco

fixed GameplayGetMass(const Lander *lander);    //legge la current mass totale

void GameplayUpdate(Lander *lander, const PlayerInput *input);      //richiama le funzioni nel giusto ordine

void UpdateMainEngine(Lander *lander, const PlayerInput *input, fixed mass);    //calcola la fisica del main engine

void UpdateRCS(Lander *lander, const PlayerInput *input, fixed mass);           //calcola la fisica degli RCS

void UpdateLinearPhysics(Lander *lander);                           //aggiornamento posizione

void UpdateRotation(Lander *lander, const PlayerInput *input);      //aggiorna theta, cosnumo RCS di rotazione e normalizzazione dell'angolo

void Collision(Lander *lander);                                     //decide lo stato del lander


#endif // GAMEPLAY_H
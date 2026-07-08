#include "gameplay.h"


/*
    gameplay.c

    This module manages the lander gameplay and physics simulation.

    Responsibilities:
    - Lander initialization
    - Mass and propellant management
    - Main engine physics
    - RCS physics
    - Linear motion update
    - Rotation update
    - Collision detection
    - Gameplay update loop

    Physics uses Q16.16 fixed-point arithmetic.
*/


void GameplayInit(Lander *lander){                  //inizializzazione del gioco

    //Environment
    lanted->gravity = MOON_G;

    //Position
    lander->x = FIX_FROM_INT(0);
    lander->y = FIX_FROM_INT(0);
    lander->z = FIX_FROM_INT(5000);                 //Initial altitude 5000m
    
    //Velocity
    lander->vx = 0;
    lander->vy = 0;
    lander->vz = 0;
    
    //Rotation
    lander->theta = 0;

    //propellant
    lander->propellant=PROP_MASS;                   // Serbatoio pieno (8487 kg)

    //Lander state
    lander->state = LANDER_FLYING;
}


//Read the information about mass
fixed GameplayGetMass(const Lander *lander){        //legge la current mass totale

    fixed mass = DRY_MASS + CREW_MASS + lander->propellant;
    return mass;
   
}         


//Return the percentage of Propellant still in the tank
fixed GameplayGetPropPercent(const Lander *lander){        

    fixed propPercent = fixDiv(lander->propellant, PROP_MASS);
    return propPercent;
   
}        


//Read if there is Propellant in the tank
bool GameplayHasPropellant(const Lander *lander){        

    return (lander->propellant > 0);
   
}



//Manage the main engine data
void UpdateMainEngine(Lander *lander, const PlayerInput *input, fixed mass){

    if(input->thrust_main && GameplayHasPropellant(lander)) {

        fixed az= fixDiv(MAIN_THRUST , mass) - lander->gravity;

        lander->vz += fixMul(az, SIM_DT);
        lander->propellant -= MAIN_CONSUMPTION;

    }

    else
    {

        lander->vz -= fixMul(lander->gravity, SIM_DT);

    }

}       


//Manage the RCS engine data
void UpdateRCS(Lander *lander, const PlayerInput *input, fixed mass){

    fixed ax= fixDiv(RCS_THRUST , mass); 
    fixed ay= fixDiv(RCS_THRUST , mass); 

    //X AXIS
    if(input->rcs_x == 1 && GameplayHasPropellant(lander)) {

        lander->vx -= fixMul(ax, SIM_DT);
        lander->propellant -= RCS_CONSUMPTION;

    }

    else if(input->rcs_x == -1 && GameplayHasPropellant(lander)) {

        lander->vx += fixMul(ax, SIM_DT);
        lander->propellant -= RCS_CONSUMPTION;
        
    }

    //Y AXIS
    if(input->rcs_y == 1 && GameplayHasPropellant(lander)) {

        lander->vy -= fixMul(ay, SIM_DT);
        lander->propellant -= RCS_CONSUMPTION;

    }

    else if(input->rcs_y == -1 && GameplayHasPropellant(lander)) {

        lander->vy += fixMul(ay, SIM_DT);
        lander->propellant -= RCS_CONSUMPTION;
        
    }

}                


//Manage the rotation
void UpdateRotation(Lander *lander, const PlayerInput *input){

   asd
    
}  


//Manage the position updates
void UpdateLinearPhysics(Lander *lander){

    lander->x += fixMul(lander->vx, SIM_DT);
    lander->y += fixMul(lander->vy, SIM_DT);
    lander->z += fixMul(lander->vz, SIM_DT);
   
}                               
          

//Define the lander status                     
void UpdateCollision(Lander *lander){

    if (lander->z <= 0) {

        lander->z = 0;

        if (lander->vz < -FIX_FROM_INT(4)) {             // 4 m/s limit for a good land 
            lander->vx = 0;
            lander->vy = 0;
            lander->vz = 0;
            lander->state = LANDER_CRASHED;             // Crash!
        } 
        
        else if (lander->vz >= -FIX_FROM_INT(4)) {

            lander->vx = 0;
            lander->vy = 0;
            lander->vz = 0;
            lander->state = LANDER_LANDED;             //Successfully landed 
        }
    }
   
}                                          


//manage the gameplay functions for the update 
void GameplayUpdate(Lander *lander, const PlayerInput *input){

    if(lander->state == LANDER_FLYING){

        fixed mass = GameplayGetMass(lander);
        UpdateMainEngine(lander, input, mass); 
        UpdateRCS(lander, input, mass);
        UpdateRotation(lander, input);
        UpdateLinearPhysics(lander);
        UpdateCollision(lander); 
    }

    else if(lander->state == LANDER_CRASHED){
        return;
        //funzione motivazione del crash vel elevata e targhet mancato
    }

    else if(lander->state == LANDER_LANDED){
        return;
        //funzione score cosnumo propellant e atterraggio con velocità più vicina a 4  
        //e placeholder moltiplicatore difficoltà area di atterraggio e moltiplicatore difficoltà peso CREW_MASS 
    }
}    


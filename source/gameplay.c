#include "gameplay.h"

#include <stdint.h>
#include <stdbool.h>
#include <tonc.h>

#include "fixedpoint32.h"
#include "physics_constants.h"





void GameplayInit(Lander *lander){                  //inizializzazione del gioco

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
/*
    //Engine state
    bool mainEngine;    //Thrust state
    bool rcsTop;        // Upper RCS firing
    bool rcsBottom;     // Lower RCS firing
    bool rcsLeft;       // Left RCS firing
    bool rcsRight;      // Right RCS firing
*/
    //Lander state
    lander->state = STATE_FLYING;
}


//Read the information about mass
fixed GameplayGetMass(const Lander *lander){        //legge la current mass totale

    fixed mass = DRY_MASS + CREW_MASS + lander->propellant;
   
}         


//Manage the main engine data
void UpdateMainEngine(Lander *lander, const PlayerInput *input, fixed mass){

    fixed az= fixDiv(MAIN_THRUST , mass) - MOON_G; 

    if(input->thrust_main == 1 && lander->propellant>0){

        lander->vz += fixMul(az, SIM_DT);
        lander->propellant -= MAIN_CONSUMPTION;

    }

    else if(input->thrust_main == 0){

        lander->vz -= fixMul(MOON_G, SIM_DT);

    }

}       


//Manage the RCS engine data
void UpdateRCS(Lander *lander, const PlayerInput *input, fixed mass){

    fixed ax= fixDiv(RCS_THRUST , mass); 
    fixed ay= fixDiv(RCS_THRUST , mass); 

    //X AXIS
    if(input->rcs_x == 1 && lander->propellant>0){

        lander->vx -= fixMul(ax, SIM_DT);
        lander->propellant -= RCS_CONSUMPTION;

    }

    else if(input->rcs_x == -1 && lander->propellant>0){

        lander->vx += fixMul(ax, SIM_DT);
        lander->propellant -= RCS_CONSUMPTION;
        
    }

    //Y AXIS
    if(input->rcs_y == 1 && lander->propellant>0){

        lander->vy -= fixMul(ax, SIM_DT);
        lander->propellant -= RCS_CONSUMPTION;

    }

    else if(input->rcs_y == -1 && lander->propellant>0){

        lander->vy += fixMul(ax, SIM_DT);
        lander->propellant -= RCS_CONSUMPTION;
        
    }

}                


//Manage the position updates
void UpdateLinearPhysics(Lander *lander){

    lander->x += fixMul(lander->vx, SIM_DT);
    lander->y += fixMul(lander->vy, SIM_DT);
    lander->z += fixMul(lander->vz, SIM_DT);
   
}                               


//Manage the rotation
void UpdateRotation(Lander *lander, const PlayerInput *input){

   asd
    
}            


//Define the lander status                      // INSERIRE LE FUNZIONI DI PUNTEGGI SCORE NEGLI IF, NELLE IF PER DARE IL RETURN A RAFFA (se non ti ricori chiedi a raffa)
void Collision(Lander *lander){

    if (lander->z <= 0) { 
        if (lander->vz > FIX_FROM_INT(4)) {             // 4 m/s limit for a good land 
            lander->state = LANDER_CRASHED;             // Crash!
        } 
        
        else if (lander->vz <= FIX_FROM_INT(4)) {
            lander->state = LANDER_LANDED;             //Successfully landed 
        }
    }
   
}                                          


//manage the gameplay functions for the update 
void GameplayUpdate(Lander *lander, const PlayerInput *input){

    If(lander->state == LANDER_FLYING){

        fixed mass = GameplayGetMass(lander);
        UpdateMainEngine(Lander *lander, const PlayerInput *input, fixed mass); 
        UpdateRCS(Lander *lander, const PlayerInput *input, fixed mass);
        UpdateLinearPhysics(Lander *lander);
        UpdateRotation(Lander *lander, const PlayerInput *input);
        Collision(Lander *lander); 
    }

    else if(lander->state == LANDER_CRASHED){
        return 0
        //funzione motivazione del crash vel elevata e targhet mancato
    }

    else if(lander->state == LANDER_LANDED){
        return 0
        //funzione score cosnumo propellant e atterraggio con velocità più vicina a 4  
        //e placeholder moltiplicatore difficoltà area di atterraggio e moltiplicatore difficoltà peso CREW_MASS 
    }
}    


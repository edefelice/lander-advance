#include "gameplay.h"
#include "fixedpoint32.h"
#include "physics_constants.h"
#include "tonc_math.h"
#include "tonc_types.h"
#include "landing_stat.h"

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


void GameplayInit(Lander *lander){                  //game initialization

    //Environment
    lander->gravity = MOON_G;

    //Position
    lander->x = FIX_FROM_INT(0);
    lander->y = FIX_FROM_INT(0);
    lander->z = MOON_H;                             //Initial altitude 3000m
    
    //Velocity
    lander->vx = 0;
    lander->vy = 0;
    lander->vz = 0;
    
    //Rotation
    lander->theta = 0;
    lander->omega = 0;

    //propellant
    lander->propellant=PROP_MASS;                   // Full tank (8487 kg)

    //Remaining power
    lander->available_power = P_USES;
    lander->light_on = false;
    lander->light_timer = 0;

    //Lander state
    lander->state = LANDER_FLYING;

    //Crash information
    lander->crash_reason = GR_REASON_NONE;

    //Touchdown data initialization
    lander->touchdown_vx = 0;
    lander->touchdown_vy = 0;
    lander->touchdown_vz = 0;
    lander->touchdown_omega = 0;

}


//Read the information about mass
fixed GameplayGetMass(const Lander *lander){

    fixed mass = DRY_MASS + CREW_MASS + lander->propellant;
    return mass;
   
}         


//Return the percentage of Propellant still in the tank
fixed GameplayGetPropPercent(const Lander *lander){        

    fixed propPercent = fixMul(fixDiv(lander->propellant, PROP_MASS), FIX_FROM_INT(100));
    return propPercent;
   
}        


//Read if there is propellant in the tank
bool GameplayHasPropellant(const Lander *lander){        

    return (lander->propellant > 0);
   
}


//Manage the main engine data
void UpdateMainEngine(Lander *lander, const PlayerInput *input, fixed mass){

    if(input->thrust_main && GameplayHasPropellant(lander)) {

        fixed az= fixDiv(MAIN_THRUST , mass) - lander->gravity;

        lander->vz += fixMul(az, SIM_DT);
        lander->propellant -= MAIN_CONSUMPTION;

        if(lander->propellant < 0)
            lander->propellant = 0;
    }

    else
    {

        lander->vz -= fixMul(lander->gravity, SIM_DT);

    }

}       
 

//Manage the RCS engine data
void UpdateRCS(Lander *lander, const PlayerInput *input, fixed mass, bool fast_mode){

    fixed rcs_thrust = fast_mode ? (RCS_THRUST * 10) : RCS_THRUST;
    fixed a = fixDiv(rcs_thrust, mass);
    fixed a_body_x = 0;
    fixed a_body_y = 0; 

    //X AXIS
    if(input->rcs_x == 1 && GameplayHasPropellant(lander)) {
        a_body_x = -a;
    }
    else if(input->rcs_x == -1 && GameplayHasPropellant(lander)) {
        a_body_x = a;
    }

    //Y AXIS
    if(input->rcs_y == 1 && GameplayHasPropellant(lander)) {
        a_body_y = -a;
    }
    else if(input->rcs_y == -1 && GameplayHasPropellant(lander)) {
        a_body_y = a;   
    }

    if(a_body_x == 0 && a_body_y == 0) {
        return; // Does not consume propellant
    }

    if(a_body_x != 0) {
        lander->propellant -= RCS_CONSUMPTION;
    }
    if(a_body_y != 0) {
        lander->propellant -= RCS_CONSUMPTION;
    }
    if(lander->propellant < 0) {
        lander->propellant = 0;
    }
 
    u16 theta = (u16)fixDiv(lander->theta, FIX_TWO_PI); // Convert theta in brad
    fixed cos_theta = (fixed)(lu_cos(theta) << 4);
    fixed sin_theta = (fixed)(lu_sin(theta) << 4);
    fixed ax_world = fixMul(a_body_x, cos_theta) + fixMul(a_body_y, sin_theta);
    fixed ay_world = -fixMul(a_body_x, sin_theta) + fixMul(a_body_y, cos_theta);
    lander->vx += fixMul(ax_world, SIM_DT);
    lander->vy += fixMul(ay_world, SIM_DT);
}

//Manage the rotation
void UpdateRotation(Lander *lander, const PlayerInput *input, fixed mass, bool fast_mode){

    // Calculate angular acceleration
    fixed rcs_thrust = fast_mode ? (RCS_THRUST * 10) : RCS_THRUST;
    fixed torque = fixMul(rcs_thrust, LEM_RADIUS);
    fixed inertia = fixMul (fixMul(INERTIA_FACTOR, mass), fixMul(LEM_RADIUS, LEM_RADIUS));  
    fixed alpha = fixDiv(torque, inertia);

    //Press L, clockwise rotation
    if(input->rotate == -1 && GameplayHasPropellant(lander)) {

        lander->omega += fixMul(alpha, SIM_DT);
        lander->propellant -= RCS_CONSUMPTION;

        if(lander->propellant < 0)
            lander->propellant = 0;        

    }

    //Press R, counterclockwise rotation
    if(input->rotate == 1 && GameplayHasPropellant(lander)) {

        lander->omega -= fixMul(alpha, SIM_DT);
        lander->propellant -= RCS_CONSUMPTION;

        if(lander->propellant < 0)
            lander->propellant = 0;        

    }

    // Update rotation angle
    lander->theta += fixMul(lander->omega, SIM_DT);

    //Normalize angle

    while(lander->theta >= FIX_TWO_PI){

        lander->theta -= FIX_TWO_PI;

    }

    while(lander->theta < 0){

        lander->theta += FIX_TWO_PI;

    }
    
}  


//Manage the position updates
void UpdateLinearPhysics(Lander *lander) {

    lander->x += fixMul(lander->vx, SIM_DT);
    lander->y += fixMul(lander->vy, SIM_DT);
    lander->z += fixMul(lander->vz, SIM_DT);
   
}                               
          

//Define the lander status                                                  
void UpdateCollision(Lander *lander, int area_idx, const Sites *moon_sites) {


    if (lander->z <= 0) {

        bool pad_status = 0;                                                                            // Variable for pad status on landing
        fixed pad_distance_sqr = 0;                                                                     // Variable for pad distance on landing

        lander->z = 0;
        
        // Compute pad status and lander distance from pad

        IsonPad(lander, area_idx, moon_sites, &pad_status, &pad_distance_sqr);

         //Save touchdown values before stopping the lander

        lander->touchdown_vx = lander->vx;
        lander->touchdown_vy = lander->vy;
        lander->touchdown_vz = lander->vz;
        lander->touchdown_omega = lander->omega;
        lander->pad_d_sqr = pad_distance_sqr;
 

        if (!pad_status) {                                                                          //Pad position

            lander->crash_reason = GR_REASON_OUT_OF_PAD;
            lander->state = LANDER_CRASHED;                                                         // Crash!

        } 

        else if ( fixAbs(lander->vz) > MAX_LANDING_VZ) {                                            // Vertcal limit
            
            lander->crash_reason = GR_REASON_VERTICAL_SPEED;
            lander->state = LANDER_CRASHED;                                                         // Crash!
        } 

        else if (fixAbs(lander->vx) > MAX_LANDING_VX || fixAbs(lander->vy) > MAX_LANDING_VY) {      // Traslational limit

            lander->crash_reason = GR_REASON_HORIZONTAL_SPEED;
            lander->state = LANDER_CRASHED;                                                         // Crash!

        } 

        else if (fixAbs(lander->omega) > MAX_LANDING_OMEGA)      {                                  // Angular velocity limit

            lander->crash_reason = GR_REASON_ANGULAR_SPEED;
            lander->state = LANDER_CRASHED;                                                         // Crash!

        } 

        else {

            lander->crash_reason = GR_REASON_NONE;
            lander->state = LANDER_LANDED;                                                          //Successfully landed 
        }

            lander->vx = 0;
            lander->vy = 0;
            lander->vz = 0;
            lander->omega = 0;
    }
}                                         


//manage the gameplay functions for the update 
void GameplayUpdate(Lander *lander, const PlayerInput *input, bool fast_mode, int area_idx, const Sites *moon_sites) {

    if(lander->state == LANDER_FLYING){

        if (input->light) {
            if (lander->light_on) {
                //lander->light_on = false;
                //lander->light_timer = 0;
            } else if (lander->available_power > 0) {
                lander->available_power--;
                lander->light_on = true;
                lander->light_timer = 1800;
            }
        }

        if (lander->light_on) {
            if (lander->light_timer > 0) {
                lander->light_timer--;
            }
            if (lander->light_timer == 0) {
                lander->light_on = false;
            }
        }

        if (input->radar) {
            if (lander->radar_on) {
                //lander->radar_on = false;
                //lander->radar_timer = 0;
            } else if (lander->available_power > 0) {
                lander->available_power--;
                lander->radar_on = true;
                lander->radar_timer = 1800;
            }
        }

        if (lander->radar_on) {
            if (lander->radar_timer > 0) {
                lander->radar_timer--;
            }
            if (lander->radar_timer == 0) {
                lander->radar_on = false;
            }
        }

        fixed mass = GameplayGetMass(lander);
        UpdateMainEngine(lander, input, mass); 
        UpdateRCS(lander, input, mass, fast_mode);
        UpdateRotation(lander, input, mass, fast_mode);
        UpdateLinearPhysics(lander);
        UpdateCollision(lander, area_idx, moon_sites); 
    }

}    



#ifndef COCKPIT_H
#define COCKPIT_H

#include <string.h>

#include <tonc.h>

// Definitions for Gameplay actions

#define MAIN_THR_ON    0x001  
#define TOP_THR_ON      0x002   // Lateral thruster top (towards down) ON
#define LEFT_THR_ON     0x004   // Lateral thruster left (towards right) ON 
#define BOT_THR_ON      0x008   // Lateral thruster bottom (towards up) ON 
#define RIGHT_THR_ON    0x010   // Lateral thruster right (towards left) ON 
#define R_ROT_THR_ON    0x020   // Rotation thruster right (counter-clockwise) ON 
#define L_ROT_THR_ON    0x040   // Rotation thruster left (clockwise) ON
#define RADAR_ON        0x080 
#define LIGHT_ON        0x100   
 

// Definitions for Menus actions

#define M_CONFIRM       0x001  
#define M_RETURN        0x002
#define M_UP            0x004   
#define M_LEFT          0x008    
#define M_DOWN          0x010   
#define M_RIGHT         0x020   
 
// Definitions for General

#define PAUSE           0x200 

u16 cpit_input(void);
u16 menu_input(void);


#endif
#ifndef COCKPIT_H
#define COCKPIT_H

#include <string.h>

#include <tonc.h>

// Struct for Player Inputs (note: added to follow the one used in gameplay.h, remove the struct from gameplay.h and
// include this file or remove this struct and add #include <gameplay.h> to this file)

// Player inputS
typedef struct {

    int8_t rcs_x;           // -1 (left thruster), 1 (right thruster), 0 (no buttons)
    int8_t rcs_y;           // -1 (bottom thuster), 1 (upper thruster), 0 (no buttons)
    int8_t rotate;          // -1 (L, clockwise rotation), 1 (R, counter-clockwise rotation), 0 (no buttons)
    bool thrust_main;       // 1 if A is pressed (main engine), 0 (no buttons)
    bool light;             // 0 if off, 1 if on
    bool radar;             // 0 if off, 1 if on
    bool pause;             
    bool popipopi;          // It does... something...

} PlayerInput;

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
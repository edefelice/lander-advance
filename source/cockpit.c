#include "cockpit.h"

u16 cpit_input()
{
u16 action = 0;


VBlankIntrWait(); 
key_poll(); 

if(key_is_down(KEY_A)){
    action ^ MAINT_THR_ON;
}
if(key_is_down(KEY_UP)){
    action ^ TOP_THR_ON;
}

if(key_is_down(KEY_LEFT)){
    action ^ LEFT_THR_ON;
}

if(key_is_down(KEY_DOWN)){
    action ^ BOT_THR_ON;
}
if(key_is_down(KEY_RIGHT)){
    action ^ RIGHT_THR_ON;
}

if(key_hit(KEY_R) && key_held(KEY_B)){
    action ^ RADAR_ON;
} else if (key_is_down(KEY_R)){
    action ^ L_ROT_THR_ON;
}

if(key_hit(KEY_R) && key_held(KEY_B)){
    action ^ RADAR_ON;
} else if (key_is_down(KEY_L)){
    action ^ L_ROT_THR_ON;
}


}

u16 menu_input()
{
u16 actions;

VBlankIntrWait();
key_poll();



}
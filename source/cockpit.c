#include "cockpit.h"

u16 cpit_input()
{
u16 action = 0;

VBlankIntrWait();   // To remove if already used outside
key_poll();         // To remove if already used outside

if(key_is_down(KEY_A)){
    action = action | MAINT_THR_ON;
}
if(key_is_down(KEY_UP)){
    action = action | TOP_THR_ON;
}

if(key_is_down(KEY_LEFT)){
    action = action | LEFT_THR_ON;
}

if(key_is_down(KEY_DOWN)){
    action = action | BOT_THR_ON;
}
if(key_is_down(KEY_RIGHT)){
    action = action | RIGHT_THR_ON;
}

if(key_hit(KEY_R) && key_held(KEY_B)){
    action = action | RADAR_ON;
} else if (key_is_down(KEY_R)){
    action = action | L_ROT_THR_ON;
}

if(key_hit(KEY_L) && key_held(KEY_B)){
    action = action | RADAR_ON;
} else if (key_is_down(KEY_L)){
    action = action | L_ROT_THR_ON;
}

if(key_hit(KEY_START)){
    action = action | PAUSE;
}

return action;
}

u16 menu_input()
{
u16 action = 0;

VBlankIntrWait();   // To remove if already used outside
key_poll();         // To remove if already used outside

if(key_hit(KEY_A)){
    action = action | M_CONFIRM;
}
if(key_hit(KEY_B)){
    action = action | M_RETURN;
}
if(key_hit(KEY_UP)){
    action = action | M_UP;
}

if(key_hit(KEY_LEFT)){
    action = action | M_LEFT;
}

if(key_hit(KEY_DOWN)){
    action = action | M_DOWN;
}
if(key_hit(KEY_RIGHT)){
    action = action | M_RIGHT;
}

if(key_hit(KEY_START)){
    action = action | PAUSE;
}

return action;
}
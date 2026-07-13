#include <cockpit.h>

PlayerInput cpit_input(){    

    // Initialize the status to 0
    PlayerInput inputs = {0, 0, 0, 0, 0, 0, 0, 0};


    // Wait for the update of the screen to poll the keys
    VBlankIntrWait();   // To remove if already used outside
    key_poll();         // To remove if already used outside


    // Convert the status of the keys to an intent to be used during flight
    if(key_is_down(KEY_A)){
        inputs.thrust_main = 1;
    }

    if(key_is_down(KEY_LEFT)){
       inputs.rcs_x = -1;
    }

    if(key_is_down(KEY_RIGHT)){
        inputs.rcs_x = 1;
    }

    if(key_is_down(KEY_UP)){
        inputs.rcs_y = 1;
    }

    if(key_is_down(KEY_DOWN)){
        inputs.rcs_y = -1;
    }

    if(key_hit(KEY_R) && key_held(KEY_B)){
        inputs.radar = 1;
    } else if (key_is_down(KEY_R)){
        inputs.rotate = 1;
    }

    if(key_hit(KEY_L) && key_held(KEY_B)){
        inputs.light = 1;
    } else if (key_is_down(KEY_L)){
        inputs.rotate = -1;
    }

    if(key_hit(KEY_START)){
        inputs.pause = 1;
    }

    if(key_hit(KEY_SELECT)){
        inputs.popipopi = 1;
    }

    return inputs;
}

u16 menu_input(){

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
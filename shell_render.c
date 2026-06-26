#include <tonc.h>
#include "shell.h"
#include "shell_render.h"

#define COLOR_WHITE 0X7FFF
#define COLOR_RED 0X001F
#define COLOR_BLU 0x7C00
#define COLOR_SILVER 0x5EF7

void shell_render_engine_init(void) { //Initialize engine and set Mode 0 for GBA
   REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;
   tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
   

}

void shell_render_display(void) { 
   GameState present_state = shell_state(); //GameState from shell.h
   tte_erase_screen();
   switch (present_state) {
    case STATE_TITLE:
    tte_set_color(COLOR_SILVER);
    tte_set_pos(75, 80);
    tte_write("LANDER ADVANCE\n");

   tte_set_color = COLOR_WHITE;
    tte_set_pos(69, 100);
    tte_write("Press A to launch");
    break;

    case STATE_CELESTIAL_BODY_SELECTION:
    tte_set_color(COLOR_WHITE);
    tte_set_pos(33, 50);
    tte_write("Set up the flight trajectory\n\n");

    break;

    case STATE_AREA_SELECTION:
    tte_set_color(COLOR_WHITE);
    tte_set_pos(48, 50);
    tte_write("Predict the landing area\n\n");

    break;

    case STATE_CONFIG_SELECTION:
    tte_set_pos(69, 45);
    tte_write("Select the lander\n");
    tte_set_pos(51, 57);
    tte_write("and the organic payload");

    break;

    case STATE_LANDING:
    tte_set_color(COLOR_BLU);
    tte_set_pos(66, 80);
    tte_write("Landed succesfully\n\n");

    break;

     case STATE_CRASH:
     tte_set_color(COLOR_RED);
    tte_set_pos(33, 80);
    tte_write("ERROR! Reprocessing required.\n\n");

    break;

     case STATE_FIN:
     tte_set_color(COLOR_SILVER);
    tte_set_pos(72, 80);
    tte_write("Final Telemetry:\n\n");

    break;
    
    default:
    break;







       
    
   }


    
}
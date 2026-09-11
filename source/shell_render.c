#include <tonc.h>
#include "shell.h"
#include "shell_render.h"
#include <stdio.h>
#include "game_data.h"

#define COLOR_WHITE 0X7FFF
#define COLOR_SILVER 0x5EF7
#define COLOR_RED 0x001F
#define COLOR_BLU 0x7C00
#define COLOR_GREEN 0x03ED
#define BASE_OFFSET 8

#define PB_GREEN  15
#define PB_SILVER 14
#define PB_BLU    13
#define PB_RED    12
#define PB_WHITE  11

static void set_text_color(u16 color) {
    u16 pb = PB_GREEN;
    if (color == COLOR_SILVER) pb = PB_SILVER;
    else if (color == COLOR_BLU) pb = PB_BLU;
    else if (color == COLOR_RED) pb = PB_RED;
    else if (color == COLOR_WHITE) pb = PB_WHITE;
    tte_set_special(SE_PALBANK(pb));
}

#define tte_set_color(type, clr) set_text_color(clr)

void shell_render_engine_init(void) {
   //font and tte init
   tte_init_se_default(0, BG_CBB(1) | BG_SBB(15) | BG_4BPP | BG_PRIO(0));
   pal_bg_mem[PB_GREEN * 16 + 1] = COLOR_GREEN;
   pal_bg_mem[PB_SILVER * 16 + 1] = COLOR_SILVER;
   pal_bg_mem[PB_BLU * 16 + 1] = COLOR_BLU;
   pal_bg_mem[PB_RED * 16 + 1] = COLOR_RED;
   pal_bg_mem[PB_WHITE * 16 + 1] = COLOR_WHITE;
}

void shell_render_display(void) { 
   GameState present_state = shell_state();
   char buffer[32]; //buffer format used only for numbers
   const PlanetData* p_data;
   const AreaData* a_data;
   const LanderData* l_data;
   
   pal_bg_mem[PB_GREEN * 16 + 1] = COLOR_GREEN;
   pal_bg_mem[PB_SILVER * 16 + 1] = COLOR_SILVER;
   pal_bg_mem[PB_BLU * 16 + 1] = COLOR_BLU;
   pal_bg_mem[PB_RED * 16 + 1] = COLOR_RED;
   pal_bg_mem[PB_WHITE * 16 + 1] = COLOR_WHITE;

   tte_erase_screen();
   
   switch (present_state) {
    case STATE_TITLE:
        tte_set_color(TTE_INK, COLOR_SILVER); 
        tte_set_pos(BASE_OFFSET * BASE_OFFSET, 6 * BASE_OFFSET); 
        tte_write("LANDER ADVANCE");
        
        tte_set_color(TTE_INK, COLOR_WHITE); 
        tte_set_pos(6 * BASE_OFFSET, 10 * BASE_OFFSET);
        tte_write("Press A to launch");
        break;

    case STATE_GAME_MODE_SELECTION:
        tte_set_color(TTE_INK, COLOR_SILVER);
        tte_set_pos(4 * BASE_OFFSET, 2 * BASE_OFFSET);
        tte_write("Select Game Mode");

        tte_set_color(TTE_INK, COLOR_BLU);
        tte_set_pos(2 * BASE_OFFSET, 4 * BASE_OFFSET);
        tte_write(is_fast_mode() ? "  Simulation" : "> Simulation");

        tte_set_color(TTE_INK, COLOR_GREEN);
        tte_set_pos(4 * BASE_OFFSET, 6 * BASE_OFFSET);
        tte_write("Original game mode,");
        tte_set_pos(4 * BASE_OFFSET, 7 * BASE_OFFSET);
        tte_write("realistic experience");

        tte_set_color(TTE_INK, COLOR_RED);
        tte_set_pos(2 * BASE_OFFSET, 9 * BASE_OFFSET);
        tte_write(is_fast_mode() ? "> Fast" : "  Fast");

        tte_set_color(TTE_INK, COLOR_GREEN);
        tte_set_pos(4 * BASE_OFFSET, 11 * BASE_OFFSET);
        tte_write("RCS more powerful,");
        tte_set_pos(4 * BASE_OFFSET, 12 * BASE_OFFSET);
        tte_write("faster rotation,");
        tte_set_pos(4 * BASE_OFFSET, 13 * BASE_OFFSET);
        tte_write("shorter game time");

        tte_set_color(TTE_INK, COLOR_GREEN);
        tte_set_pos(2 * BASE_OFFSET, 16 * BASE_OFFSET);
        tte_write("Press A to confirm");
        break;

    case STATE_CELESTIAL_BODY_SELECTION:
      switch (body_state()){
         case SUB_SHUTTLE_MOVING:
             p_data = planet_data(planet_index());
             tte_set_color(TTE_INK, COLOR_SILVER);
             
             tte_set_pos(4 * BASE_OFFSET, 4 * BASE_OFFSET); tte_write("Flight trajectory");
             tte_set_color(TTE_INK, COLOR_GREEN);
             tte_set_pos(2 * BASE_OFFSET, 6 * BASE_OFFSET); tte_write("Celestial Body:");
             
             tte_set_pos(2 * BASE_OFFSET, BASE_OFFSET * BASE_OFFSET);
             tte_write(planet_index() == 0 ? "> " : "  ");
             tte_write(p_data->planet_name);
             
             tte_set_pos(2 * BASE_OFFSET, 14 * BASE_OFFSET); tte_write("Press A for Info");
             break;
             
         case SUB_BODY_INFO:
             p_data = planet_data(planet_index());
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(7 * BASE_OFFSET, 4 * BASE_OFFSET); tte_write("Body Info");
             
             tte_set_color(TTE_INK, COLOR_GREEN);
             tte_set_pos(2 * BASE_OFFSET, 6 * BASE_OFFSET); tte_write("Name: "); tte_write(p_data->planet_name);
             
             tte_set_pos(2 * BASE_OFFSET, BASE_OFFSET * BASE_OFFSET); tte_write("Gravity: ");
             snprintf(buffer, sizeof(buffer), "%d.%02d m/s^2", p_data->gravity_int, p_data->gravity_dec);
             tte_write(buffer);

             tte_set_pos(2 * BASE_OFFSET, 10 * BASE_OFFSET); tte_write("Starting: ");
             snprintf(buffer, sizeof(buffer), "%d m", p_data->starting_altitude);
             tte_write(buffer);
             
             tte_set_pos(2 * BASE_OFFSET, 12 * BASE_OFFSET); tte_write("Desc: "); tte_write(p_data->planet_description);
             break;
             
         case SUB_MODE_SELECTION:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(4 * BASE_OFFSET, 4 * BASE_OFFSET); tte_write("Select Flight Mode");
             
             tte_set_color(TTE_INK, COLOR_BLU);
             tte_set_pos(2 * BASE_OFFSET, 7 * BASE_OFFSET);
             tte_write(is_night_mode() ? "  Day Mode" : "> Day Mode");
             
             tte_set_color(TTE_INK, COLOR_RED);
             tte_set_pos(2 * BASE_OFFSET, 9 * BASE_OFFSET);
             tte_write(is_night_mode() ? "> Night Mode" : "  Night Mode");
             
             tte_set_color(TTE_INK, COLOR_GREEN);
             tte_set_pos(2 * BASE_OFFSET, 14 * BASE_OFFSET); tte_write("Press A to confirm");
             break;
      }
      break;

    case STATE_AREA_SELECTION: 
      switch (area_state()) {
         case SUB_AREA_POINTER_MOVING:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(3 * BASE_OFFSET, 4 * BASE_OFFSET); tte_write("Select landing area");
             
             tte_set_color(TTE_INK, COLOR_GREEN);
             tte_set_pos(2 * BASE_OFFSET, 6 * BASE_OFFSET);
             tte_write(area_index() == 0 ? "> " : "  "); tte_write(area_data(planet_index(), 0)->area_name);
             
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(2 * BASE_OFFSET, BASE_OFFSET * BASE_OFFSET);
             tte_write(area_index() == 1 ? "> " : "  "); tte_write(area_data(planet_index(), 1)->area_name);
             
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(2 * BASE_OFFSET, 10 * BASE_OFFSET);
             tte_write(area_index() == 2 ? "> " : "  "); tte_write(area_data(planet_index(), 2)->area_name);
             
             if (area_is_coming_soon()) {
                 tte_set_color(TTE_INK, COLOR_SILVER);
                 tte_set_pos(2 * BASE_OFFSET, 15 * BASE_OFFSET); tte_write("Coming soon...");
             } else {
                 tte_set_color(TTE_INK, area_index() == 0 ? COLOR_GREEN : COLOR_SILVER);
                 tte_set_pos(2 * BASE_OFFSET, 15 * BASE_OFFSET); tte_write("Press A for Info");
             }
             break;
             
         case SUB_AREA_INFO:
             a_data = area_data(planet_index(), area_index());
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(4 * BASE_OFFSET, 4 * BASE_OFFSET); tte_write("Landing Area Info");
             
             tte_set_color(TTE_INK, COLOR_GREEN);
             tte_set_pos(2 * BASE_OFFSET, 6 * BASE_OFFSET); tte_write("Name: "); tte_write(a_data->area_name);
             
             tte_set_pos(2 * BASE_OFFSET, BASE_OFFSET * BASE_OFFSET); tte_write("Diff: ");
             snprintf(buffer, sizeof(buffer), "%d", a_data->difficulty);
             tte_write(buffer);
             
             tte_set_pos(2 * BASE_OFFSET, 10 * BASE_OFFSET); tte_write("Desc: "); tte_write(a_data->area_description);
             break;
      }
      break;

    case STATE_CONFIG_SELECTION:
        l_data = lander_data(lander_index());
        switch (config_state()) {
          case SUB_CONFIG_POINTER_MOVING:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(4 * BASE_OFFSET, 4 * BASE_OFFSET); tte_write("Select the lander");
             
             tte_set_color(TTE_INK, COLOR_GREEN);
             tte_set_pos(2 * BASE_OFFSET, 7 * BASE_OFFSET); tte_write("> "); tte_write(l_data->lander_name);
             
             tte_set_pos(2 * BASE_OFFSET, 14 * BASE_OFFSET); tte_write("Press A for Info");
             break;
             
          case SUB_LANDER_INFO:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(7 * BASE_OFFSET, 4 * BASE_OFFSET); tte_write("Lander Info");
             
             tte_set_color(TTE_INK, COLOR_GREEN);
             tte_set_pos(2 * BASE_OFFSET, 6 * BASE_OFFSET); tte_write("Name: "); tte_write(l_data->lander_name);
             
             tte_set_pos(2 * BASE_OFFSET, BASE_OFFSET * BASE_OFFSET); tte_write("Crew: ");
             snprintf(buffer, sizeof(buffer), "%d", l_data->max_crew);
             tte_write(buffer);
             
             tte_set_pos(2 * BASE_OFFSET, 10 * BASE_OFFSET); tte_write("Mass: ");
             snprintf(buffer, sizeof(buffer), "%d kg", l_data->mass);
             tte_write(buffer);

             tte_set_pos(2 * BASE_OFFSET, 12 * BASE_OFFSET); tte_write("Fuel: ");
             snprintf(buffer, sizeof(buffer), "%d kg", l_data->fuel);
             tte_write(buffer);
             
             tte_set_pos(2 * BASE_OFFSET, 14 * BASE_OFFSET); tte_write("Thrust: ");
             snprintf(buffer, sizeof(buffer), "%d N", l_data->thrust_main);
             tte_write(buffer);

             tte_set_pos(2 * BASE_OFFSET, 16 * BASE_OFFSET); tte_write("RCS: ");
             snprintf(buffer, sizeof(buffer), "%d N", l_data->thrust_rcs);
             tte_write(buffer);
             break;
             
          case SUB_CREW_INFO:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(2 * BASE_OFFSET, 4 * BASE_OFFSET); tte_write("Select organic payload");
             
             tte_set_color(TTE_INK, COLOR_GREEN);
             tte_set_pos(2 * BASE_OFFSET, BASE_OFFSET * BASE_OFFSET); tte_write("Lander: "); tte_write(l_data->lander_name);
             
             tte_set_pos(2 * BASE_OFFSET, 10 * BASE_OFFSET);
             snprintf(buffer, sizeof(buffer), "Crew: < %d > / %d", crew_count(), max_crew());
             tte_write(buffer);
             
             tte_set_pos(2 * BASE_OFFSET, 15 * BASE_OFFSET); tte_write("A launch | B back");
             break;
        }
        break;
/*
    case STATE_GAMEPLAY:
        tte_set_color(TTE_INK, COLOR_BLU); 
        tte_set_pos(25, 40); tte_write("--- GAMEPLAY ACTIVE ---");
        
        tte_set_color(TTE_INK, COLOR_WHITE);
        tte_set_pos(8, 70); tte_write("TAB:Pause | R:Win | L:Lose");
        break;
*/
    case STATE_PAUSE: 
        if (pause_state() == SUB_SHOW_CREDITS) {
            tte_set_color(TTE_INK, COLOR_SILVER);
            tte_set_pos(11 * BASE_OFFSET, 3 * BASE_OFFSET); tte_write("CREDITS");

            tte_set_color(TTE_INK, COLOR_GREEN);
            tte_set_pos(2 * BASE_OFFSET, 6 * BASE_OFFSET); tte_write("Ernesto De Felice");
            tte_set_pos(2 * BASE_OFFSET, 8 * BASE_OFFSET); tte_write("Pierluca De Felice");
            tte_set_pos(2 * BASE_OFFSET, 10 * BASE_OFFSET); tte_write("Raffaele Colamarino");
            tte_set_pos(2 * BASE_OFFSET, 12 * BASE_OFFSET); tte_write("Raffaele Aucelli");

            tte_set_color(TTE_INK, COLOR_WHITE);
            tte_set_pos(2 * BASE_OFFSET, 15 * BASE_OFFSET); tte_write("Press A / B to return");
        }
        else {
            tte_set_color(TTE_INK, COLOR_SILVER);
            tte_set_pos(7 * BASE_OFFSET, 4 * BASE_OFFSET); tte_write("PAUSE");
            
            tte_set_color(TTE_INK, COLOR_GREEN);
            tte_set_pos(2 * BASE_OFFSET, 6 * BASE_OFFSET); tte_write(pause_index() == 0 ? "> Resume" : "  Resume");
            tte_set_pos(2 * BASE_OFFSET, BASE_OFFSET * BASE_OFFSET); tte_write(pause_index() == 1 ? "> Restart" : "  Restart");
            tte_set_pos(2 * BASE_OFFSET, 10 * BASE_OFFSET); tte_write(pause_index() == 2 ? "> Title" : "  Title");
            tte_set_pos(2 * BASE_OFFSET, 13 * BASE_OFFSET); tte_write(pause_index() == 3 ? "> Credits" : "  Credits");
        }
        break;

    case STATE_LANDING:
        tte_set_color(TTE_INK, COLOR_BLU);
        tte_set_pos(9 * BASE_OFFSET, 9 * BASE_OFFSET); tte_write("Landed!");
        tte_set_pos(5 * BASE_OFFSET, 14 * BASE_OFFSET); tte_write("Press A for data");
        break;

    case STATE_CRASH:
        tte_set_color(TTE_INK, COLOR_RED); 
        tte_set_pos(10 * BASE_OFFSET, 9 * BASE_OFFSET); tte_write("CRASH!");
        tte_set_pos(5 * BASE_OFFSET, 14 * BASE_OFFSET); tte_write("Press A for data");
        break;

    case STATE_FIN:
        tte_set_color(TTE_INK, COLOR_SILVER);
        tte_set_pos(5 * BASE_OFFSET, 4 * BASE_OFFSET); tte_write("Final Telemetry:");
        
        tte_set_color(TTE_INK, COLOR_GREEN);
        tte_set_pos(2 * BASE_OFFSET, 7 * BASE_OFFSET); tte_write("Result: "); tte_write(result_victory() ? "SUCCESS" : "DESTROYED");
        
        tte_set_pos(2 * BASE_OFFSET, 9 * BASE_OFFSET); tte_write("Score: ");
        snprintf(buffer, sizeof(buffer), "%d", result_score());
        tte_write(buffer);
        
        tte_set_pos(2 * BASE_OFFSET, 11 * BASE_OFFSET); tte_write("Cause: "); tte_write(result_reason());
        
        tte_set_color(TTE_INK, COLOR_BLU);
        tte_set_pos(2 * BASE_OFFSET, 14 * BASE_OFFSET); tte_write(fin_index() == 0 ? "> Restart" : "  Restart");
        tte_set_color(TTE_INK, COLOR_RED);
        tte_set_pos(2 * BASE_OFFSET, 16 * BASE_OFFSET); tte_write(fin_index() == 1 ? "> Title" : "  Title");
        break;
    
    default: 
        break;
   }
}
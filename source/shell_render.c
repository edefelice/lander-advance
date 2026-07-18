#include <tonc.h>
#include "shell.h"
#include "shell_render.h"
#include <stdio.h>
#include "game_data.h"

#define COLOR_WHITE 0X7FFF
#define COLOR_SILVER 0x5EF7
#define COLOR_RED 0xFA68
#define COLOR_BLU 0x7C00

void shell_render_engine_init(void) {
   //font and tte init
   tte_init_se_default(0, BG_CBB(1) | BG_SBB(15) | BG_4BPP | BG_PRIO(0));
}

void shell_render_display(void) { 
   GameState present_state = shell_state();
   char buffer[32]; //buffer format used only for numbers
   const PlanetData* p_data;
   const AreaData* a_data;
   const LanderData* l_data;
   
   tte_erase_screen();
   
   switch (present_state) {
    case STATE_TITLE:
        tte_set_color(TTE_INK, COLOR_SILVER); 
        tte_set_pos(64, 50); 
        tte_write("LANDER ADVANCE");
        
        tte_set_color(TTE_INK, COLOR_WHITE); 
        tte_set_pos(52, 80);
        tte_write("Press A to launch");
        break;

    case STATE_CELESTIAL_BODY_SELECTION:
      switch (body_state()){
         case SUB_SHUTTLE_MOVING:
             p_data = planet_data(planet_index());
             tte_set_color(TTE_INK, COLOR_WHITE);
             
             tte_set_pos(8, 20); tte_write("Flight trajectory");
             tte_set_pos(8, 45); tte_write("Celestial Body:");
             
             tte_set_pos(8, 65);
             tte_write(planet_index() == 0 ? "> " : "  ");
             tte_write(p_data->planet_name);
             
             tte_set_pos(8, 110); tte_write("Press A for Info");
             break;
             
         case SUB_BODY_INFO:
             p_data = planet_data(planet_index());
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(8, 20); tte_write("Body Info");
             
             tte_set_color(TTE_INK, COLOR_WHITE);
             tte_set_pos(8, 45); tte_write("Name: "); tte_write(p_data->planet_name);
             
             tte_set_pos(8, 65); tte_write("Gravity: ");
             snprintf(buffer, sizeof(buffer), "%s", p_data->gravity);
             tte_write(buffer);

             tte_set_pos(8, 85); tte_write("Starting: ");
             snprintf(buffer, sizeof(buffer), "%s m", p_data->starting_altitude);
             tte_write(buffer);
             
             tte_set_pos(8, 105); tte_write("Desc: "); tte_write(p_data->planet_description);
             break;
      }
      break;

    case STATE_AREA_SELECTION: 
      switch (area_state()) {
         case SUB_AREA_POINTER_MOVING:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(8, 20); tte_write("Predict the landing area");
             
             tte_set_color(TTE_INK, COLOR_WHITE);
             tte_set_pos(8, 45);
             tte_write(area_index() == 0 ? "> " : "  "); tte_write(area_data(planet_index(), 0)->area_name);
             
             tte_set_pos(8, 65);
             tte_write(area_index() == 1 ? "> " : "  "); tte_write(area_data(planet_index(), 1)->area_name);
             
             tte_set_pos(8, 85);
             tte_write(area_index() == 2 ? "> " : "  "); tte_write(area_data(planet_index(), 2)->area_name);
             
             tte_set_pos(8, 120); tte_write("Press A for Info");
             break;
             
         case SUB_AREA_INFO:
             a_data = area_data(planet_index(), area_index());
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(8, 20); tte_write("Landing Area Info");
             
             tte_set_color(TTE_INK, COLOR_WHITE);
             tte_set_pos(8, 45); tte_write("Name: "); tte_write(a_data->area_name);
             
             tte_set_pos(8, 65); tte_write("Diff: ");
             snprintf(buffer, sizeof(buffer), "%d", a_data->difficulty);
             tte_write(buffer);
             
             tte_set_pos(8, 85); tte_write("Desc: "); tte_write(a_data->area_description);
             break;
      }
      break;

    case STATE_CONFIG_SELECTION:
        l_data = lander_data(lander_index());
        switch (config_state()) {
          case SUB_CONFIG_POINTER_MOVING:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(8, 30); tte_write("Select the lander");
             
             tte_set_color(TTE_INK, COLOR_WHITE);
             tte_set_pos(8, 55); tte_write("> "); tte_write(l_data->lander_name);
             
             tte_set_pos(8, 110); tte_write("Press A for Info");
             break;
             
          case SUB_LANDER_INFO:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(8, 20); tte_write("Lander Info");
             
             tte_set_color(TTE_INK, COLOR_WHITE);
             tte_set_pos(8, 40); tte_write("Name: "); tte_write(l_data->lander_name);
             
             tte_set_pos(8, 60); tte_write("Crew: ");
             snprintf(buffer, sizeof(buffer), "%d", l_data->max_crew);
             tte_write(buffer);
             
             tte_set_pos(8, 80); tte_write("Mass: ");
             snprintf(buffer, sizeof(buffer), "%d kg", l_data->mass);
             tte_write(buffer);

             tte_set_pos(8, 100); tte_write("Fuel: ");
             snprintf(buffer, sizeof(buffer), "%d kg", l_data->fuel);
             tte_write(buffer);
             
             tte_set_pos(8, 120); tte_write("Thrust: ");
             snprintf(buffer, sizeof(buffer), "%d kN", l_data->thrust_main);
             tte_write(buffer);

             tte_set_pos(8, 140); tte_write("RCS: ");
             snprintf(buffer, sizeof(buffer), "%d kN", l_data->thrust_rcs);
             tte_write(buffer);
             break;
             
          case SUB_CREW_INFO:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(8, 25); tte_write("Select organic payload");
             
             tte_set_color(TTE_INK, COLOR_WHITE);
             tte_set_pos(8, 55); tte_write("Lander: "); tte_write(l_data->lander_name);
             
             tte_set_pos(8, 75);
             snprintf(buffer, sizeof(buffer), "Crew: < %d > / %d", crew_count(), max_crew());
             tte_write(buffer);
             
             tte_set_pos(8, 120); tte_write("Press A launch | B back");
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
        tte_set_color(TTE_INK, COLOR_SILVER);
        tte_set_pos(8, 20); tte_write("PAUSE");
        
        tte_set_color(TTE_INK, COLOR_WHITE);
        tte_set_pos(8, 45); tte_write(pause_index() == 0 ? "> Resume" : "  Resume");
        tte_set_pos(8, 65); tte_write(pause_index() == 1 ? "> Restart" : "  Restart");
        tte_set_pos(8, 85); tte_write(pause_index() == 2 ? "> Title" : "  Title");
        tte_set_pos(8, 105); tte_write(pause_index() == 3 ? "> Credits" : "  Credits");
        break;

    case STATE_LANDING:
        tte_set_color(TTE_INK, COLOR_BLU);
        tte_set_pos(8, 70); tte_write("Landed! Press A for data");
        break;

    case STATE_CRASH:
        tte_set_color(TTE_INK, COLOR_RED); 
        tte_set_pos(8, 70); tte_write("CRASH! Press A for data");
        break;

    case STATE_FIN:
        tte_set_color(TTE_INK, COLOR_SILVER);
        tte_set_pos(8, 20); tte_write("Final Telemetry:");
        
        tte_set_color(TTE_INK, COLOR_WHITE);
        tte_set_pos(8, 45); tte_write("Result: "); tte_write(result_victory() ? "SUCCESS" : "DESTROYED");
        
        tte_set_pos(8, 65); tte_write("Score: ");
        snprintf(buffer, sizeof(buffer), "%d", result_score());
        tte_write(buffer);
        
        tte_set_pos(8, 85); tte_write("Cause: "); tte_write(result_reason());
        
        tte_set_pos(8, 120); tte_write("Press A to Title");
        break;
    
    default: 
        break;
   }
}
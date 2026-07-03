#include <tonc.h>
#include "shell.h"
#include "shell_render.h"
#include <stdio.h>
#include "game_data.h"

#define COLOR_WHITE 0X7FFF
#define COLOR_SILVER 0x5EF7
#define COLOR_RED 0xFA68 //coral red
#define COLOR_BLU 0x7C00

void shell_render_engine_init(void) { //Initialize engine and set Mode 3 for GBA
   REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;
   tte_init_bmp_default(3);
}

void shell_render_display(void) { 
   GameState present_state = shell_state(); //GameState from shell.h
   char buffer[100];
   const PlanetData* p_data;
   const AreaData* a_data;
   const LanderData* l_data;
   
   tte_erase_screen();
   
   switch (present_state) {
    case STATE_TITLE:
        tte_set_color(TTE_INK, COLOR_SILVER);
        tte_set_color(TTE_SHADOW, COLOR_WHITE);
        tte_set_pos(75, 80);
        tte_write("LANDER ADVANCE\n\n");

        tte_set_color(TTE_INK, COLOR_WHITE);
        tte_set_pos(69, 100);
        tte_write("Press A to launch");
        break;

    case STATE_CELESTIAL_BODY_SELECTION:
      switch (body_state()){
         case SUB_SHUTTLE_MOVING:
             tte_set_color(TTE_INK, COLOR_WHITE);
             tte_set_pos(33, 50);
             tte_write("Set up the flight trajectory\n\nPress A for Info");
             break;
         
         case SUB_BODY_INFO:
             p_data = planet_data(planet_index());
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(33, 50);
             tte_write("Body Info\n");
             tte_set_color(TTE_INK, COLOR_WHITE);
             snprintf(buffer, sizeof(buffer), "Name: %s \nDescription: %s \nGravity: %d", 
             p_data->planet_name, p_data->planet_description, p_data->gravity);
             tte_write(buffer);
             break;
      }
      break;

    case STATE_AREA_SELECTION:
      switch (area_state()) {
         case SUB_AREA_POINTER_MOVING:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(33, 50);
             tte_write("Predict the landing area\n\nPress A for Info");
             break;

         case SUB_AREA_INFO:
             a_data = area_data(planet_index(), area_index());
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(33, 50);
             tte_write("Landing Area Info\n");
             tte_set_color(TTE_INK, COLOR_WHITE);
             snprintf(buffer, sizeof(buffer), "Name: %s \nDescription: %s \nDifficulty: %d", 
             a_data->area_name, a_data->area_description, a_data->difficulty);
             tte_write(buffer);
             break;
      }
      break;

    case STATE_CONFIG_SELECTION:
        l_data = lander_data(lander_index());
        switch (config_state()) {
          case SUB_CONFIG_POINTER_MOVING:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(69, 45);
             tte_write("Select the lander\n\nPress A for Info");
             break;
             
          case SUB_LANDER_INFO:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(69, 45);
             tte_write("Lander Info\n");
             tte_set_color(TTE_INK, COLOR_WHITE);
             snprintf(buffer, sizeof(buffer), "Name: %s \nDescription: %s \nMax Crew: %d \nMass: %d kg \nThrust: %d kN",
              l_data->lander_name, l_data->lander_description, l_data->max_crew, l_data->mass, l_data->thrust);
             tte_write(buffer);
             break;

          case SUB_CREW_INFO:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(51, 57);
             tte_write("Select the organic payload\n");
             tte_set_color(TTE_INK, COLOR_WHITE);
             snprintf(buffer, sizeof(buffer), "Lander: %s \nCrew Assigned: %d / %d",
             l_data->lander_name, crew_count(), max_crew());
             tte_write(buffer);
             tte_write("\nPress A to launch or B to go back");
             break;
        }
        break;
        
        //For testing purposes only
        case STATE_GAMEPLAY:
          tte_set_color(TTE_INK, COLOR_SILVER);
          tte_set_pos(51, 57);
          tte_write("Gameplay running...\n");
          tte_write("Press START, aka TAB to open Pause Menu");
          tte_write("Press R on pc to simulate WIN condition\n");
          tte_write("Press L on pc to simulate LOSE condition");
          break;


    case STATE_PAUSE:
        tte_set_color(TTE_INK, COLOR_SILVER);
        tte_set_pos(51, 30);
        tte_write("PAUSE\n\n");
        tte_set_color(TTE_INK, COLOR_WHITE);
        
        snprintf(buffer, sizeof(buffer), "%s Resume\n%s Restart\n%s Title\n%s Credits", 
                 pause_index() == 0 ? ">" : " ",
                 pause_index() == 1 ? ">" : " ",
                 pause_index() == 2 ? ">" : " ",
                 pause_index() == 3 ? ">" : " ");
        tte_write(buffer);
        break;

    case STATE_LANDING:
        tte_set_color(TTE_INK, COLOR_BLU);
        tte_set_pos(66, 80);
        tte_write("Landed succesfully\n\nPress A for the final telemetry");
        break;

    case STATE_CRASH:
        tte_set_color(TTE_INK, COLOR_RED);
        tte_set_pos(33, 80);
        tte_write("ERROR! Reprocessing required.\n\nPress A for the final telemetry");
        break;

    case STATE_FIN:
        tte_set_color(TTE_INK, COLOR_SILVER);
        tte_set_pos(72, 40);
        tte_write("Final Telemetry:");
        tte_set_color(TTE_INK, COLOR_WHITE);
        snprintf(buffer, sizeof(buffer), "Result: %s\nScore: %d\nCause: %s\n\nPress A to return to Title", 
        result_victory() ? "Congratulations, but you can do better" : "Reprocessing required",
        result_score(),
        result_reason());
        tte_write(buffer);
        break;
    
    default:
        break;
   }
}
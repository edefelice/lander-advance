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
         tte_write("Set up the flight trajectory");
         break;

         case SUB_BODY_INFO:
         tte_set_color(TTE_INK, COLOR_SILVER);
         tte_set_pos(33, 50);
         tte_write("Body Info");
         switch (planet_index()) {
           case 0:
           break;

           case 1:
           tte_set_color(TTE_INK, COLOR_SILVER);
           tte_set_pos(33, 50);
           tte_write("Not available");
           break;

           case 2:
           tte_set_color(TTE_INK, COLOR_SILVER);
           tte_set_pos(33, 50);
           tte_write("Not available");
           break;

           case 3:
           tte_set_color(TTE_INK, COLOR_SILVER);
           tte_set_pos(33, 50);
           tte_write("Not available");
           break;

           case 4:
           tte_set_color(TTE_INK, COLOR_SILVER);
           tte_set_pos(33, 50);
           tte_write("Not available");
           break;

           case 5:
           tte_set_color(TTE_INK, COLOR_SILVER);
           tte_set_pos(33, 50);
           tte_write("Not available");
           break;

           case 6:
           tte_set_color(TTE_INK, COLOR_SILVER);
           tte_set_pos(33, 50);
           tte_write("Not available");
           break;

           case 7:
           tte_set_color(TTE_INK, COLOR_SILVER);
           tte_set_pos(33, 50);
           tte_write("Not available");
           break;

           case 8:
           tte_set_color(TTE_INK, COLOR_SILVER);
           tte_set_pos(33, 50);
           tte_write("Not available");
           break;

           case 9:
           tte_set_color(TTE_INK, COLOR_SILVER);
           tte_set_pos(33, 50);
           tte_write("Not available");
           break;
         }
         break;
      }
      break;

    case STATE_AREA_SELECTION:
      switch (area_state()) {
         case SUB_AREA_POINTER_MOVING:
         tte_set_color(TTE_INK, COLOR_SILVER);
         tte_set_pos(33, 50);
         tte_write("Predict the landing area");
         break;

         case SUB_AREA_INFO:
         tte_set_color(TTE_INK, COLOR_SILVER);
         tte_set_pos(33, 50);
         tte_write("Landing area info");
         switch (planet_index()){
           case 0:
             switch (area_index()){
               case 0:
               break;

               case 1:
               break;

               case 2:
               break;
             }
             break;

           case 1:
             switch (area_index()){
               case 0:
               break;

               case 1:
               break;

               case 2:
               break;
             }
             break;

           case 2:
             switch (area_index()){
               case 0:
               break;

               case 1:
               break;

               case 2:
               break;
             }
             break;

           case 3:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(33, 50);
             tte_write("Not available");
             break;

           case 4:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(33, 50);
             tte_write("Not available");
             break;

           case 5:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(33, 50);
             tte_write("Not available");
             break;

           case 6:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(33, 50);
             tte_write("Not available");
             break;

           case 7:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(33, 50);
             tte_write("Not available");
             break;

           case 8:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(33, 50);
             tte_write("Not available");
             break;

           case 9:
             tte_set_color(TTE_INK, COLOR_SILVER);
             tte_set_pos(33, 50);
             tte_write("Not available");
             break;
         }
         break;
      }
      break;

    case STATE_CONFIG_SELECTION:
    tte_set_color(TTE_INK, COLOR_WHITE);
    tte_set_pos(69, 45);
    tte_write("Select the lander\n");
    tte_set_pos(51, 57);
    tte_write("and the organic payload");
    break;

    case STATE_LANDING:
    tte_set_color(TTE_INK, COLOR_BLU);
    tte_set_pos(66, 80);
    tte_write("Landed succesfully");
    break;

     case STATE_CRASH:
     tte_set_color(TTE_INK, COLOR_RED);
    tte_set_pos(33, 80);
    tte_write("ERROR! Reprocessing required.");
    break;

     case STATE_FIN:
     tte_set_color(TTE_INK, COLOR_SILVER);
    tte_set_pos(72, 80);
    tte_write("Final Telemetry:");
    break;
    
    default:
    break;
   }
}
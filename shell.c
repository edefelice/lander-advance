#include "shell.h"

//State
static GameState present_state;

//Sub-states of the Macro-states
static BodySubState present_body;
static AreaSubState present_area;
static ConfigSubState present_config;

//For testing only
static int up_pointer = 0; //Page up for selection
static int down_pointer = 0;
static int left_pointer = 0;
static int right_pointer = 0;
static int A_button = 0;
static int fake_planet = 0;
static int fake_area_x = 0;
static int fake_area_y = 0;
static int fake_config_x = 0;
static int fake_config_y = 0;
static int fake_result = 0;

static void main_states_management(void) {  //switching between the states logic
switch (present_state) {
    case STATE_TITLE:
    if (A_button == 1) {
        present_state = STATE_CELESTIAL_BODY_SELECTION;
        A_button = 0;
    }
    break;
   case STATE_CELESTIAL_BODY_SELECTION:
   if (A_button == 1) {
    present_state = STATE_AREA_SELECTION;
    A_button = 0;
   }
   break;
   case STATE_AREA_SELECTION:
   if (A_button == 1) {
    present_state = STATE_CONFIG_SELECTION;
    A_button = 0;
   }
   break;
   case STATE_CONFIG_SELECTION:
   if (A_button == 1){
    present_state = STATE_GAMEPLAY;
    A_button = 0;
   }
   break;
   case STATE_GAMEPLAY:
   if (fake_result == 1) {
    present_state = STATE_LANDING;
    fake_result = 0;
   }
  else if (fake_result == 2) {
    present_state = STATE_CRASH;
    fake_result = 0;
   }
   break;
   case STATE_LANDING:
   if (A_button == 1) {
    present_state = STATE_FIN;
    A_button = 0;
   }
   break;
   case STATE_CRASH:
   if (A_button == 1) {
    present_state = STATE_FIN;
    A_button = 0;
   }
  break;
  case STATE_FIN:
  if (A_button == 1) {
    present_state = STATE_TITLE;
    A_button = 0;
  }
  break;
   }
}

static void sub_states_management(void) { //movement logic for the sub_states
switch (present_state){
    case STATE_CELESTIAL_BODY_SELECTION:
    switch (present_body){
        case SUB_SHUTTLE_MOVING:
         if (left_pointer == 1 && fake_planet > 0) {
        fake_planet--;
    }
    else if (right_pointer == 1 && fake_planet < 9) {
        fake_planet++;
    }
    break;
    }
    break;
    case STATE_AREA_SELECTION:
    switch (present_area) {
        case SUB_AREA_POINTER_MOVING:
        if (left_pointer == 1 && fake_area_x > 0) {
            fake_area_x--;
        }
        else if (right_pointer == 1 && fake_area_x < 20) {
        fake_area_x++;
        }
        else if (up_pointer == 1 && fake_area_y > 0) {
            fake_area_y--;
        }
        else if (down_pointer == 1 && fake_area_y < 20) {
            fake_area_y++;
        }
        break;
    }
    break;
    case STATE_CONFIG_SELECTION:
    switch (present_config) {
        case SUB_CONFIG_POINTER_MOVING:
        if (left_pointer == 1 && fake_config_x > 0) {
            fake_config_x--;
        }
        else if (right_pointer == 1 && fake_config_x < 9){
            fake_config_x++;
        }
        else if (up_pointer == 1 && fake_config_y > 0) {
            fake_config_y--;
        }
        else if (down_pointer == 1 && fake_config_y < 20) {
            fake_config_y++;
        }
        break;
    }
    break;
    case STATE_GAMEPLAY:
    break;
    case STATE_LANDING:
    break;
    case STATE_CRASH:
    break;
    case STATE_FIN:
    break;

}
}

void shell_init(void) { //initialization of the states and sub_states
    present_state = STATE_TITLE;
    present_body = SUB_SHUTTLE_MOVING;
    present_area = SUB_AREA_POINTER_MOVING;
    present_config = SUB_CONFIG_POINTER_MOVING;
    fake_planet = 0;
    fake_area_x = 0;
    fake_area_y = 0;
    fake_config_x = 0;
    fake_config_y = 0;
    fake_result = 0;
    }


    GameState shell_state(void){
        return present_state;
    }
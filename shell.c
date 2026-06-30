#include "shell.h"

//State
static GameState present_state;

//Sub-states of the Macro-states
static BodySubState present_body;
static AreaSubState present_area;
static ConfigSubState present_config;

//Pointers placeholders
static int up_pointer = 0; //Page up for selection
static int down_pointer = 0;
static int left_pointer = 0;
static int right_pointer = 0;
int A_button = 0;          //Removed static for testing
static int B_button = 0;

//variables
static int fake_planet = 0;
static int selected_area = 0;
static int selected_lander = 0;
static int selected_crew = 0;
int fake_result = 0;       //Removed static for testing

//Time management
static int frame_counter = 0;

//Switching between the states logic
void main_states_management(void) {  
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
                present_area = SUB_AREA_INFO;
                A_button = 0;
                frame_counter = 0;
            }
            break;
        case STATE_AREA_SELECTION:
            if (A_button == 1) {
                present_state = STATE_CONFIG_SELECTION;
                present_config = SUB_CONFIG_POINTER_MOVING;
                A_button = 0;
            }
            break;
        case STATE_CONFIG_SELECTION:
            if (A_button == 1){
                if (present_config = SUB_CONFIG_POINTER_MOVING;){
                    present_config = SUB_CREW_INFO;
                    selected_crew = 1;
                 }
                 else if (present_config = SUB_CREW_INFO){
                    present_state = STATE_GAMEPLAY;
                 }
                frame_counter = 0;
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
        default:
            break;
    }
}

//Movement logic for the sub_states
void sub_states_management(void) { 
    switch (present_state){
        case STATE_CELESTIAL_BODY_SELECTION:
            switch (present_body){
                case SUB_SHUTTLE_MOVING:
                    if (left_pointer == 1 && fake_planet > 0) {
                        fake_planet--;
                        frame_counter = 0;
                    }
                    else if (right_pointer == 1 && fake_planet < 9) {
                        fake_planet++;
                        frame_counter = 0;
                    }
                    else {
                        frame_counter++;
                    }
                    if (left_pointer == 0 && right_pointer == 0 && frame_counter == 60 ) {
                        present_body = SUB_BODY_INFO;
                        frame_counter = 0;
                    }
                    break;
                case SUB_BODY_INFO:
                    if (B_button == 1) {
                        present_body = SUB_SHUTTLE_MOVING;
                    }
                    break;
                default:
                    break;
            }
            break;
        case STATE_AREA_SELECTION:
            switch (present_area) {
                case SUB_AREA_POINTER_MOVING:
                     if (up_pointer == 1 && selected_area > 0) {
                        selected_area--;
                        frame_counter = 0;
                    }
                    else if (down_pointer == 1 && selected_area < 2) {
                        selected_area++;
                        frame_counter = 0;
                    }
                    else {
                        frame_counter++;
                    }
                    if (left_pointer == 0 && right_pointer == 0 && up_pointer == 0 && down_pointer == 0 && frame_counter == 60 ) {
                        present_area = SUB_AREA_INFO;
                    }
                    break;
                    case SUB_AREA_INFO:
                    if (B_button == 1) {
                        present_area = SUB_AREA_POINTER_MOVING;
                    }
                default:
                    break;
            }
            break;
        case STATE_CONFIG_SELECTION:
            switch (present_config) {
                case SUB_CONFIG_POINTER_MOVING:
                    if (left_pointer == 1 && selected_lander > 0) {
                        selected_lander--;
                        frame_counter = 0;
                    }
                    else if (right_pointer == 1 && selected_lander < 2){
                        selected_lander++;
                        frame_counter = 0;
                    }
                    else {
                        frame_counter++
                    }
                    if (left_pointer == 0 && right_pointer == 0 && frame_counter == 60) {
                        present_config = SUB_LANDER_INFO;
                    }
                    break;
                    case SUB_LANDER_INFO:
                    if (B_button == 1) {
                        present_config = SUB_CONFIG_POINTER_MOVING;
                    }
                    break;
                    case SUB_CREW_INFO:
                    if (left_pointer == 1 && selected_crew > 2) {
                        selected_crew--;
                    }
                    else if (right_pointer == 1 && selected_crew < max_crew()){
                        selected_crew++;
                    }
                    else if (B_button == 1) {
                        present_config = SUB_CONFIG_POINTER_MOVING;
                        B_button = 0;
                    }
                    break;
            }
        default:
            break;
    }
}

//Initialization of the states and sub_states
void shell_init(void) { 
    present_state = STATE_TITLE;
    present_body = SUB_SHUTTLE_MOVING;
    present_area = SUB_AREA_POINTER_MOVING;
    present_config = SUB_CONFIG_POINTER_MOVING;
    fake_planet = 0;
    selected_area = 0;
    selected_lander = 0;
    selected_crew = 0;
    fake_result = 0;
}

//getter functions
GameState shell_state(void){
    return present_state;
}

BodySubState body_state(void) {
    return present_body;
}

AreaSubState area_state(void){
    return present_area;
}

ConfigSubState config_state(void){
    return present_config;
}

int planet_index(void) {
    return fake_planet;
}

int area_index(void) {
    return selected_area;
}
//Max crew for each lander (pilot + others)
int max_crew(void){
    switch (selected_lander){
        case 0: return 2;
        case 1: return 4;
        case 2: return 6;
        default: return 2;
    }
    
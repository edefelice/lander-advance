#include "shell.h"
#include "game_data.h"
#include "game_result.h"
#include "cockpit.h"
#include <stdbool.h>

//State
static GameState present_state;

//Sub-states of the Macro-states
static BodySubState present_body;
static AreaSubState present_area;
static ConfigSubState present_config;
static PauseSubState present_pause;

//Pointers placeholders
static int up_pointer = 0; //Page up for selection
static int down_pointer = 0;
static int left_pointer = 0;
static int right_pointer = 0;
static int A_button = 0;
//Tracks A_button from the previous frame, avoiding that the key can be held on the 1st gameplay frame
static int A_button_prev = 0;
static int B_button = 0;
static int START_button = 0;

//variables
static int selected_planet = 0;
static int selected_area = 0;
static int selected_lander = 0;
static int selected_crew = 0;
static int selected_pause = 0;
static bool result_pending = false;
static bool selected_night_mode = false;
static bool selected_fast_mode = false;

// Reason descriptor for losing
static const char* const reason_text[] = {
    [GR_REASON_NONE] = "N/A",
    [GR_REASON_VERTICAL_SPEED] = "Excessive vertical speed",
    [GR_REASON_HORIZONTAL_SPEED] = "Excessive horizontal speed",
    [GR_REASON_ANGULAR_SPEED] = "Excessive angular speed",
    [GR_REASON_OUT_OF_PAD] = "Out of pad"
};

/*
//GameResult to be replaced with the official one
struct GameResult {
    int result;
    int score;
    const char* reason;
};
*/

static GameResult present_result;

//Switching between the states logic
void main_states_management(void) {  
    switch (present_state) {
        case STATE_TITLE:
            if (A_button == 1) {
                present_state = STATE_GAME_MODE_SELECTION;
                A_button = 0;
            }
            break;
        case STATE_GAME_MODE_SELECTION:
            if (A_button == 1) {
                present_state = STATE_CELESTIAL_BODY_SELECTION;
                A_button = 0;
            }
            else if (B_button == 1) {
                present_state = STATE_TITLE;
                B_button = 0;
            }
            break;
        case STATE_CELESTIAL_BODY_SELECTION:
            if (A_button == 1 && present_body == SUB_BODY_INFO) {
                present_body = SUB_MODE_SELECTION;
                A_button = 0;
            }
            else if (A_button == 1 && present_body == SUB_MODE_SELECTION) {
                present_state = STATE_AREA_SELECTION;
                present_area = SUB_AREA_POINTER_MOVING;
                A_button = 0;
            }
            break;
        case STATE_AREA_SELECTION:
            if (A_button == 1 && present_area == SUB_AREA_INFO) {
                present_state = STATE_CONFIG_SELECTION;
                present_config = SUB_CONFIG_POINTER_MOVING;
                A_button = 0;
            }
            break;
        case STATE_CONFIG_SELECTION:
            if (A_button_prev == 1 && A_button == 0){
                if (present_config == SUB_LANDER_INFO){
                    present_config = SUB_CREW_INFO;
                    selected_crew = 2; //Starting value also identified as minimum value (pilot + 1 eqip)
                    A_button = 0;
                }
                else if (present_config == SUB_CREW_INFO){
                    present_state = STATE_GAMEPLAY;
                    A_button = 0;
                }
            }
            break;
        case STATE_GAMEPLAY:
            if (START_button == 1){
                present_state = STATE_PAUSE;
                present_pause = SUB_RESUME;
                selected_pause = 0;
                START_button = 0;
            }
            
            if (result_pending == true) {
                present_state = (present_result.outcome == GR_WIN) ? STATE_LANDING : STATE_CRASH;
                result_pending = false;
            }
            break;
        case STATE_PAUSE:
            if (A_button_prev == 1 && A_button == 0){
                switch (present_pause){
                    case SUB_RESUME:
                        present_state = STATE_GAMEPLAY;
                        break;
                    case SUB_RESTART:
                        present_state = STATE_GAMEPLAY;
                        present_result.outcome = GR_LOSE;
                        present_result.score = 0;
                        present_result.reason = GR_REASON_NONE;                        
                        break;
                    case SUB_TITLE:
                        shell_init();
                        present_state = STATE_TITLE;
                        break;
                    case SUB_CREDITS:
                        present_pause = SUB_SHOW_CREDITS;
                        A_button = 0;
                        break;
                    case SUB_SHOW_CREDITS:
                        break;
                }
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
                shell_init();
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
        case STATE_GAME_MODE_SELECTION:
            if (up_pointer == 1 || down_pointer == 1 || left_pointer == 1 || right_pointer == 1) {
                selected_fast_mode = !selected_fast_mode;
            }
            break;
        case STATE_CELESTIAL_BODY_SELECTION:
            switch (present_body){
                case SUB_SHUTTLE_MOVING:
                    if (left_pointer == 1 && selected_planet > 0) {
                        selected_planet--;
                    }
                    else if (right_pointer == 1 && selected_planet < 9) {
                        selected_planet++;
                    }
                    else if (A_button == 1) {
                        present_body = SUB_BODY_INFO;
                        A_button = 0;
                    }
                    else if (B_button == 1){
                        present_state = STATE_GAME_MODE_SELECTION;
                        B_button = 0;
                    }
                    break;
                case SUB_BODY_INFO:
                    if (B_button == 1) {
                        present_body = SUB_SHUTTLE_MOVING;
                        B_button = 0;
                    }
                    break;
                case SUB_MODE_SELECTION:
                    if (up_pointer == 1 || down_pointer == 1 || left_pointer == 1 || right_pointer == 1) {
                        selected_night_mode = !selected_night_mode;
                    }
                    else if (B_button == 1) {
                        present_body = SUB_BODY_INFO;
                        B_button = 0;
                    }
                    break;
            }
            break;
        case STATE_AREA_SELECTION:
            switch (present_area) {
                case SUB_AREA_POINTER_MOVING:
                    if (up_pointer == 1 && selected_area > 0) {
                        selected_area--;
                    }
                    else if (down_pointer == 1 && selected_area < 2) {
                        selected_area++;
                    }
                    else if (A_button == 1) {
                        present_area = SUB_AREA_INFO;
                        A_button = 0;
                    }
                    else if (B_button == 1) {
                        present_state = STATE_CELESTIAL_BODY_SELECTION;
                        present_body = SUB_MODE_SELECTION;
                        B_button = 0;
                    }
                    break;
                case SUB_AREA_INFO:
                    if (B_button == 1) {
                        present_area = SUB_AREA_POINTER_MOVING;
                        B_button = 0;
                    }
                    break;
            }
            break;
        case STATE_CONFIG_SELECTION:
            switch (present_config) {
                case SUB_CONFIG_POINTER_MOVING:
                    if (left_pointer == 1 && selected_lander > 0) {
                        selected_lander--;
                    }
                    else if (right_pointer == 1 && selected_lander < 1){ //based on the number of the landers
                        selected_lander++;
                    }
                    else if (A_button == 1) {
                        present_config = SUB_LANDER_INFO;
                        A_button = 0;
                    }
                    else if (B_button == 1) {
                        present_state = STATE_AREA_SELECTION;
                        present_area = SUB_AREA_POINTER_MOVING;
                        B_button = 0;
                    }
                    break;
                case SUB_LANDER_INFO:
                    if (B_button == 1) {
                        present_config = SUB_CONFIG_POINTER_MOVING;
                        B_button = 0;
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
                        present_config = SUB_LANDER_INFO;
                        B_button = 0;
                    }
                    break;
            }
            break;
        case STATE_PAUSE:
            if (present_pause == SUB_SHOW_CREDITS) {
                if (A_button == 1 || B_button == 1) {
                    present_pause = SUB_CREDITS;
                    selected_pause = 3;
                    A_button = 0;
                    B_button = 0;
                }
            }
            else {
                if (up_pointer == 1 && selected_pause > 0){
                    selected_pause--;
                }
                else if (down_pointer == 1 && selected_pause < 3){
                    selected_pause++;
                }
                present_pause = (PauseSubState)selected_pause;
            }
            break;
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
    selected_planet = 0;
    selected_area = 0;
    selected_lander = 0;
    selected_crew = 0;
    selected_night_mode = false;
    selected_fast_mode = false;

    //reset results
    present_result.outcome = GR_LOSE;
    present_result.score = 0;
    present_result.reason = GR_REASON_NONE;

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

PauseSubState pause_state(void) {
    return present_pause;
}

int planet_index(void) {
    return selected_planet;
}

int area_index(void) {
    return selected_area;
}

//Max crew for each lander (pilot + others)
int max_crew(void){
    return lander_data(selected_lander)->max_crew;
}

int lander_index(void){
    return selected_lander;
}

int crew_count(void) {
    return selected_crew;
}

int pause_index(void) {
    return selected_pause;
}

int result_victory(void) {
    return present_result.outcome == 0;
}

int result_score(void) {
    return present_result.score;
}

const char* result_reason(void) {
    return reason_text[present_result.reason];
}

bool is_night_mode(void) {
    return selected_night_mode;
}

bool is_fast_mode(void) {
    return selected_fast_mode;
}

void shell_feed_input(u16 action) {
    A_button = 0;
    B_button = 0;
    up_pointer = 0;
    right_pointer = 0;
    down_pointer = 0;
    left_pointer = 0;
    START_button = 0;

    if (action & M_CONFIRM) A_button = 1;
    if (action & M_RETURN)  B_button = 1;
    if (action & M_UP)      up_pointer = 1;
    if (action & M_DOWN)    down_pointer = 1;
    if (action & M_LEFT)    left_pointer = 1;
    if (action & M_RIGHT)   right_pointer = 1;
    if (action & PAUSE)     START_button = 1;
}

void shell_submit_result(const GameResult *result) {
    present_result = *result;
    result_pending = true;
}

void shell_commit_input(void) {
    A_button_prev = A_button;
}
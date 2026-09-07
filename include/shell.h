#ifndef SHELL_H
#define SHELL_H

//States enum
#include "game_result.h"
#include <tonc.h>

typedef enum {
    STATE_TITLE,
    STATE_CELESTIAL_BODY_SELECTION, //Macro-state
    STATE_AREA_SELECTION, //Macro-state
    STATE_CONFIG_SELECTION, //Macro-state
    STATE_GAMEPLAY, //The actual gameplay
    STATE_PAUSE, //Pause
    STATE_LANDING, //Win state
    STATE_CRASH, //Game Over - Lost
    STATE_FIN //End + Results
} GameState; 

//Sub-states of STATE_CELESTIAL_BODY_SELECTION
typedef enum {
    SUB_SHUTTLE_MOVING,
    SUB_BODY_INFO
} BodySubState;

//Sub-states of STATE_AREA_SELECTION
typedef enum {
    SUB_AREA_POINTER_MOVING,
    SUB_AREA_INFO
} AreaSubState;

//Sub-states of STATE_CONFIG_SELECTION
typedef enum {
    SUB_CONFIG_POINTER_MOVING,
    SUB_LANDER_INFO,
    SUB_CREW_INFO
} ConfigSubState;

typedef enum {
    SUB_RESUME,
    SUB_RESTART,
    SUB_TITLE,
    SUB_CREDITS,
    SUB_SHOW_CREDITS
} PauseSubState;

//getter functions
GameState shell_state(void);
BodySubState body_state(void);
AreaSubState area_state(void);
ConfigSubState config_state(void);
PauseSubState pause_state(void);
int planet_index(void);
int area_index(void);
int lander_index(void);
int crew_count(void);
int max_crew(void);
int pause_index(void);
int result_victory(void);
int result_score(void);
const char* result_reason(void);

void shell_init(void);
void main_states_management(void);
void sub_states_management(void);

//Reads input
void shell_feed_input(u16 action);
//Reads GameResult to print the result on screen
void shell_submit_result(const GameResult *result);
/*
  Must be called after main_states_management()/sub_states_management()
  in the main loop, so A_button_prev reflects the previous frame's
  value when the state machine reads it, not the current one.
*/
void shell_commit_input(void);

#endif
#ifndef SHELL_H
#define SHELL_H

//States enum
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

//getter functions
GameState shell_state(void);
BodySubState body_state(void);
AreaSubState area_state(void);
ConfigSubState config_state(void);
int planet_index(void);
int area_index(void);
int lander_index(void);
int crew_count(void);
int max_crew(void);

void shell_init(void);
void main_states_management(void);
void sub_states_management(void);

#endif
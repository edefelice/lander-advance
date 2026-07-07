#ifndef GAME_DATA_H
#define GAME_DATA_H

typedef struct {
    char gravity[10];
    char starting_altitude[10];
    char planet_name[20];      //fixed array included directly in the struct instead of the pointers
    char planet_description[40]; 
} PlanetData;

typedef struct {
    int difficulty;
    char area_name[20];      
    char area_description[40];   
} AreaData;

typedef struct {
    int max_crew;
    int mass;
    int fuel;
    int thrust_main;
    int thrust_rcs;
    char lander_name[20];        
    char lander_description[20]; 
} LanderData;

//getter functions
const PlanetData* planet_data(int index);
const AreaData* area_data(int planet_index, int area_index);
const LanderData* lander_data(int index);

#endif
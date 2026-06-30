#ifndef GAME_DATA_H
#define GAME_DATA_H

typedef struct {
    const char* planet_name;
    const char* planet_description;
    int gravity; // gravity value for the planet
} PlanetData;

typedef struct {
    const char* area_name;
    const char* area_description;
    int difficulty; //difficulty value for the area
} AreaData;

typedef struct {
    const char* lander_name;
    const char* lander_description;
    int max_crew;
    int mass;
    int thrust;
} LanderData;

//getter functions
const PlanetData* planet_data(int index);
const AreaData* area_data(int planet_index, int area_index);
const LanderData* lander_data(int index);

#endif

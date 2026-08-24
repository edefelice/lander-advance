#include "game_data.h"
#include "fixedpoint32.h"
#include "physics_constants.h"

//Planets Database
static const PlanetData planets[1] = { //[] is the number of planets in the database
    {(MOON_G >> FIX_SHIFT32), ((MOON_G & 0xFFFF) * 100) >> 16, FIX_TO_INT(MOON_H), "Earth's Moon", "Regolith\n        corrosion"} //the first one is the gravity integer value
//add more planets
};

//Areas Database
static const AreaData areas [1][3] = { //the first [] is the number of planets in the database, the second [] is the number of areas for each planet
//areas for planet 0
    {
        {3, "Moon area 1", "Crater,\n        dangerous"}, //the first number is the difficulty value
        {1, "Moon area 2", "Flat surface,\n        easy"},
        {4, "Moon area 3", "Peak, dangerous"}   //the first number is the difficulty value
    }
//add more planets + areas
};

//Landers Database
static const LanderData landers[1] = { //[] number of landers in database
    {2, FIX_TO_INT(DRY_MASS), FIX_TO_INT(PROP_MASS), FIX_TO_INT(MAIN_THRUST), FIX_TO_INT(RCS_THRUST), "Baffo", "Big"} //first number is the max crew, second is the mass, third is the thrust
//add more landers
};

//Getter
const PlanetData* planet_data(int index) {
    if (index < 0 || index >= 1) index = 0; //modify when adding new planets
    return &planets[index];
}

const AreaData* area_data(int planet_index, int area_index) {
    if (planet_index < 0 || planet_index >= 1) planet_index = 0; //modify when adding new planets
    if (area_index < 0 || area_index > 2) area_index = 0;
    return &areas[planet_index][area_index];
}

const LanderData* lander_data(int index) {
    if (index < 0 || index > 0) index = 0; //modify when adding more landers
    return &landers[index];
}
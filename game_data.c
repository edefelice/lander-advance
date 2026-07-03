#include "game_data.h"

//Planets Database
static const PlanetData planets[1] = { //[] is the number of planets in the database
    {"Earth's Moon", "Regolith corrosion", 333} //the last one is the gravity value to be modified
//add more planets
};

//Areas Database
static const AreaData areas [1][3] = { //the first [] is the number of planets in the database, the second [] is the number of areas for each planet
//areas for planet 0
    {
        {"Moon area 1", "Crater, dangerous", 3}, //the last number is the difficulty value
        {"Moon area 2", "Flat surface, easy", 1},
        {"Moon area 3", "Peak, highly dangerous", 4}
    }
//add more planets + areas
};

//Landers Database
static const LanderData landers[1] = { //[] number of landers in database
    {"Baffo", "Big", 2, 333, 9999} //first number is the max crew, second is the mass, last one is the thrust
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
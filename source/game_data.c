#include "game_data.h"

//Planets Database
static const PlanetData planets[1] = { //[] is the number of planets in the database
    {"1.62", "3000", "Earth's Moon", "Regolith corrosion"} //the first one is the gravity value
//add more planets
};

//Areas Database
static const AreaData areas [1][3] = { //the first [] is the number of planets in the database, the second [] is the number of areas for each planet
//areas for planet 0
    {
        {3, "Moon area 1", "Crater, dangerous"}, //the first number is the difficulty value
        {1, "Moon area 2", "Flat surface, easy"},
        {4, "Moon area 3", "Peak, dangerous"}   //the first number is the difficulty value
    }
//add more planets + areas
};

//Landers Database
static const LanderData landers[1] = { //[] number of landers in database
    {2, 6713, 8487, 30000, 440, "Baffo", "Big"} //first number is the max crew, second is the mass, third is the thrust
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
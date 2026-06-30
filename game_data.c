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
        {"Moon area 1", "Crater, dangerous", 1} //the last number is the difficulty value
        //add more areas
    }
//add more planets + areas
};

//Landers Database
static const LanderData landers[1] = { //[] number of landers in database
    {"Baffo", "Big", "2", "333", "9999"} //first number is the max crew, second is the mass, last one is the thrust
//add more landers
};
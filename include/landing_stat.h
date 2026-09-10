#ifndef LANDING_STAT_H  
#define LANDING_STAT_H 

#include "gameplay.h"
#include "landing_area.h"

void IsonPad(const Lander *lander, int area_idx, const Sites *moon_sites, bool *pad_status, fixed *distance_sqr);

#endif
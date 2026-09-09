#ifndef LANDING_STAT_H  
#define LANDING_STAT_H 

#include "gameplay.h"
#include "landing_area.h"

extern int active_area_idx;


static inline int get_active_area_idx(void) { return active_area_idx; };

bool IsonPad(const Lander *lander, int area, const Sites *moon_sites);

#endif
#ifndef AFFINE_BACKGROUND_H
#define AFFINE_BACKGROUND_H

#include "gameplay.h"

/*
    lander_to_affine_src(...)
    Maps lander state (position, altitude, attitude) to the affine parameters (AFF_SRC_EX)
    that describe the lunar surface as seen through the cockpit.
*/
void lander_to_affine_src(const Lander *lander, AFF_SRC_EX *src);

/*
    map_swap(...)
    swap map to the near version if the player reaches an height < SWAP_HEIGHT or > SWAP_EXIT.
*/
void map_swap(const Lander *lander);

int get_active_area_idx(void);

#endif // AFFINE_BACKGROUND_H
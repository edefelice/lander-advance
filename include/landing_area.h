#ifndef LANDING_AREA_H
#define LANDING_AREA_H

#include "fixedpoint32.h"

#define AREA_LIMIT 219 // Metre
//#define ZONE_LIMIT (fixed)0x447AE   // Landing Zone Radius [Metre] (4.28m)
#define N_ZONES 22 // Number of landing zones
#define NUM_AREAS 3


typedef struct {
    // Metre from map centre
    fixed min_x;
    fixed max_x;
    fixed min_y;
    fixed max_y;
    fixed centre_x;
    fixed centre_y;
    const unsigned int *tiles;
    unsigned int tiles_len;
    const unsigned short *map;
    unsigned int map_len;
    const unsigned short *pal;
    unsigned int pal_len;
} LandingArea;

typedef struct {
    // Metre from map centre
    fixed centre_x;
    fixed centre_y;
} LandingZone;

typedef struct {
    // Landing Sites
    LandingZone site[N_ZONES];
} Sites;

extern const LandingArea AREAS[];
extern const Sites moon_sites[];

#endif
#ifndef LANDING_AREA_H
#define LANDING_AREA_H

#include "fixedpoint32.h"
#include "graphics/moon_site1_v1.h"
#include "graphics/moon_site2_v1.h"
#include "graphics/moon_site3_v1.h"

#define AREA_LIMIT 219 // Metre

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

static const LandingArea AREAS[] = {
    // Area 1
    {-FIX_FROM_INT(1316),
        -FIX_FROM_INT(1316 - AREA_LIMIT),
        FIX_FROM_INT(2194 - AREA_LIMIT),
        FIX_FROM_INT(2194),
        -FIX_FROM_INT(1316 - (AREA_LIMIT>>1)),
        FIX_FROM_INT(2194 - (AREA_LIMIT>>1)),
        moon_site1_v1Tiles,
        moon_site1_v1TilesLen,
        moon_site1_v1Map,
        moon_site1_v1MapLen,
        moon_site1_v1Pal,
        moon_site1_v1PalLen},
        // Area 2
    {-FIX_FROM_INT(1536),
        -FIX_FROM_INT(1536 - AREA_LIMIT),
        FIX_FROM_INT(1097 - AREA_LIMIT),
        FIX_FROM_INT(1097),
        -FIX_FROM_INT(1536 - (AREA_LIMIT>>1)),
        FIX_FROM_INT(1097 - (AREA_LIMIT>>1)),
        moon_site2_v1Tiles,
        moon_site2_v1TilesLen,
        moon_site2_v1Map,
        moon_site2_v1MapLen,
        moon_site2_v1Pal,
        moon_site2_v1PalLen},
        // Area 3
    {FIX_FROM_INT(2303),
        FIX_FROM_INT(2303 + AREA_LIMIT),
        -FIX_FROM_INT(1974 + AREA_LIMIT),
        -FIX_FROM_INT(1974),
        FIX_FROM_INT(2303 + (AREA_LIMIT>>1)),
        -FIX_FROM_INT(1974 + (AREA_LIMIT>>1)),
        moon_site3_v1Tiles,
        moon_site3_v1TilesLen,
        moon_site3_v1Map,
        moon_site3_v1MapLen,
        moon_site3_v1Pal,
        moon_site3_v1PalLen}
};

#define NUM_AREAS (sizeof(AREAS) / sizeof(AREAS[0]))

#endif
#include "landing_stat.h"
#include "gameplay.h"
#include "fixedpoint32.h"
#include "physics_constants.h"
#include "tonc_math.h"
#include "tonc_types.h"
#include "landing_area.h"


int active_area_idx = -1;



bool IsonPad(const Lander *lander, int area, const Sites *moon_sites) {
    if (area < 0 || lander == NULL || moon_sites == NULL) {
        return 0;
    }

    const fixed r2 = fixMul(ZONE_LIMIT, ZONE_LIMIT);
    const Sites *current = &moon_sites[area];

    for (int i = 0; i < N_ZONES; i++) {

        fixed dx = current->site[i].centre_x - lander->x;
        if(dx > ZONE_LIMIT || dx < -ZONE_LIMIT){
            continue;
        }

        fixed dy = current->site[i].centre_y - lander->y;
        if(dy > ZONE_LIMIT || dy < -ZONE_LIMIT){
            continue;
        }

        fixed distance = fixMul(dx, dx) + fixMul(dy, dy);

        if (distance < r2) {
            return 1;  
        }
    }

    return 0;
}
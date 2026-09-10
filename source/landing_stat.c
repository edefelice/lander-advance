#include "landing_stat.h"
#include "gameplay.h"
#include "fixedpoint32.h"
#include "landing_area.h"
#include "physics_constants.h"


void IsonPad(const Lander *lander, int area_idx, const Sites *moon_sites, bool *pad_status, fixed *distance_sqr) {
    

    if (area_idx < 0 || lander == NULL || moon_sites == NULL || pad_status == NULL || distance_sqr == NULL ) {
        if (pad_status != NULL) {
            *pad_status = 0;
        }
        if (distance_sqr != NULL) {
            *distance_sqr = 0;
        }
        return;
    }

    const Sites *current = &moon_sites[area_idx];

    for (int i = 0; i < N_ZONES; i++) {

        fixed dx = current->site[i].centre_x - lander->x;
        if(dx > ZONE_LIMIT || dx < -ZONE_LIMIT){
            continue;
        }

        fixed dy = current->site[i].centre_y - lander->y;
        if(dy > ZONE_LIMIT || dy < -ZONE_LIMIT){
            continue;
        }

        fixed d_sqr = fixMul(dx, dx) + fixMul(dy, dy);

        if (d_sqr < PAD_R2) {
            *pad_status = 1;
            *distance_sqr = d_sqr;
            return;  
        }
    }

    *pad_status = 0;
    *distance_sqr = 0;
}
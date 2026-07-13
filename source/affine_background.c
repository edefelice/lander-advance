#include <tonc.h>
#include "affine_background.h"
#include "fixedpoint32.h"
#include "physics_constants.h"

#define MIN_SHRINK 0x80
#define MAX_SHRINK 0x180
#define MAX_HEIGHT 5000 // TODO: should come from scenario start altitude (Pierluca W7)
#define PIVOT_X (SCREEN_WIDTH / 2)
#define PIVOT_Y (SCREEN_HEIGHT / 2)
#define TEXEL_PER_METRE 13 // TODO: Compute actual value that chanfes with height
#define MAP_SIZE 512 // moon_far is 64x64 tiles = 512px; TODO: derive from scenario when maps vary
#define MARGIN 50 //pixels
#define WORLD_LOW 0
#define WORLD_LIMIT ((MAP_SIZE - MARGIN) << 8) // upper limit for the map clamp interval [0, limit)

/*
    shrink(...)
    Calculates shrinking factor
    Input: min value, max value, current height, maximum height
    Output: shrinking factor
*/
static inline int shrink(const int min, const int max, const int height, const int max_height) {
    return (height >= 0 && height <= max_height) ? min + (max - min) * height / max_height : max;
}

void lander_to_affine_src(const Lander *lander, AFF_SRC_EX *src) {
    // Initialize source affine matrix
    src->scr_x = PIVOT_X;
    src->scr_y = PIVOT_Y;
    src->sx = shrink(MIN_SHRINK, MAX_SHRINK, FIX_TO_INT(lander->z), MAX_HEIGHT);
    src->sy = src->sx;
    // Pan
    // Shift by 8 bits because the affine matrix uses 8.8 representation.
    // Multiplies TEXEL_PER_METRE to convert: metre->texel
    
    int pan_x = (lander->x >> 8) * TEXEL_PER_METRE;
    pan_x = clamp(pan_x, WORLD_LOW, WORLD_LIMIT);
    src->tex_x = pan_x;
    int pan_y = (-lander->y >> 8) * TEXEL_PER_METRE;
    pan_y = clamp(pan_y, WORLD_LOW, WORLD_LIMIT);
    src->tex_y = pan_y;
    // Angle
    src->alpha = (u16)fixDiv(lander->theta, FIX_TWO_PI);
}
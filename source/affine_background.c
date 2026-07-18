#include <tonc.h>
#include "affine_background.h"
#include "fixedpoint32.h"
#include "physics_constants.h"

#define MAX_SHRINK 0x190
#define MAX_HEIGHT 3000 // TODO: eventually change in tuning (Pierluca W7)
#define SWAP_HEIGHT 95 // Metre
#define MM_PER_TEXEL_FAR    13710   // 512px  → 7020 m
#define MM_PER_TEXEL_NEAR     217   // 1024px →  222 m
#define PIVOT_X 103 // Pivot x-coordinate on screen at half porthole width
#define PIVOT_Y (SCREEN_HEIGHT) // Pivot y-coordinate on screen
#define MAP_SIZE 512 // moon_far is 64x64 tiles = 512px; TODO: derive from scenario when maps vary
#define MAX_RADIUS 163 // sqrt((PORTHOLE_W/2)² + PORTHOLE_H²), pivot at bottom centre

/*
    shrink(...)
    Calculates shrinking factor
    Input: max value, current height, maximum height
    Output: shrinking factor
*/
static inline fixed shrink(const fixed max, const fixed height, const fixed max_height) {
    return fixMul(fixDiv(height + FIX_FROM_INT(3), max_height + FIX_FROM_INT(3)), max);
}

void lander_to_affine_src(const Lander *lander, AFF_SRC_EX *src) {
    // Initialize source affine matrix
    src->scr_x = PIVOT_X;
    src->scr_y = PIVOT_Y;
    // MAX_SHRINK is 8.8; << 8 promotes to Q16.16 for the ramp, >> 8 returns to 8.8 for the PPU
    fixed z_cam = clamp(lander->z, 0, FIX_FROM_INT(MAX_HEIGHT) + 1);
    src->sx = shrink((fixed)(MAX_SHRINK << 8), z_cam, FIX_FROM_INT(MAX_HEIGHT)) >> 8;
    src->sy = src->sx;
    // Clamp limits TODO: evaluate wether to keep the "radius law" or simplify it
    int lo = MAX_RADIUS * src->sx;
    int hi = (MAP_SIZE << 8) - lo;
    // Pan
    // Shift by 8 bits because the affine matrix uses 8.8 representation.
    // Divides by MM_PER_TEXEL_FAR to convert: metre->texel
    int pan_x = ((lander->x >> 8) * 1000) / MM_PER_TEXEL_FAR;
    int tex_x = ((MAP_SIZE / 2) << 8) + pan_x;
    tex_x = clamp(tex_x, lo, hi + 1);
    src->tex_x = tex_x;
    int pan_y = ((-lander->y >> 8) * 1000) / MM_PER_TEXEL_FAR;
    int tex_y = ((MAP_SIZE / 2) << 8) + pan_y;
    tex_y = clamp(tex_y, lo, hi + 1);
    src->tex_y = tex_y;
    // Rotation angle
    src->alpha = (u16)fixDiv(lander->theta, FIX_TWO_PI);
}
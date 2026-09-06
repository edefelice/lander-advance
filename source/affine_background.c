#include <stdbool.h>
#include <stdint.h>
#include <tonc.h>
#include "affine_background.h"
#include "fixedpoint32.h"
#include "physics_constants.h"
#include "landing_area.h"
#include "graphics/moon_far_v3.h"

#define MAX_SHRINK 0x190
#define MAX_HEIGHT_FAR 3000 // TODO: eventually change in tuning (Pierluca W7)
#define MAX_HEIGHT_NEAR 200 // TODO: eventually change in tuning (Pierluca W7)
#define SWAP_HEIGHT 200 // Metre TODO: eventually change in tuning (Pierluca W7)
#define SWAP_MARGIN 25  // Metre TODO: eventually change in tuning (Pierluca W7)
#define SWAP_EXIT ((SWAP_HEIGHT) + (SWAP_MARGIN))
#define MM_PER_TEXEL_FAR 13710   // 512px  → 7020 m
#define MM_PER_TEXEL_NEAR 214// 219m //217   // 1024px →  222 m
#define PIVOT_X 103 // Pivot x-coordinate on screen at half porthole width
#define PIVOT_Y (SCREEN_HEIGHT) // Pivot y-coordinate on screen
#define MAP_SIZE_FAR 512 // moon_far is 64x64 tiles = 512px; TODO: derive from scenario when maps vary
#define MAP_SIZE_NEAR 1024 // moon_site1 is 128x128 tiles = 1024px
#define MAX_RADIUS 137 // porthole height + bottom frame

static bool is_near = false;
static int active_area_idx = -1;

/*
    shrink(...)
    Calculates shrinking factor
    Input: max value, current height, maximum height
    Output: shrinking factor
*/
static inline fixed shrink(const fixed max, const fixed height, const fixed max_height) {
    return fixMul(fixDiv(height + FIX_FROM_INT(3), max_height + FIX_FROM_INT(3)), max);
}

static inline int get_active_area_idx(void) { return active_area_idx; };

void lander_to_affine_src(const Lander *lander, AFF_SRC_EX *src) {
    int idx = get_active_area_idx();
    int map_size = is_near ? MAP_SIZE_NEAR : MAP_SIZE_FAR;
    int mm_per_texel = is_near ? MM_PER_TEXEL_NEAR : MM_PER_TEXEL_FAR;
    int area_centre_x = is_near ? AREAS[idx].centre_x : 0;
    int area_centre_y = is_near ? AREAS[idx].centre_y : 0;
    int max_height = is_near ? MAX_HEIGHT_NEAR : MAX_HEIGHT_FAR;
    // Initialize source affine matrix
    src->scr_x = PIVOT_X;
    src->scr_y = PIVOT_Y;
    // MAX_SHRINK is 8.8; << 8 promotes to Q16.16 for the ramp, >> 8 returns to 8.8 for the PPU
    fixed z_cam = clamp(lander->z, 0, FIX_FROM_INT(max_height) + 1);
    src->sx = shrink((fixed)(MAX_SHRINK << 8), z_cam, FIX_FROM_INT(max_height)) >> 8;
    src->sy = src->sx;
    // Clamp limits TODO: evaluate wether to keep the "radius law" or simplify it
    int lo = MAX_RADIUS * src->sx;
    int hi = (map_size << 8) - lo;
    // Pan
    // Shift by 8 bits because the affine matrix uses 8.8 representation.
    // Divides by mm_per_texel to convert: metre->texel
    int pan_x = (((lander->x - area_centre_x) >> 8) * 1000) / mm_per_texel;
    int tex_x = ((map_size / 2) << 8) + pan_x;
    tex_x = is_near ? tex_x : clamp(tex_x, lo, hi + 1);
    src->tex_x = tex_x;
    int pan_y = ((-(lander->y - area_centre_y) >> 8) * 1000) / mm_per_texel;
    int tex_y = ((map_size / 2) << 8) + pan_y;
    tex_y = is_near ? tex_y : clamp(tex_y, lo, hi + 1);
    src->tex_y = tex_y;
    // Rotation angle
    src->alpha = (u16)fixDiv(lander->theta, FIX_TWO_PI);
}

void map_swap(const Lander *lander) {

    if (!is_near && lander->z < FIX_FROM_INT(SWAP_HEIGHT)) {
        for(int i = 0; i < NUM_AREAS; i++) {
            if (lander->x > AREAS[i].min_x && lander->x < AREAS[i].max_x
                && lander->y > AREAS[i].min_y && lander->y < AREAS[i].max_y) {
                    is_near = true;
                    active_area_idx = i;
                    // Load background tiles in CBB0
                    memcpy32(tile8_mem[0], AREAS[i].tiles, AREAS[i].tiles_len / 4);
                    // Load background tilemap in SBB 24
                    memcpy16(se_mem[24], AREAS[i].map, AREAS[i].map_len / 2);
                    // Load background palette
                    memcpy16(pal_bg_mem, AREAS[i].pal, AREAS[i].pal_len / 2 - 3); // first 13 colours
                    REG_BG2CNT = (REG_BG2CNT & ~(BG_SIZE_MASK | BG_SBB_MASK | BG_WRAP)) | BG_AFF_128x128 | BG_SBB(24);
                    break;
            }
        }
    }
    else if (lander->z > FIX_FROM_INT(SWAP_EXIT) && is_near) {
        is_near = false;
        // Load background tiles in CBB0
        memcpy32(tile8_mem[0], moon_far_v3Tiles, moon_far_v3TilesLen / 4);
        // Load background tilemap in SBB 28
        memcpy16(se_mem[28], moon_far_v3Map, moon_far_v3MapLen / 2);
        // Load background palette
        memcpy16(pal_bg_mem, moon_far_v3Pal, moon_far_v3PalLen / 2 - 3); // first 13 colours
        REG_BG2CNT = (REG_BG2CNT & ~(BG_SIZE_MASK | BG_SBB_MASK)) | BG_AFF_64x64 | BG_WRAP | BG_SBB(28);
    }
}
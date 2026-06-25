#include <tonc.h>
#include "graphics/placeholder.h"
#include "tonc_memdef.h"

BG_AFFINE affine_bg;
AFF_SRC_EX affine_src;

// Calculates shrinking factor
// Input: min value, max value, current height, maximum height
// Output: shrinking factor
static inline int shrink(const int min, const int max, const int height, const int max_height) {
    return (height >= 0 && height <= max_height) ? min + (max - min) * height / max_height : max;
}

int main(void) {
    // Initialize source affine matrix
    affine_src.tex_x = 256<<8;
    affine_src.tex_y = 256<<8;
    affine_src.scr_x = 240 / 2;
    affine_src.scr_y = 160 / 2;
    affine_src.sx = 0x100;
    affine_src.sy = 0x100;
    affine_src.alpha = 0;
    // Parameters to test background scaling
    int height = 0;
    int position_x = 0;
    int position_y = 0;
    int direction = 1;
    int pan_direction = 1;
    //int height = max_height;
    int min_shrink = 0x80;
    int max_shrink = 0x180;
    int max_height = 1024;
    // Load background tiles in CBB0
    memcpy16(tile8_mem[0], placeholderTiles, placeholderTilesLen / 2);
    // Load background tilemap in SBB 28
    memcpy16(se_mem[28], placeholderMap, placeholderMapLen / 2);
    // Load background palette
    memcpy16(pal_bg_mem, placeholderPal, placeholderPalLen / 2);
    // Configure BG2 with wrap on
    REG_BG2CNT = BG_CBB(0) | BG_SBB(28) | BG_AFF_64x64 | BG_WRAP;
    // Set affine background (Mode 1, BG2)
    REG_DISPCNT = DCNT_MODE(1) | DCNT_BG2;
    irq_init(NULL);
    irq_add(II_VBLANK, NULL);
    while(1) {
        // Configure BG Affine 2
        bg_rotscale_ex(&affine_bg, &affine_src);
        VBlankIntrWait();
        height += direction;
        position_x += pan_direction<<8;
        position_y += pan_direction<<8;
        if (height >= max_height) {
            direction = -1;
        }
        else if (height <= 0) {
            height = 0;
            direction = 1;
        }
        affine_src.tex_x = position_x;
        //affine_src.tex_y = position_y;
        //affine_src.sx = shrink(min_shrink, max_shrink, height, max_height);
        //affine_src.sy = affine_src.sx;
        //affine_src.alpha += 0x100;
        REG_BG_AFFINE[2] = affine_bg;
    }
}
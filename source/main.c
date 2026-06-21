#include <tonc.h>
#include "graphics/placeholder.h"

BG_AFFINE affine_bg;

int main(void) {
    // Load background tiles in CBB0
    memcpy16(tile8_mem[0], placeholderTiles, placeholderTilesLen / 2);
    // Load background tilemap in SBB 28
    memcpy16(se_mem[28], placeholderMap, placeholderMapLen / 2);
    // Load background palette
    memcpy16(pal_bg_mem, placeholderPal, placeholderPalLen / 2);
    // Configure BG2
    REG_BG2CNT = BG_CBB(0) | BG_SBB(28) | BG_AFF_64x64;
    // Configure BG Affine 2
    bg_aff_identity(&affine_bg);
    REG_BG_AFFINE[2] = affine_bg;
    // Set affine background (Mode 1, BG2)
    REG_DISPCNT = DCNT_MODE(1) | DCNT_BG2;
    while(1) {
        VBlankIntrWait();
    }
}
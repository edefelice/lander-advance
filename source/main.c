#include <tonc.h>
#include "gameplay.h"
#include "graphics/moon_far_v2.h"
#include "affine_background.h"

BG_AFFINE affine_bg;
AFF_SRC_EX affine_src;


int main(void) {

    Lander lander;
    GameplayInit(&lander);
    PlayerInput input = {0};
    // Load background tiles in CBB0
    memcpy16(tile8_mem[0], moon_far_v2Tiles, moon_far_v2TilesLen / 2);
    // Load background tilemap in SBB 28
    memcpy16(se_mem[28], moon_far_v2Map, moon_far_v2MapLen / 2);
    // Load background palette
    memcpy16(pal_bg_mem, moon_far_v2Pal, moon_far_v2PalLen / 2);
    // Configure BG2 with wrap off
    REG_BG2CNT = BG_CBB(0) | BG_SBB(28) | BG_AFF_64x64;
    // Set affine background (Mode 1, BG2)
    REG_DISPCNT = DCNT_MODE(1) | DCNT_BG2;
    irq_init(NULL);
    irq_add(II_VBLANK, NULL);
    while(1) {
        VBlankIntrWait(); // Wait VBlank
        key_poll(); // Check key status
        // TODO: input control
        GameplayUpdate(&lander, &input);
        lander_to_affine_src(&lander, &affine_src);
        // Configure BG Affine 2
        bg_rotscale_ex(&affine_bg, &affine_src);
        REG_BG_AFFINE[2] = affine_bg;
    }
}
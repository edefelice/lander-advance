#include <tonc.h>
#include "gameplay.h"
#include "graphics/moon_far_v2.h"
#include "graphics/DRAFT_UHD_1.h"
#include "affine_background.h"
#include "hud.h"

static OBJ_ATTR obj_buffer[128];

int main(void) {
    BG_AFFINE affine_bg = {0};
    AFF_SRC_EX affine_src = {0};
    Lander lander;
    GameplayInit(&lander);
    PlayerInput input = {0};
    // Load background tiles in CBB0
    memcpy16(tile8_mem[0], moon_far_v2Tiles, moon_far_v2TilesLen / 2);
    // Load hud background tiles in CBB2
    memcpy32(tile8_mem[2], DRAFT_UHD_1Tiles, DRAFT_UHD_1TilesLen / 4);
    // Load background tilemap in SBB 28
    memcpy16(se_mem[28], moon_far_v2Map, moon_far_v2MapLen / 2);
    // Load hud background tilemap in SBB 30
    memcpy32(se_mem[30], DRAFT_UHD_1Map, DRAFT_UHD_1MapLen / 4);
    // Load background palette
    memcpy16(pal_bg_mem, moon_far_v2Pal, moon_far_v2PalLen / 2);
    // Load hud background palette
    memcpy16(&pal_bg_mem[13], DRAFT_UHD_1Pal, DRAFT_UHD_1PalLen / 2);
    // Load hud sprites
    hud_load_gfx();
    // Configure BG1 and priority 0
    REG_BG1CNT = BG_CBB(2) | BG_SBB(30) | BG_8BPP | BG_REG_32x32 | BG_PRIO(1);
    // Configure BG2 with wrap off and priority 3
    REG_BG2CNT = BG_CBB(0) | BG_SBB(28) | BG_AFF_64x64 | BG_PRIO(3);
    // Set affine background (Mode 1, BG2)
    REG_DISPCNT = DCNT_MODE(1) | DCNT_OBJ | DCNT_OBJ_1D | DCNT_BG1 | DCNT_BG2;
    // Initialize sprites
    oam_init(obj_buffer, 128);
    int n_obj = hud_bar_init(obj_buffer, 0);
    irq_init(NULL);
    irq_add(II_VBLANK, NULL);
    int fuelbar_cols = 0;
    while(1) {
        key_poll(); // Check key status
        input = cpit_input();
        GameplayUpdate(&lander, &input);
        fuelbar_cols = hud_propellant_to_cols(&lander);
        hud_bar_update(obj_buffer, 0, fuelbar_cols);
        lander_to_affine_src(&lander, &affine_src);
        // Configure BG Affine 2
        bg_rotscale_ex(&affine_bg, &affine_src);
        VBlankIntrWait(); // Wait VBlank
        oam_copy(oam_mem, obj_buffer, n_obj); // copy sprites in oam
        REG_BG_AFFINE[2] = affine_bg;
    }
}
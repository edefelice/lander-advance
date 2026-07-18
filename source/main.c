#include <tonc.h>
#include "gameplay.h"
#include "graphics/moon_far_v2.h"
#include "graphics/HUD_1.h"
#include "affine_background.h"
#include "hud.h"
#include "maxmod.h"
#include "mm_types.h"
#include "soundbank.h"
#include "soundbank_bin.h"

static OBJ_ATTR obj_buffer[128];

int main(void) {
    BG_AFFINE affine_bg = {0};
    AFF_SRC_EX affine_src = {0};
    Lander lander;
    GameplayInit(&lander);
    PlayerInput input = {0};
    // Load background tiles in CBB0
    memcpy32(tile8_mem[0], moon_far_v2Tiles, moon_far_v2TilesLen / 4);
    // Load hud background tiles in CBB2
    memcpy32(tile8_mem[2], HUD_1Tiles, HUD_1TilesLen / 4);
    // Load background tilemap in SBB 28
    memcpy16(se_mem[28], moon_far_v2Map, moon_far_v2MapLen / 2);
    // Load hud background tilemap in SBB 30
    memcpy16(se_mem[30], HUD_1Map, HUD_1MapLen / 2);
    // Load background palette
    memcpy16(pal_bg_mem, moon_far_v2Pal, moon_far_v2PalLen / 2);
    // Load hud background palette
    memcpy16(&pal_bg_mem[HUD_BACKGROUND_PAL_BASE], HUD_1Pal, HUD_1PalLen / 2);
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
    int n_obj = hud_init(obj_buffer, 0);
    irq_init(NULL);
    irq_add(II_VBLANK, mmVBlank);
    mmInitDefault((mm_addr)soundbank_bin, 8); // TODO: check when adding audio files
    //mmEffect(SFX_TEST_TONE); // Added just for test. Change when adding audio.
    while(1) {
        key_poll(); // Check key status
        input = cpit_input();
        GameplayUpdate(&lander, &input);
        hud_update(obj_buffer, 0, &lander, &input);
        lander_to_affine_src(&lander, &affine_src);
        // Configure BG Affine 2
        bg_rotscale_ex(&affine_bg, &affine_src);
        VBlankIntrWait(); // Wait VBlank
        mmFrame();
        oam_copy(oam_mem, obj_buffer, n_obj); // copy sprites in oam
        REG_BG_AFFINE[2] = affine_bg;
    }
}
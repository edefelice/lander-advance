#include <stdint.h>
#include <tonc.h>
#include "hud.h"
#include "physics_constants.h"
#include "graphics/fuel_pow_bars.h"

#define HUD_FUEL_POW_BASE_BAR 0
#define HUD_SPEED_BASE_BARS ((HUD_FUEL_POW_BASE_BAR) + fuel_pow_barsTilesLen / 32)
#define BAR_CELLS 8
#define BAR_COLS 60
#define BAR_X 25
#define BAR_Y 5
#define STEP 8
#define STATES 8

// Palbank map
enum HudPalbank {
    HUD_PB_FUEL_POW = 0,
    HUD_PB_SPEED,
    HUD_PB_DIGIT_SMALL,
    HUD_PB_DIGIT_BIG,
    HUD_PB_COUNT
};

int hud_bar_init(OBJ_ATTR *buffer, int slot) {
    for (int i = 0; i < BAR_CELLS; i++){
        obj_set_attr(&buffer[slot + i], ATTR0_SQUARE | ATTR0_REG, ATTR1_SIZE_8x8,
        ATTR2_PALBANK(HUD_PB_FUEL_POW) | ATTR2_PRIO(1) | HUD_FUEL_POW_BASE_BAR);
    obj_set_pos(&buffer[slot + i], BAR_X + i * STEP, BAR_Y);
    }
    return BAR_CELLS;
}

void hud_bar_update(OBJ_ATTR *buffer, int slot, int cols) {
    int local = 0;
    int level = 0;
    int tile = 0;
    for (int i = 0; i < BAR_CELLS; i++) {
        local = cols - i * STEP;
        level = clamp(local, 0, STATES + 1);
        if (level == 0) {
            obj_hide(&buffer[slot + i]);
        }
        else {
            obj_unhide(&buffer[slot + i], ATTR0_REG);
            tile = HUD_FUEL_POW_BASE_BAR + level - 1;
            buffer[slot + i].attr2 = ATTR2_PALBANK(HUD_PB_FUEL_POW) | ATTR2_PRIO(1) | tile;
        }
    }
}

int hud_propellant_to_cols(const Lander *lander) {
    return (int64_t)lander->propellant * BAR_COLS / PROP_MASS;
}

void hud_load_gfx(void) {
    // Load fuel bar
    memcpy32(&tile_mem_obj[0][HUD_FUEL_POW_BASE_BAR], fuel_pow_barsTiles,
        fuel_pow_barsTilesLen / 4);
    // Load fuel bar palette
    memcpy16(&pal_obj_mem[HUD_PB_FUEL_POW * 16], fuel_pow_barsPal, fuel_pow_barsPalLen / 2);
}
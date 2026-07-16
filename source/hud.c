#include <stdint.h>
#include <tonc.h>
#include "hud.h"
#include "physics_constants.h"
#include "graphics/fuel_pow_bars.h"

#define HUD_FUEL_POW_BASE_BAR 0
#define HUD_SPEED_BASE_BARS ((HUD_FUEL_POW_BASE_BAR) + fuel_pow_barsTilesLen / 32)
#define FUEL_COLS 60
#define CELL_PX 8
#define BAR_LEVELS 8

// Palbank map
enum HudPalbank {
    HUD_PB_FUEL_POW = 0,
    HUD_PB_SPEED,
    HUD_PB_DIGIT_SMALL,
    HUD_PB_DIGIT_BIG,
    HUD_PB_COUNT
};

typedef struct {
    // Position
    int x;
    int y;
    // Cell displacement
    int dx;
    int dy;
    int flip; // attr1 mask
    int cells; // cells number
    int base; // 1st tile index
    int palette_bank;
} HudBar;

enum HudBarId {
    HUD_BAR_FUEL = 0,
    HUD_BAR_COUNT
};

static const HudBar bars[] = {
    // Fuel bar
    { 25, 5, 8, 0, 0, 8,
        HUD_FUEL_POW_BASE_BAR, HUD_PB_FUEL_POW }
};

static int hud_bar_init(OBJ_ATTR *buffer, int slot, const HudBar *bar) {
    for (int i = 0; i < bar->cells; i++){
        obj_set_attr(&buffer[slot + i], ATTR0_SQUARE | ATTR0_HIDE, ATTR1_SIZE_8x8,
        ATTR2_PALBANK(bar->palette_bank) | ATTR2_PRIO(0) | bar->base);
    obj_set_pos(&buffer[slot + i], bar->x + i * bar->dx, bar->y + i * bar->dy);
    }
    return bar->cells;
}

static void hud_bar_update(OBJ_ATTR *buffer, int slot, const HudBar *bar, int cols) {
    int local = 0;
    int level = 0;
    int tile = 0;
    for (int i = 0; i < bar->cells; i++) {
        local = cols - i * CELL_PX;
        level = clamp(local, 0, BAR_LEVELS + 1);
        if (level == 0) {
            obj_hide(&buffer[slot + i]);
        }
        else {
            obj_unhide(&buffer[slot + i], ATTR0_REG);
            tile = bar->base + level - 1;
            buffer[slot + i].attr2 = ATTR2_PALBANK(bar->palette_bank) | ATTR2_PRIO(0) | tile;
        }
    }
}

static int hud_propellant_to_cols(const Lander *lander) {
    return (int64_t)lander->propellant * FUEL_COLS / PROP_MASS;
}

void hud_load_gfx(void) {
    // Load fuel bar
    memcpy32(&tile_mem_obj[0][HUD_FUEL_POW_BASE_BAR], fuel_pow_barsTiles,
        fuel_pow_barsTilesLen / 4);
    // Load fuel bar palette
    memcpy16(&pal_obj_mem[HUD_PB_FUEL_POW * 16], fuel_pow_barsPal, fuel_pow_barsPalLen / 2);
}

int hud_init(OBJ_ATTR *buffer, int slot) {
    return hud_bar_init(buffer, slot, &bars[HUD_BAR_FUEL]);
}

void hud_update(OBJ_ATTR *buffer, int slot, const Lander *lander) {
    hud_bar_update(buffer, slot, &bars[HUD_BAR_FUEL],hud_propellant_to_cols(lander));
}
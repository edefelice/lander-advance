#include <stdint.h>
#include <tonc.h>
#include "hud.h"
#include "fixedpoint32.h"
#include "gameplay.h"
#include "graphics/fuel_pow_bars.h"
#include "graphics/speedbars.h"

#define HUD_FUEL_POW_BASE_BAR 0
#define HUD_HORIZONTAL_BASE_BAR ((HUD_FUEL_POW_BASE_BAR) + fuel_pow_barsTilesLen / 32)
#define HUD_VERTICAL_BASE_BAR ((HUD_HORIZONTAL_BASE_BAR) + BAR_LEVELS)
#define FUEL_COLS 60
#define FUEL_FULL_SCALE (FIX_FROM_INT(100))
#define VX_FULL_SCALE (FIX_FROM_INT(12)) // m/s in Q16.16
#define VX_COLS 24
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
    HUD_BAR_VX_POS,
    HUD_BAR_VX_NEG,
    HUD_BAR_COUNT
};

static const HudBar bars[] = {
    // Fuel bar
    { 25, 5, 8, 0, 0, 8, HUD_FUEL_POW_BASE_BAR, HUD_PB_FUEL_POW },
    // Vx, positive half
    { 44, 145, 8, 0, 0, 3, HUD_HORIZONTAL_BASE_BAR, HUD_PB_SPEED },
    // Vx, negative half
    { 35, 145, -8, 0, ATTR1_HFLIP, 3,
        HUD_HORIZONTAL_BASE_BAR, HUD_PB_SPEED }
};

static int hud_bar_init(OBJ_ATTR *buffer, int slot, const HudBar *bar) {
    for (int i = 0; i < bar->cells; i++){
        obj_set_attr(&buffer[slot + i], ATTR0_SQUARE | ATTR0_HIDE, ATTR1_SIZE_8x8 | bar->flip,
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
    return (int64_t)GameplayGetPropPercent(lander) * FUEL_COLS / FUEL_FULL_SCALE;
}

static int hud_speed_to_cols(fixed v) {
    return (int64_t)v * VX_COLS / VX_FULL_SCALE;
}

static int bar_cols(int i, const Lander *lander) {
    int cols = 0;
    switch (i) {
        case HUD_BAR_FUEL:
            cols = hud_propellant_to_cols(lander);
            break;
        case HUD_BAR_VX_POS:
            cols = hud_speed_to_cols(lander->vx);
            break;
        case HUD_BAR_VX_NEG:
            cols = hud_speed_to_cols(-lander->vx);
            break;
    }
    return cols;
}

void hud_load_gfx(void) {
    // Load fuel bar
    memcpy32(&tile_mem_obj[0][HUD_FUEL_POW_BASE_BAR], fuel_pow_barsTiles,
        fuel_pow_barsTilesLen / 4);
    // Load fuel bar palette
    memcpy16(&pal_obj_mem[HUD_PB_FUEL_POW * 16], fuel_pow_barsPal, fuel_pow_barsPalLen / 2);
    // Load vx bar
    memcpy32(&tile_mem_obj[0][HUD_HORIZONTAL_BASE_BAR], speedbarsTiles,
        speedbarsTilesLen / 4);
    // Load vx bar palette
    memcpy16(&pal_obj_mem[HUD_PB_SPEED * 16], speedbarsPal, speedbarsPalLen / 2);
}

int hud_init(OBJ_ATTR *buffer, int slot) {
    int s = slot;
    for (int i = 0; i < HUD_BAR_COUNT; i++) {
        s += hud_bar_init(buffer, s, &bars[i]);
    }
    return (s - slot);
}

void hud_update(OBJ_ATTR *buffer, int slot, const Lander *lander) {
    int s = slot;
    for (int i = 0; i < HUD_BAR_COUNT; i++) {
        hud_bar_update(buffer, s, &bars[i],bar_cols(i, lander));
        s += bars[i].cells;
    }
}
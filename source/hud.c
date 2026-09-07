#include <stdint.h>
#include <tonc.h>
#include "hud.h"
#include "fixedpoint32.h"
#include "gameplay.h"
#include "graphics/HUD_1.h"
#include "graphics/fuel_pow_bars.h"
#include "graphics/speedbars.h"
#include "graphics/digit_small.h"
#include "graphics/digit_big.h"
#include "cockpit.h"

#define HUD_FUEL_POW_BASE_BAR 0
#define DIGIT_DOT_GLYPHS 10 // index for dotted numbers
#define FUEL_COLS 60
#define SPEED_COLS 24
#define CELL_PX 8
#define BAR_LEVELS 8
#define HUD_HORIZONTAL_BASE_BAR ((HUD_FUEL_POW_BASE_BAR) + fuel_pow_barsTilesLen / 32)
#define HUD_VERTICAL_BASE_BAR ((HUD_HORIZONTAL_BASE_BAR) + BAR_LEVELS)
#define HUD_DIGIT_SMALL_BASE ((HUD_VERTICAL_BASE_BAR) + BAR_LEVELS)
#define HUD_DIGIT_BIG_BASE ((HUD_DIGIT_SMALL_BASE) + digit_smallTilesLen / 32)
#define FUEL_FULL_SCALE (FIX_FROM_INT(100))
#define VX_FULL_SCALE (FIX_FROM_INT(12)) // m/s in Q16.16 TODO: check when fine tuning
#define VY_FULL_SCALE (FIX_FROM_INT(12)) // m/s in Q16.16 TODO: check when fine tuning
#define W_FULL_SCALE (FIX_FROM_INT(6)) // rad/s in Q16.16 TODO: check when fine tuning

#define HUD_LAMP_PAL_BASE 18   // first live palette index of the lamps
#define HUD_LAMP_ON_STEP  10   // lit colour = unlit colour + 10
#define HUD_LAMP_OFF ((HUD_LAMP_PAL_BASE) - (HUD_BACKGROUND_PAL_BASE))
#define HUD_LAMP_ON  ((HUD_LAMP_OFF) + (HUD_LAMP_ON_STEP))

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
    int cells; // Number of cells
    int base; // 1st tile index
    int palette_bank;
    int prio; // Priority
} HudBar;

enum HudBarId {
    HUD_BAR_FUEL = 0,
    HUD_BAR_POWER,
    HUD_BAR_VX_POS,
    HUD_BAR_VX_NEG,
    HUD_BAR_VY_POS,
    HUD_BAR_VY_NEG,
    HUD_BAR_W_POS,
    HUD_BAR_W_NEG,
    HUD_BAR_COUNT
};

static const HudBar bars[] = {
    // Fuel bar
    { 25, 5, 8, 0, 0, 8, HUD_FUEL_POW_BASE_BAR, HUD_PB_FUEL_POW, 0 },
    // Power bar
    { 112, 5, 9, 0, 0, 6, HUD_FUEL_POW_BASE_BAR, HUD_PB_FUEL_POW, 0 },
    // Vx, positive half
    { 44, 145, 8, 0, 0, 3, HUD_HORIZONTAL_BASE_BAR, HUD_PB_SPEED, 0 },
    // Vx, negative half
    { 35, 145, -8, 0, ATTR1_HFLIP, 3, HUD_HORIZONTAL_BASE_BAR, HUD_PB_SPEED, 0 },
    // Vy, positive half
    { 7, 108, 0, -8, 0, 3, HUD_VERTICAL_BASE_BAR, HUD_PB_SPEED, 0 },
    // Vy, negative half
    { 7, 117, 0,  8, ATTR1_VFLIP, 3, HUD_VERTICAL_BASE_BAR, HUD_PB_SPEED, 0 },
    // w, positive half
    { 108, 145,  8, 0, 0, 3, HUD_HORIZONTAL_BASE_BAR, HUD_PB_SPEED, 0 },
    // w, negative half
    {  99, 145, -8, 0, ATTR1_HFLIP,  3, HUD_HORIZONTAL_BASE_BAR, HUD_PB_SPEED, 0 }

};

typedef struct {
    HudBar bar;
    int stride; // 1 = digit_small, 2 = digit_big
    int dot_cell; // dotted-digit cell index, -1 = no dot
} HudDigits;

static const HudDigits digits[] = {
    // Vx digits int
    { { 33, 137, 5, 0, 0, 2, HUD_DIGIT_SMALL_BASE, HUD_PB_DIGIT_SMALL, 0 }, 1, 1},
    // Vx digits decimal
    { { 45, 137, 5, 0, 0, 2, HUD_DIGIT_SMALL_BASE, HUD_PB_DIGIT_SMALL, 0 }, 1, -1},
    // Vy digits int
    { { 16, 111, 5, 0, 0, 2, HUD_DIGIT_SMALL_BASE, HUD_PB_DIGIT_SMALL, 0 }, 1, 1},
    // Vy digits decimal
    { { 28, 111, 5, 0, 0, 2, HUD_DIGIT_SMALL_BASE, HUD_PB_DIGIT_SMALL, 0 }, 1, -1},
    // w digits integer part
    { { 97, 137, 5, 0, 0, 2, HUD_DIGIT_SMALL_BASE, HUD_PB_DIGIT_SMALL, 0 }, 1, 1},
    // w digits decimal part
    { { 109, 137, 5, 0, 0, 2, HUD_DIGIT_SMALL_BASE, HUD_PB_DIGIT_SMALL, 0 }, 1, -1},
    // H (altitude) integer part
    { { 171, 27, 5, 0, 0, 4, HUD_DIGIT_BIG_BASE + 2, HUD_PB_DIGIT_BIG, 0 }, 1, 3},
    // H (altitude) decimal part
    { { 193, 27, 5, 0, 0, 1, HUD_DIGIT_BIG_BASE + 2, HUD_PB_DIGIT_BIG, 0 }, 1, -1},
    // Vz, integer part
    { { 176, 39, 5, 0, 0, 2, HUD_DIGIT_BIG_BASE + 2, HUD_PB_DIGIT_BIG, 0 }, 1, 1 },
    // Vz, decimal part
    { { 188, 39, 5, 0, 0, 2, HUD_DIGIT_BIG_BASE + 2, HUD_PB_DIGIT_BIG, 0 }, 1, -1 },
    // Vz, sign
    { { 170, 39, 5, 0, 0, 1, HUD_DIGIT_BIG_BASE, HUD_PB_DIGIT_BIG, 0 }, 1, -1 }
};

enum HudDigitsId {
    HUD_DIGITS_VX_INT = 0,
    HUD_DIGITS_VX_DEC,
    HUD_DIGITS_VY_INT,
    HUD_DIGITS_VY_DEC,
    HUD_DIGITS_W_INT,
    HUD_DIGITS_W_DEC,
    HUD_DIGITS_H_INT,
    HUD_DIGITS_H_DEC,
    HUD_DIGITS_VZ_INT,
    HUD_DIGITS_VZ_DEC,
    HUD_DIGITS_VZ_SIGN,
    HUD_DIGITS_COUNT
};

enum HudLampId {
    HUD_LAMP_DANGER = 0,
    HUD_LAMP_MAIN,
    HUD_LAMP_TRANS_L,
    HUD_LAMP_TRANS_R,
    HUD_LAMP_TRANS_UP,
    HUD_LAMP_TRANS_DOWN,
    HUD_LAMP_ROT_L,
    HUD_LAMP_ROT_R,
    HUD_LAMP_RADAR,
    HUD_LAMP_LIGHT,
    HUD_LAMP_COUNT
};

static int hud_bar_init(OBJ_ATTR *buffer, int slot, const HudBar *bar) {
    for (int i = 0; i < bar->cells; i++){
        obj_set_attr(&buffer[slot + i], ATTR0_SQUARE | ATTR0_HIDE, ATTR1_SIZE_8x8 | bar->flip,
        ATTR2_PALBANK(bar->palette_bank) | ATTR2_PRIO(bar->prio) | bar->base);
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
            buffer[slot + i].attr2 = ATTR2_PALBANK(bar->palette_bank) | ATTR2_PRIO(bar->prio) | tile;
        }
    }
}

static void hud_digits_update(OBJ_ATTR *buffer, int slot, const HudDigits *d, int value) {
    int div = 1;
    for (int i = 0; i < d->bar.cells - 1; i++) {
        div *= 10;
    }
    int digit = 0;
    int tile = 0;
    for(int j = 0; j < d->bar.cells; j++) {
        obj_unhide(&buffer[slot + j], ATTR0_REG);
        digit = value / div % 10;
        tile = d->bar.base + d->stride * digit;
        if (j == d->dot_cell) {
            tile += DIGIT_DOT_GLYPHS * d->stride;
        }
        buffer[slot + j].attr2 = ATTR2_PALBANK(d->bar.palette_bank) | ATTR2_PRIO(d->bar.prio) | tile;
        div /= 10;
    }
}

static void hud_sign_update(OBJ_ATTR *buffer, int slot, const HudDigits *d, int value) {
    obj_unhide(&buffer[slot], ATTR0_REG);
    int tile = (value >= 0) ? d->bar.base : d->bar.base + 1;
    buffer[slot].attr2 = ATTR2_PALBANK(d->bar.palette_bank) | ATTR2_PRIO(d->bar.prio) | tile;
}

static int hud_propellant_to_cols(const Lander *lander) {
    return (int64_t)GameplayGetPropPercent(lander) * FUEL_COLS / FUEL_FULL_SCALE;
}

static int hud_speed_to_cols(fixed v, fixed full_scale) {
    return (int64_t)v * SPEED_COLS / full_scale;
}

static int bar_cols(int i, const Lander *lander) {
    int cols = 0;
    switch (i) {
        case HUD_BAR_FUEL:
            cols = hud_propellant_to_cols(lander);
            break;
        case HUD_BAR_POWER:
            cols = lander->available_power * CELL_PX;
            break;
        case HUD_BAR_VX_POS:
            cols = hud_speed_to_cols(lander->vx, VX_FULL_SCALE);
            break;
        case HUD_BAR_VX_NEG:
            cols = hud_speed_to_cols(-lander->vx, VX_FULL_SCALE);
            break;
        case HUD_BAR_VY_POS:
            cols = hud_speed_to_cols(lander->vy, VY_FULL_SCALE);
            break;
        case HUD_BAR_VY_NEG:
            cols = hud_speed_to_cols(-lander->vy, VY_FULL_SCALE);
            break;
        case HUD_BAR_W_POS:
            cols = hud_speed_to_cols(lander->omega, W_FULL_SCALE);
            break;
        case HUD_BAR_W_NEG:
            cols = hud_speed_to_cols(-lander->omega, W_FULL_SCALE);
            break;
    }
    return cols;
}

static int digits_value(int i, const Lander *lander) {
    fixed magnitude = 0;
    int value = 0;
    switch (i) {
        case HUD_DIGITS_VX_INT:
            magnitude = (lander->vx < 0) ? -lander->vx : lander->vx;
            value = magnitude / FIX_FROM_INT(1);
            break;
        case HUD_DIGITS_VX_DEC:
            magnitude = (lander->vx < 0) ? -lander->vx : lander->vx;
            value = (int64_t)magnitude * 100 / FIX_FROM_INT(1);
            value %= 100;
            break;
        case HUD_DIGITS_VY_INT:
            magnitude = (lander->vy < 0) ? -lander->vy : lander->vy;
            value = magnitude / FIX_FROM_INT(1);
            break;
        case HUD_DIGITS_VY_DEC:
            magnitude = (lander->vy < 0) ? -lander->vy : lander->vy;
            value = (int64_t)magnitude * 100 / FIX_FROM_INT(1);
            value %= 100;
            break;
        case HUD_DIGITS_W_INT:
            magnitude = (lander->omega < 0) ? -lander->omega : lander->omega;
            value = magnitude / FIX_FROM_INT(1);
            break;
        case HUD_DIGITS_W_DEC:
            magnitude = (lander->omega < 0) ? -lander->omega : lander->omega;
            value = (int64_t)magnitude * 100 / FIX_FROM_INT(1);
            value %= 100;
            break;
        case HUD_DIGITS_H_INT:
            value = lander->z / FIX_FROM_INT(1);
            break;
        case HUD_DIGITS_H_DEC:
            value = (int64_t)lander->z * 10 / FIX_FROM_INT(1);
            value %= 10;
            break;
        case HUD_DIGITS_VZ_INT:
            magnitude = (lander->vz < 0) ? -lander->vz : lander->vz;
            value = magnitude / FIX_FROM_INT(1);
            break;
        case HUD_DIGITS_VZ_DEC:
            magnitude = (lander->vz < 0) ? -lander->vz : lander->vz;
            value = (int64_t)magnitude * 100 / FIX_FROM_INT(1);
            value %= 100;
            break;
        case HUD_DIGITS_VZ_SIGN:
            value = lander->vz;
            break;
    }
    return value;
}

static bool lamp_on(int k, const Lander *lander, const PlayerInput *input) {
    bool on = false;
    switch(k) {
        case HUD_LAMP_DANGER:
            on = false; // TODO: needs crash condition from gameplay
            break;
        case HUD_LAMP_MAIN:
            on = input->thrust_main;
            break;
        case HUD_LAMP_TRANS_L:
            on = (input->rcs_x == -1);
            break;
        case HUD_LAMP_TRANS_R:
            on = (input->rcs_x == 1);
            break;
        case HUD_LAMP_TRANS_UP:
            on = (input->rcs_y == 1);
            break;
        case HUD_LAMP_TRANS_DOWN:
            on = (input->rcs_y == -1);
            break;
        case HUD_LAMP_ROT_L:
            on = (input->rotate == -1);
            break;
        case HUD_LAMP_ROT_R:
            on = (input->rotate == 1);
            break;
        case HUD_LAMP_RADAR:
            on = input->radar;
            break;
        case HUD_LAMP_LIGHT:
            on = lander->light_on;
            break;
    }
    return on;
}

void hud_load_gfx(void) {
    // Load fuel/power bar
    memcpy32(&tile_mem_obj[0][HUD_FUEL_POW_BASE_BAR], fuel_pow_barsTiles,
        fuel_pow_barsTilesLen / 4);
    // Load fuel/power bar palette
    memcpy16(&pal_obj_mem[HUD_PB_FUEL_POW * 16], fuel_pow_barsPal, fuel_pow_barsPalLen / 2);
    // Load speed bars
    memcpy32(&tile_mem_obj[0][HUD_HORIZONTAL_BASE_BAR], speedbarsTiles,
        speedbarsTilesLen / 4);
    // Load speed bars palette
    memcpy16(&pal_obj_mem[HUD_PB_SPEED * 16], speedbarsPal, speedbarsPalLen / 2);
    // Load digits (small) tiles
    memcpy32(&tile_mem_obj[0][HUD_DIGIT_SMALL_BASE], digit_smallTiles, digit_smallTilesLen / 4);
    // Load digits (small) palette
    memcpy16(&pal_obj_mem[HUD_PB_DIGIT_SMALL * 16], digit_smallPal, digit_smallPalLen / 2);
    // Load digits (big) tiles
    memcpy32(&tile_mem_obj[0][HUD_DIGIT_BIG_BASE], digit_bigTiles, digit_bigTilesLen / 4);
    // Load digits (big) palette
    memcpy16(&pal_obj_mem[HUD_PB_DIGIT_BIG * 16], digit_bigPal, digit_bigPalLen / 2);
}

int hud_init(OBJ_ATTR *buffer, int slot) {
    int s = slot;
    for (int i = 0; i < HUD_BAR_COUNT; i++) {
        s += hud_bar_init(buffer, s, &bars[i]);
    }
    for(int j = 0; j < HUD_DIGITS_COUNT; j++) { // Includes the sign cell: init positions every cell
        s += hud_bar_init(buffer, s, &digits[j].bar);
    }
    return (s - slot);
}

void hud_update(OBJ_ATTR *buffer, int slot, const Lander *lander, const PlayerInput *input) {
    int s = slot;
    for (int i = 0; i < HUD_BAR_COUNT; i++) {
        hud_bar_update(buffer, s, &bars[i],bar_cols(i, lander));
        s += bars[i].cells;
    }
    for (int j = 0; j < HUD_DIGITS_VZ_SIGN; j++) { // Sign is last: handled after the loop, not as a digit
        hud_digits_update(buffer, s, &digits[j], digits_value(j, lander));
        s += digits[j].bar.cells;
    }
    hud_sign_update(buffer, s, &digits[HUD_DIGITS_VZ_SIGN], digits_value(HUD_DIGITS_VZ_SIGN, lander));

    for (int k = 0; k < HUD_LAMP_COUNT; k++) {
        pal_bg_mem[HUD_LAMP_PAL_BASE + k] = HUD_1Pal[(lamp_on(k, lander, input) ? HUD_LAMP_ON : HUD_LAMP_OFF) + k];
    }
}

int hud_post_fuel_power_slot(void) {
    return bars[HUD_BAR_FUEL].cells + bars[HUD_BAR_POWER].cells;
}

int hud_digit_slot_end(void) {
    int s = 0;
    for (int i = 0; i < HUD_DIGITS_COUNT; i++) {
        s += digits[i].bar.cells;
    }
    return s;
}
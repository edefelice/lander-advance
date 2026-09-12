#ifndef HUD_H
#define HUD_H

#include <tonc.h>
#include "gameplay.h"
#include "cockpit.h"

#define MAX_SPRITES 128
// HUD background palette is loaded into pal_bg_mem at this offset (see hud.c lamps)
#define HUD_BACKGROUND_PAL_BASE 25
// Index into pal_bg_mem of the speedbar "ruler" colour
// (offset 25 within HUD_1Pal, relocated by HUD_BACKGROUND_PAL_BASE on load)
#define HUD_SPEED_RULER_PAL_IDX ((HUD_BACKGROUND_PAL_BASE) + 25)

void hud_load_gfx(void);
int hud_init(OBJ_ATTR *buffer, int slot);
void hud_update(OBJ_ATTR *buffer, int slot, const Lander *lander, const PlayerInput *input);
int hud_post_fuel_power_slot(void);
int hud_digit_slot_end(void);
int hud_warning_slot(int slot);
#endif // HUD_H
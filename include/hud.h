#ifndef HUD_H
#define HUD_H

#include <tonc.h>
#include "gameplay.h"
#include "cockpit.h"

// HUD background palette is loaded into pal_bg_mem at this offset (see hud.c lamps)
#define HUD_BACKGROUND_PAL_BASE 13

void hud_load_gfx(void);
int hud_init(OBJ_ATTR *buffer, int slot);
void hud_update(OBJ_ATTR *buffer, int slot, const Lander *lander, const PlayerInput *input);
#endif // HUD_H
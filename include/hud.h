#ifndef HUD_H
#define HUD_H

#include <tonc.h>
#include "gameplay.h"

void hud_load_gfx(void);
int hud_init(OBJ_ATTR *buffer, int slot);
void hud_update(OBJ_ATTR *buffer, int slot, const Lander *lander);
#endif // HUD_H
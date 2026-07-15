#ifndef HUD_H
#define HUD_H

#include <tonc.h>
#include "gameplay.h"

int hud_bar_init(OBJ_ATTR *buffer, int slot);
void hud_bar_update(OBJ_ATTR *buffer, int slot, int cols);
int hud_propellant_to_cols(const Lander *lander);

#endif // HUD_H
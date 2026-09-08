#ifndef GAME_SCORE_H
#define GAME_SCORE_H

#include "gameplay.h"
#include "game_result.h"


/*
    game_score.h

    This module defines the scoring system and its public interface.

    Responsibilities:

    - Define score weights and maximum score
    - Expose the final game result calculation

    Score is divided among fuel, distance, landing quality,
    battery and landing pad position.
*/

#define SCORE_FUEL       3000       //more fuel you have, higher score (linear)
#define SCORE_DISTANCE   3000       //further from the center of the map, higher score (linear)
#define SCORE_LANDING    2000       //velocity close to 0, higher score (exp)
#define SCORE_BATTERY    1000       //more battery, higher score (linear)
#define SCORE_PAD        1000       //center of the pad, higher score (PIER VEDI TU)

/*
------------------------------------------------------------
Score weights

Total maximum score = 10000

Fuel and distance are the most important components because
they are strongly related: reaching distant landing pads
normally requires more propellant. (60% total score)
------------------------------------------------------------
*/

#define SCORE_MAX        10000

GameResult GameScoreCreateResult(const Lander *lander);

#endif // GAME_SCORE_H
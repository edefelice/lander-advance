// Shared struct to store the outcome of a completed game.
#ifndef GAME_RESULT_H
#define GAME_RESULT_H

/*
    game_result.h

    This module defines the result of a completed game.

    Responsibilities:

    - Define game outcome (win or lose)
    - Define the reason for a crash
    - Store the final score

    GameResult is shared between gameplay, scoring and
    game result presentation modules.
*/

typedef enum {
    GR_WIN = 0,
    GR_LOSE
} GrOutcome;

typedef enum {
    GR_REASON_NONE = 0,
    GR_REASON_VERTICAL_SPEED,
    GR_REASON_HORIZONTAL_SPEED,
    GR_REASON_ANGULAR_SPEED,
    GR_REASON_OUT_OF_PAD
} GrReason;

typedef struct {
    GrOutcome outcome;
    GrReason  reason;
    int       score;
} GameResult;

#endif // GAME_RESULT_H
// Shared struct to store the outcome of a completed game.
#ifndef GAME_RESULT_H
#define GAME_RESULT_H

typedef enum {
    GR_WIN = 0,
    GR_LOSE
} GrOutcome;

typedef enum {
    GR_REASON_NONE = 0,
    GR_REASON_TOO_FAST,
    GR_REASON_OUT_OF_PAD
} GrReason;

typedef struct {
    GrOutcome outcome;
    GrReason  reason;
    int       score;
} GameResult;

#endif // GAME_RESULT_H
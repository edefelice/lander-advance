// Shared struct to store game score.
#ifndef GAME_RESULT_H
#define GAME_RESULT_H

typedef enum { GR_WIN = 0, GR_LOSE} gr_outcome;

typedef enum {
    GR_REASON_TOO_FAST = 0,
    GR_REASON_OUT_OF_PAD
} gr_reason;

typedef struct {
    gr_outcome outcome;
    gr_reason  reason;
    int        score;
} game_result;

#endif // GAME_RESULT_H
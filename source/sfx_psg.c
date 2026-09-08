#include <tonc.h>
#include <stdint.h>
#include "sfx_psg.h"

typedef enum {
    STEP_SILENCE = 0,
    STEP_TRIGGER
} Ch2StepType;

typedef struct {
    Ch2StepType type;
    uint16_t cnt; // Complete value for REG_SND2CNT (only if TRIGGER enabled)
    uint16_t rate; // 11-bit rate (only if TRIGGER enabled)
    uint8_t frames;
} Ch2Step;

static const Ch2Step STEPS_SELECTION[] = {
    {STEP_TRIGGER, SSQR_ENV_BUILD(15, 0, 1) | SSQR_DUTY1_4, 0x76B, 3}, // A5
    {STEP_TRIGGER, SSQR_ENV_BUILD(15, 0, 1) | SSQR_DUTY1_4, 0x79D, 3} // E6
};

static const Ch2Step STEPS_BACK[] = {
    {STEP_TRIGGER, SSQR_ENV_BUILD(15, 0, 1) | SSQR_DUTY1_4, 0x739, 5}, // E5
    {STEP_TRIGGER, SSQR_ENV_BUILD(15, 0, 1) | SSQR_DUTY1_4, 0x6D6, 7} // A4
};

static const Ch2Step STEPS_WARNING[] = {
    { STEP_TRIGGER, SSQR_ENV_BUILD(15,0,2)|SSQR_DUTY1_2, 0x783, 6 },
    { STEP_SILENCE, 0, 0, 4 },
    { STEP_TRIGGER, SSQR_ENV_BUILD(15,0,2)|SSQR_DUTY1_2, 0x783, 6 },
    { STEP_SILENCE, 0, 0, 4 },
    { STEP_TRIGGER, SSQR_ENV_BUILD(15,0,2)|SSQR_DUTY1_2, 0x783, 6 }
};
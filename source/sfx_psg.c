#include <tonc.h>
#include <stdint.h>
#include "sfx_psg.h"


// Channel 2 sequencer

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

static const Ch2Step *ch2_steps = NULL;
static uint8_t ch2_count = 0;
static uint8_t ch2_idx = 0;
static uint8_t ch2_timer = 0;

static void ch2_apply(const Ch2Step *s) {
    if (s->type == STEP_TRIGGER) {
        REG_SND2CNT = s->cnt;
        REG_SND2FREQ = SFREQ_RESET | SFREQ_RATE(s->rate);
    }
}

// Channel 3 sequencer
typedef enum {
    W_TRIGGER = 0,
    W_LEGATO,
    W_VOLUME,
    W_STOP
} WaveStepType;

typedef struct {
    WaveStepType type;
    uint16_t rate;
    uint16_t vol;
    uint8_t frames;
} WaveStep;

static const WaveStep STEPS_TRUMPET[] = {
    { W_TRIGGER, 0x712, 0,               6 },  // low
    { W_LEGATO,  0x739, 0,               6 },  // high
    { W_LEGATO,  0x712, 0,               6 },  // low
    { W_LEGATO,  0x739, 0,               4 },  // high
    { W_VOLUME,  0,     (1<<14),         1 },  // 50% duty
    { W_VOLUME,  0,     (1<<14)|(1<<13), 1 },  // 25% duty
    { W_STOP,    0,     0,               0 },
};

static void load_sawtooth_wave(void) {
    uint32_t wave_data[4];
    uint8_t samples[32];

    for (int i = 0; i < 32; i++) {
        samples[i] = (i * 15) / 31;  // rampa 0..15
    }

    // Due campioni per byte: nibble alto = primo campione, nibble basso = secondo
    for (int j = 0; j < 4; j++) {
        u8 b0 = (samples[j * 8] << 4) | samples[j * 8 + 1];
        u8 b1 = (samples[j * 8 + 2] << 4) | samples[j * 8 + 3];
        u8 b2 = (samples[j * 8 + 4] << 4) | samples[j * 8 + 5];
        u8 b3 = (samples[j * 8 + 6] << 4) | samples[j * 8 + 7];
        wave_data[j] = b0 | (b1<<8) | (b2<<16) | (b3<<24);
    }

    REG_SND3SEL = 0;  // canale spento durante la scrittura, dimensione=32, banco=0
    REG_WAVE_RAM0 = wave_data[0];
    REG_WAVE_RAM1 = wave_data[1];
    REG_WAVE_RAM2 = wave_data[2];
    REG_WAVE_RAM3 = wave_data[3];
    REG_SND3SEL = (1<<7) | (1<<6);  // riaccende il canale (bit 7), dimensione/banco restano 0
}
#include <tonc.h>
#include <stdint.h>
#include "sfx_psg.h"
#include "tonc_memdef.h"
#include "tonc_memmap.h"

typedef enum {
    STEP_SILENCE = 0,
    STEP_TRIGGER
} Ch2StepType;

// Channel 1
typedef struct {
    Ch2StepType type;
    uint16_t rate; // 11-bit rate (only if TRIGGER enabled)
    uint8_t frames;
} Ch1Step;

static const Ch1Step STEPS_RADAR[] = {
    {STEP_TRIGGER, 0x706, 6}, // C5
    {STEP_SILENCE, 0, 4},
    {STEP_TRIGGER, 0x706, 6},
    {STEP_SILENCE, 0, 4},
    {STEP_TRIGGER, 0x706, 6}
};

static void ch1_apply(const Ch1Step *s) {
    if (s->type == STEP_TRIGGER) {
        REG_SND1SWEEP = (4 << 4) | (0 << 3) | 3; // tempo = 4, dir = 0(up), shift = 3
        REG_SND1CNT = SSQR_ENV_BUILD(15, 0, 3) | SSQR_DUTY1_2;
        REG_SND1FREQ = SFREQ_RESET | SFREQ_RATE(s->rate);
    }
}

static const Ch1Step *ch1_steps = NULL;
static uint8_t ch1_count = 0;
static uint8_t ch1_idx = 0;
static uint8_t ch1_timer = 0;

static void ch1_play(const Ch1Step *steps, uint8_t count) {
    ch1_steps = steps;
    ch1_count = count;
    ch1_idx = 0;
    ch1_apply(&ch1_steps[0]); // immidiately apply 1st step
    ch1_timer =ch1_steps[0].frames;
}

static void ch1_tick(void) {
    if (ch1_steps == NULL || ch1_idx >= ch1_count) {
        return;
    }
    if (ch1_timer > 0) {
        ch1_timer--;
        return;
    }
    
    ch1_idx++;
    if (ch1_idx >= ch1_count) {
        ch1_steps = NULL; // finished sequence
        return;
    }
    ch1_apply(&ch1_steps[ch1_idx]);
    ch1_timer = ch1_steps[ch1_idx].frames;
}

// Channel 2

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

static const Ch2Step STEPS_DPAD[] = {
    {STEP_TRIGGER, SSQR_ENV_BUILD(10, 0, 1) | SSQR_DUTY1_8, 0x759, 3} // G5
};

static const Ch2Step STEPS_WARNING[] = {
    { STEP_TRIGGER, SSQR_ENV_BUILD(15, 0, 1) | SSQR_DUTY1_2, 0x5F4, 30 },
    { STEP_SILENCE, 0, 0, 4 },
    { STEP_TRIGGER, SSQR_ENV_BUILD(15, 0, 1) | SSQR_DUTY1_2, 0x5F4, 30 }
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

#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

static void ch2_play(const Ch2Step *steps, uint8_t count) {
    ch2_steps = steps;
    ch2_count = count;
    ch2_idx = 0;
    ch2_apply(&ch2_steps[0]); // immidiately apply 1st step
    ch2_timer =ch2_steps[0].frames;
}

static void ch2_tick(void) {
    if (ch2_steps == NULL || ch2_idx >= ch2_count) {
        return;
    }
    if (ch2_timer > 0) {
        ch2_timer--;
        return;
    }
    
    ch2_idx++;
    if (ch2_idx >= ch2_count) {
        ch2_steps = NULL; // finished sequence
        return;
    }
    ch2_apply(&ch2_steps[ch2_idx]);
    ch2_timer = ch2_steps[ch2_idx].frames;
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
    { W_TRIGGER, 0x712, 0, 6 },  // low
    { W_LEGATO, 0x739, 0, 6 },  // high
    { W_LEGATO, 0x712, 0, 6 },  // low
    { W_LEGATO, 0x739, 0, 4 },  // high
    { W_VOLUME, 0, (1<<14), 1 },  // 50% duty
    { W_VOLUME, 0, (1<<14)|(1<<13), 1 },  // 25% duty
    { W_STOP, 0, 0, 0 },
};

static const WaveStep *ch3_steps = NULL;
static uint8_t ch3_count = 0;
static uint8_t ch3_idx = 0;
static uint8_t ch3_timer = 0;

static void ch3_apply(const WaveStep *ws) {
    switch (ws->type) {
        case W_TRIGGER:
            REG_SND3SEL = (1 << 7) | (1 << 6); // turn channel on
            REG_SND3CNT = (1 << 13); // Volume 100%
            REG_SND3FREQ = SFREQ_RESET | SFREQ_RATE(ws->rate);
            break;
        case W_LEGATO:
            REG_SND3FREQ = SFREQ_RATE(ws->rate);
            break;
        case W_VOLUME:
            REG_SND3CNT = ws->vol;
            break;
        case W_STOP:
            REG_SND3SEL = 0; // turn channel off
            break;
    }
}

static void ch3_play(const WaveStep *steps, uint8_t count) {
    ch3_steps = steps;
    ch3_count = count;
    ch3_idx = 0;
    ch3_apply(&ch3_steps[0]);
    ch3_timer = ch3_steps[0].frames;
}

static void ch3_tick(void) {
    if (ch3_steps == NULL || ch3_idx >= ch3_count) {
        return;
    }
    if (ch3_timer > 0) {
        ch3_timer--;
        return;
    }

    ch3_idx++;
    if (ch3_idx >= ch3_count) {
        ch3_steps = NULL;
        return;
    }
    ch3_apply(&ch3_steps[ch3_idx]);
    ch3_timer = ch3_steps[ch3_idx].frames;
}

static void load_sawtooth_wave(void) {
    uint32_t wave_data[4];
    uint8_t samples[32];

    for (int i = 0; i < 32; i++) {
        samples[i] = (i * 15) / 31;  // 0..15 ramp
    }

    // 2 samples per byte: high nibble = 1st sample, low nibble = 2nd
    for (int j = 0; j < 4; j++) {
        u8 b0 = (samples[j * 8] << 4) | samples[j * 8 + 1];
        u8 b1 = (samples[j * 8 + 2] << 4) | samples[j * 8 + 3];
        u8 b2 = (samples[j * 8 + 4] << 4) | samples[j * 8 + 5];
        u8 b3 = (samples[j * 8 + 6] << 4) | samples[j * 8 + 7];
        wave_data[j] = b0 | (b1<<8) | (b2<<16) | (b3<<24);
    }

    REG_SND3SEL = 0;  // channel off during write, size=32, bank=0
    REG_WAVE_RAM0 = wave_data[0];
    REG_WAVE_RAM1 = wave_data[1];
    REG_WAVE_RAM2 = wave_data[2];
    REG_WAVE_RAM3 = wave_data[3];
    REG_SND3SEL = (1<<7) | (1<<6);  // channel on (bit 7), size/bank stay 0
}

// Channel 4
#define ENGINE_NOISE_RATIO 2
#define ENGINE_NOISE_SHIFT 6
#define ENGINE_NOISE_WIDTH 0
static EngineState engine_state = ENGINE_OFF;

typedef struct {
    Ch2StepType type;
    uint8_t ratio;
    uint8_t shift;
    uint8_t width;
    uint8_t vol;
    uint8_t env_step;
    uint8_t frames;
} Ch4Step;

static const Ch4Step STEPS_CRASH[] = {
    {STEP_TRIGGER, 2, 4, 0, 15, 1, 4},
    {STEP_TRIGGER, 6, 10, 0, 15, 3, 42},
    {STEP_SILENCE, 0, 0, 0, 0, 0, 0}
};

static const Ch4Step STEPS_VICTORY[] = {
    {STEP_TRIGGER, 3, 4, 1, 12, 1, 3},
    {STEP_SILENCE,0 , 0, 0, 0, 0, 8},
    {STEP_TRIGGER, 2, 3, 0, 14, 1, 3},
    {STEP_SILENCE,0 , 0, 0, 0, 0, 10},
    {STEP_TRIGGER, 3, 5, 1, 10, 1, 3},
    {STEP_SILENCE,0 , 0, 0, 0, 0, 7},
    {STEP_TRIGGER, 2, 4, 0, 13, 1, 3},
    {STEP_SILENCE,0 , 0, 0, 0, 7},
    {STEP_TRIGGER, 2, 4, 1, 11, 1, 3},
    {STEP_SILENCE,0 , 0, 0, 0, 0, 8},
    {STEP_TRIGGER, 2, 4, 0, 15, 2, 5},
    {STEP_SILENCE,0 , 0, 0, 0, 0, 0}
};

static const Ch4Step *ch4_steps = NULL;
static uint8_t ch4_count = 0;
static uint8_t ch4_idx = 0;
static uint8_t ch4_timer = 0;

static void ch4_apply(const Ch4Step *s) {
    if (s->type == STEP_TRIGGER) {
        REG_SND4CNT = SSQR_ENV_BUILD(s->vol, 0, s->env_step);
        REG_SND4FREQ = SFREQ_RESET | ((s->shift & 0xF) << 4) | ((s->width & 1) << 3) | (s->ratio & 0x7);
    }
    else {
        REG_SND4CNT = SSQR_ENV_BUILD(0, 0, 0);
    }
}

static void ch4_play(const Ch4Step *steps, uint8_t count) {
    ch4_steps = steps;
    ch4_count = count;
    ch4_idx = 0;
    ch4_apply(&ch4_steps[0]); // immidiately apply 1st step
    ch4_timer =ch4_steps[0].frames;
}

static void ch4_tick(void) {
    if (ch4_steps == NULL || ch4_idx >= ch4_count) {
        return;
    }
    if (ch4_timer > 0) {
        ch4_timer--;
        return;
    }
    
    ch4_idx++;
    if (ch4_idx >= ch4_count) {
        ch4_steps = NULL; // finished sequence
        return;
    }
    ch4_apply(&ch4_steps[ch4_idx]);
    ch4_timer = ch4_steps[ch4_idx].frames;
}

static void engine_start(uint8_t ivol) {
    REG_SND4CNT  = SSQR_ENV_BUILD(ivol, 0, 0);
    REG_SND4FREQ = (1 << 15) | ((ENGINE_NOISE_SHIFT & 0xF) << 4)
                            | ((ENGINE_NOISE_WIDTH & 1) << 3)
                            | (ENGINE_NOISE_RATIO & 0x7);
}

static void engine_stop(void) {
    REG_SND4CNT = SSQR_ENV_BUILD(0, 0, 0);
}

void sfx_engine_set(EngineState state) {
    if (state == engine_state) {
        return;
    }

    switch (state) {
        case ENGINE_MAIN:
            engine_start(9);
            break;
        case ENGINE_RCS:
            engine_start(4);
            break;
        case ENGINE_OFF:
            engine_stop();
            break;
    }

    engine_state = state;
}

#define SWITCH_NOISE_RATIO 4
#define SWITCH_NOISE_SHIFT 2
#define SWITCH_NOISE_WIDTH 1
#define SWITCH_NOISE_LEN 62

void switch_on(void) {
    REG_SND4CNT  = SSQR_ENV_BUILD(15, 0, 0) | SWITCH_NOISE_LEN;
    REG_SND4FREQ = SFREQ_RESET | SFREQ_TIMED | ((SWITCH_NOISE_SHIFT) << 4)
                            | ((SWITCH_NOISE_WIDTH) << 3) | (SWITCH_NOISE_RATIO);
    engine_state = ENGINE_OFF; // force re-trigger next call to sfx_engine_set
}

void sfx_init(void) {
    REG_SNDSTAT   = SSTAT_ENABLE;
    REG_SNDDSCNT  = SDS_DMG100;
    REG_SNDDMGCNT = SDMG_BUILD(SDMG_SQR1 | SDMG_SQR2 | SDMG_WAVE | SDMG_NOISE,
                                SDMG_SQR1 | SDMG_SQR2 | SDMG_WAVE | SDMG_NOISE, 7, 7);
    load_sawtooth_wave();
}

void sfx_play(SfxId id) {
    switch (id) {
        case SFX_SELECTION:
            ch2_play(STEPS_SELECTION, ARRAY_LEN(STEPS_SELECTION));
            break;
        case SFX_BACK:
            ch2_play(STEPS_BACK, ARRAY_LEN(STEPS_BACK));
            break;
        case SFX_DPAD:
            ch2_play(STEPS_DPAD, ARRAY_LEN(STEPS_DPAD));
            break;
        case SFX_WARNING:
            ch2_play(STEPS_WARNING, ARRAY_LEN(STEPS_WARNING));
            break;
        case SFX_TRUMPET:
            ch3_play(STEPS_TRUMPET, ARRAY_LEN(STEPS_TRUMPET));
            break;
        case SFX_RADAR:
            ch1_play(STEPS_RADAR, ARRAY_LEN(STEPS_RADAR));
            break;
        case SFX_LIGHT:
            switch_on();
            break;
        case SFX_CRASH:
            ch4_play(STEPS_CRASH, ARRAY_LEN(STEPS_CRASH));
            break;
        case SFX_VICTORY:
            ch4_play(STEPS_VICTORY, ARRAY_LEN(STEPS_VICTORY));
            break;
    }
}

void sfx_update(void) {
    ch1_tick();
    ch2_tick();
    ch3_tick();
    ch4_tick();
}
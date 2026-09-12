#ifndef SFX_PSG_H
#define SFX_PSG_H

typedef enum {
    SFX_SELECTION = 0,
    SFX_BACK,
    SFX_DPAD,
    SFX_WARNING,
    SFX_TRUMPET,
    SFX_RADAR,
    SFX_LIGHT,
    SFX_CRASH,
    SFX_VICTORY
} SfxId;

typedef enum {
    ENGINE_OFF = 0,
    ENGINE_RCS,
    ENGINE_MAIN
} EngineState;

void sfx_init(void); // Master enable + mixer
void sfx_play(SfxId id); // Plays a one-shot effect on channel 2-3 
void sfx_update(void);
void sfx_engine_set(EngineState state); // Channel 4, Main Thrust / RCS

#endif // SFX_PSG_H
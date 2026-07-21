#include <tonc.h>
#include "cockpit.h"
#include "game_result.h"
#include "gameplay.h"
#include "graphics/moon_far_v2.h"
#include "graphics/HUD_1.h"
#include "affine_background.h"
#include "hud.h"
#include "maxmod.h"
#include "mm_types.h"
#include "shell.h"
#include "shell_render.h"
#include "soundbank.h"
#include "soundbank_bin.h"
#include "tonc_memdef.h"

static OBJ_ATTR obj_buffer[MAX_SPRITES];

int main(void) {
    int post_fuel_power_idx = hud_post_fuel_power_slot();
    int digit_sprite_idx_end = hud_digit_slot_end();
    // Initialization
    BG_AFFINE affine_bg = {0};
    AFF_SRC_EX affine_src = {0};
    Lander lander;
    shell_init();
    GameState prev_state = shell_state();
    shell_render_engine_init();
    PlayerInput input = {0};
    // Load background tiles in CBB0
    memcpy32(tile8_mem[0], moon_far_v2Tiles, moon_far_v2TilesLen / 4);
    // Load hud background tiles in CBB2
    memcpy32(tile8_mem[2], HUD_1Tiles, HUD_1TilesLen / 4);
    // Load background tilemap in SBB 28
    memcpy16(se_mem[28], moon_far_v2Map, moon_far_v2MapLen / 2);
    // Load hud background tilemap in SBB 30
    memcpy16(se_mem[30], HUD_1Map, HUD_1MapLen / 2);
    // Load background palette
    memcpy16(pal_bg_mem, moon_far_v2Pal, moon_far_v2PalLen / 2);
    pal_bg_mem[0] = 0x0; // TODO: remove when loading title graphics
    // Load hud background palette
    memcpy16(&pal_bg_mem[HUD_BACKGROUND_PAL_BASE], HUD_1Pal, HUD_1PalLen / 2);
    pal_bg_mem[HUD_SPEED_RULER_PAL_IDX] = 0x0; // Black
    // Load hud sprites
    hud_load_gfx();
    // Configure BG1 and priority 0
    REG_BG1CNT = BG_CBB(2) | BG_SBB(30) | BG_8BPP | BG_REG_32x32 | BG_PRIO(1);
    // Configure BG2 with wrap on and priority 3
    REG_BG2CNT = BG_CBB(0) | BG_SBB(28) | BG_AFF_64x64 | BG_WRAP | BG_PRIO(3);
    // Set regular background (Mode 1, BG0)
    REG_DISPCNT = DCNT_MODE(1) | DCNT_BG0;
    // Initialize sprites
    oam_init(obj_buffer, MAX_SPRITES);
    int n_obj = hud_init(obj_buffer, 0);
    irq_init(NULL);
    irq_add(II_VBLANK, mmVBlank);
    mmInitDefault((mm_addr)soundbank_bin, 8); // TODO: check when adding audio files
    //mmEffect(SFX_TEST_TONE); // Just for test. Change when adding audio.
    bool result_sent = false;
    GameResult result;
    PauseSubState last_pause_choice = SUB_RESUME;
    // Prevents A held during menu confirm from triggering thrust on gameplay entry
    bool suppress_thrust_until_release = false;
    while(1) {
        GameState cur_state = shell_state(); // Update Current state
        // True = in play state coming from title/config/pause screen
        bool entered_gameplay = cur_state == STATE_GAMEPLAY && prev_state != STATE_GAMEPLAY;
        // True = in play state after restarting game
        bool fresh_start = entered_gameplay && (prev_state == STATE_CONFIG_SELECTION || last_pause_choice == SUB_RESTART);
        key_poll(); // Check key status
        if (cur_state != STATE_TITLE) {
            REG_DISPCNT |= DCNT_BG1;
        }
        else {
            REG_DISPCNT &= ~(DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D);
        }

        if(cur_state == STATE_GAMEPLAY) {
            pal_bg_mem[HUD_SPEED_RULER_PAL_IDX] = HUD_1Pal[25];
            REG_DISPCNT |= DCNT_OBJ | DCNT_OBJ_1D | DCNT_BG2; // Set affine background (Mode 1, BG2)
            for (int i = post_fuel_power_idx; i < digit_sprite_idx_end; i++) {
                obj_unhide(&obj_buffer[i], ATTR0_REG);
            }
        }
        else { // Deactivate hud and level background
            pal_bg_mem[0] = 0x0;
            pal_bg_mem[HUD_SPEED_RULER_PAL_IDX] = 0x0;
            for (int i = post_fuel_power_idx; i < MAX_SPRITES; i++) {
                obj_hide(&obj_buffer[i]);
            }
            REG_DISPCNT &= ~DCNT_BG2;
        }

        if (entered_gameplay) {
            tte_erase_screen(); // Hide menu
            pal_bg_mem[0] = moon_far_v2Pal[0];
            pal_bg_mem[HUD_SPEED_RULER_PAL_IDX] = HUD_1Pal[25];
            for (int i = 0; i < digit_sprite_idx_end; i++) {
                obj_unhide(&obj_buffer[i], ATTR0_REG);
            }
            suppress_thrust_until_release = true;
        }

        if (fresh_start) {
            tte_erase_screen(); // Hide menu
            GameplayInit(&lander);
            result_sent = false;
        }

        switch (shell_state()) {
            case STATE_GAMEPLAY:
                input = cpit_input();
                if (suppress_thrust_until_release) {
                    input.thrust_main = false;
                    if (!key_is_down(KEY_A)) {
                        suppress_thrust_until_release = false;
                    }
                }
                shell_feed_input(menu_input()); // To read the Start button
                GameplayUpdate(&lander, &input);
                if (lander.state != LANDER_FLYING && !result_sent) {
                    // For testing
                    result.outcome = (lander.state == LANDER_LANDED) ? GR_WIN : GR_LOSE;
                    result.reason = GR_REASON_NONE;
                    result.score = 0;
                    shell_submit_result(&result);
                    result_sent = true;
                }
                main_states_management(); // Changes game state to "pause" when Start button is pressed
                shell_commit_input();
                hud_update(obj_buffer, 0, &lander, &input);
                // Configure BG Affine 2
                lander_to_affine_src(&lander, &affine_src);
                bg_rotscale_ex(&affine_bg, &affine_src);
                break;
            case STATE_PAUSE:
                shell_render_display();
                shell_feed_input(menu_input()); // Reads input
                main_states_management(); // Changes game state (Title/Gameplay/Pause)
                sub_states_management(); // Changes game substate (Resume/Restart/Title/Credits)
                shell_commit_input();
                last_pause_choice = pause_state(); // Save current substate
                break;
            default:
                shell_render_display();
                shell_feed_input(menu_input());
                main_states_management();
                sub_states_management();
                shell_commit_input();
                break;
        }
        prev_state = cur_state; // Update previous state
        VBlankIntrWait(); // Wait VBlank
        mmFrame();
        oam_copy(oam_mem, obj_buffer, n_obj); // Copy sprites in oam
        REG_BG_AFFINE[2] = affine_bg; // Update affine bg register
    }
}
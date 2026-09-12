#include <stdint.h>
#include <tonc.h>
#include "cockpit.h"
#include "game_result.h"
#include "gameplay.h"
#include "graphics/moon_far_fin.h"
#include "graphics/HUD_1.h"
#include "affine_background.h"
#include "hud.h"
#include "shell.h"
#include "shell_render.h"
#include "sfx_psg.h"
#include "landing_area.h"
#include "game_score.h"
#include "graphics/logo.h"
#include "graphics/ScratchLogoSmall1.h"

static OBJ_ATTR obj_buffer[MAX_SPRITES];

#define SPOTLIGHT_BASE_TILE 128
#define SPOTLIGHT_OBJ_COUNT 4

static void spotlight_init_gfx(void) {
    for (int ty = 0; ty < 8; ty++) {
        for (int tx = 0; tx < 8; tx++) {
            int t = SPOTLIGHT_BASE_TILE + ty * 8 + tx;
            for (int py = 0; py < 8; py++) {
                int y = ty * 8 + py;
                u32 row = 0;
                for (int px = 0; px < 8; px++) {
                    int x = tx * 8 + px;
                    int dx = 127 - 2 * x;
                    int dy = 127 - 2 * y;
                    if (dx * dx + dy * dy <= 9216) {
                        row |= (1 << (px * 4));
                    }
                }
                tile_mem_obj[0][t].data[py] = row;
            }
        }
    }
}

static void spotlight_init_objs(OBJ_ATTR *buffer, int slot) {
    obj_set_attr(&buffer[slot + 0], ATTR0_SQUARE | ATTR0_HIDE | ATTR0_WINDOW, ATTR1_SIZE_64, ATTR2_ID(SPOTLIGHT_BASE_TILE));
    obj_set_pos(&buffer[slot + 0], 40, 24);

    obj_set_attr(&buffer[slot + 1], ATTR0_SQUARE | ATTR0_HIDE | ATTR0_WINDOW, ATTR1_SIZE_64 | ATTR1_HFLIP, ATTR2_ID(SPOTLIGHT_BASE_TILE));
    obj_set_pos(&buffer[slot + 1], 104, 24);

    obj_set_attr(&buffer[slot + 2], ATTR0_SQUARE | ATTR0_HIDE | ATTR0_WINDOW, ATTR1_SIZE_64 | ATTR1_VFLIP, ATTR2_ID(SPOTLIGHT_BASE_TILE));
    obj_set_pos(&buffer[slot + 2], 40, 88);

    obj_set_attr(&buffer[slot + 3], ATTR0_SQUARE | ATTR0_HIDE | ATTR0_WINDOW, ATTR1_SIZE_64 | ATTR1_HFLIP | ATTR1_VFLIP, ATTR2_ID(SPOTLIGHT_BASE_TILE));
    obj_set_pos(&buffer[slot + 3], 104, 88);
}

static void spotlight_set_active(OBJ_ATTR *buffer, int slot, bool active) {
    if (active) {
        obj_unhide(&buffer[slot + 0], ATTR0_REG);
        obj_unhide(&buffer[slot + 1], ATTR0_REG);
        obj_unhide(&buffer[slot + 2], ATTR0_REG);
        obj_unhide(&buffer[slot + 3], ATTR0_REG);
    } else {
        obj_hide(&buffer[slot + 0]);
        obj_hide(&buffer[slot + 1]);
        obj_hide(&buffer[slot + 2]);
        obj_hide(&buffer[slot + 3]);
    }
}

int main(void) {
    int post_fuel_power_idx = hud_post_fuel_power_slot();
    int digit_sprite_idx_end = hud_digit_slot_end();
    bool lander_light_prev = false;
    bool radar_on_prev = false;
    // Initialization
    BG_AFFINE affine_bg = {0};
    AFF_SRC_EX affine_src = {0};
    Lander lander;

    irq_init(NULL);
    irq_add(II_VBLANK, NULL);
    // Load title screen tiles in CBB2
    memcpy32(tile8_mem[2], ScratchLogoSmall1Tiles, ScratchLogoSmall1TilesLen / 4);
    // Load title screen tilemap in SBB 23
    memcpy16(se_mem[29], ScratchLogoSmall1Map, ScratchLogoSmall1MapLen / 2);
    // Load title screen palette
    memcpy16(pal_bg_mem, ScratchLogoSmall1Pal, ScratchLogoSmall1PalLen / 2);
    // Configure BG1 and priority 0
    REG_BG1CNT = BG_CBB(2) | BG_SBB(29) | BG_8BPP | BG_REG_32x32 | BG_PRIO(1);
    // Set regular background (Mode 1, BG0)
    REG_DISPCNT = DCNT_MODE(1) | DCNT_BG1;
    REG_BLDCNT = BLD_BG1 | BLD_BLACK;
    for (int frame = 16; frame >= 0; frame--) {
        REG_BLDY = BLDY_BUILD(frame); 
        for (int j = 0; j < 4; j++) {    
            VBlankIntrWait();  
        }
    }
    for (int i = 0; i < 180; i++) {
        VBlankIntrWait();
    }
    for (int frame = 0; frame <= 16; frame++) {
        REG_BLDY = BLDY_BUILD(frame); 
        for (int j = 0; j < 4; j++) {    
            VBlankIntrWait();  
        }
    }
    REG_BLDCNT = 0;
    REG_BLDY = 0;
    REG_BG1CNT &= ~(BG_CBB(2) | BG_SBB(29) | BG_8BPP | BG_REG_32x32 | BG_PRIO(1));
    REG_DISPCNT &= ~(DCNT_MODE(1) | DCNT_BG1);    


    shell_init();
    GameState prev_state = shell_state();
    shell_render_engine_init();
    PlayerInput input = {0};
    // Load title screen tiles in CBB2
    memcpy32(tile8_mem[2], logoTiles, logoTilesLen / 4);
    // Load title screen tilemap in SBB 23
    memcpy16(se_mem[29], logoMap, logoMapLen / 2);
    // Load title screen palette
    memcpy16(&pal_bg_mem[HUD_SPEED_RULER_PAL_IDX + 1], logoPal, logoPalLen / 2);
    //pal_bg_mem[HUD_SPEED_RULER_PAL_IDX] = 0x0; // Black TODO: Check if still needed 
    // Load hud sprites
    hud_load_gfx();
    spotlight_init_gfx();
    // Configure BG1 and priority 0
    REG_BG1CNT = BG_CBB(2) | BG_SBB(29) | BG_8BPP | BG_REG_32x32 | BG_PRIO(1);
    // Set regular background (Mode 1, BG0)
    REG_DISPCNT = DCNT_MODE(1) | DCNT_BG0;
    // Initialize sprites
    oam_init(obj_buffer, MAX_SPRITES);
    int n_obj = hud_init(obj_buffer, 0);
    int spotlight_slot = n_obj;
    spotlight_init_objs(obj_buffer, spotlight_slot);
    int total_obj = n_obj + SPOTLIGHT_OBJ_COUNT;
    sfx_init();
    bool result_sent = false;
    uint16_t action = 0;
    GameResult result;
    PauseSubState last_pause_choice = SUB_RESUME;
    // Prevents A held during menu confirm from triggering thrust on gameplay entry
    bool suppress_thrust_until_release = false;
    while(1) {
        GameState cur_state = shell_state(); // Update Current state
        bool entered_title = (cur_state == STATE_TITLE && prev_state != STATE_TITLE);
        bool left_title = (cur_state != STATE_TITLE && prev_state == STATE_TITLE);
        // True = in play state coming from title/config/pause screen
        bool entered_gameplay = cur_state == STATE_GAMEPLAY && prev_state != STATE_GAMEPLAY;
        bool fresh_start = entered_gameplay && (prev_state == STATE_CONFIG_SELECTION || last_pause_choice == SUB_RESTART || prev_state == STATE_FIN);
        key_poll(); // Check key status
        if (cur_state != STATE_GAME_MODE_SELECTION) {
            REG_DISPCNT |= DCNT_BG1;
        }
        else {
            REG_DISPCNT &= ~(DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D);
        }

        if(cur_state == STATE_GAMEPLAY) {
            pal_bg_mem[HUD_SPEED_RULER_PAL_IDX] = HUD_1Pal[25];
            REG_DISPCNT |= DCNT_OBJ | DCNT_OBJ_1D;
            bool radar_active = lander.radar_on && (lander.z < FIX_FROM_INT(200));
            if (!is_night_mode() || lander.light_on || radar_active) {
                REG_DISPCNT |= DCNT_BG2;
            } else {
                REG_DISPCNT &= ~DCNT_BG2;
            }
            for (int i = post_fuel_power_idx; i < digit_sprite_idx_end; i++) {
                obj_unhide(&obj_buffer[i], ATTR0_REG);
            }
        }
        else { // Deactivate hud and level background
            pal_bg_mem[0] = 0x0;
            if (cur_state != STATE_TITLE) {
                pal_bg_mem[HUD_SPEED_RULER_PAL_IDX] = 0x0;
            }
            for (int i = post_fuel_power_idx; i < MAX_SPRITES; i++) {
                obj_hide(&obj_buffer[i]);
            }
            REG_DISPCNT &= ~(DCNT_BG2 | DCNT_WIN0 | DCNT_WIN1 | DCNT_WINOBJ);
            REG_BLDCNT = 0;
            REG_BLDY = 0;
            spotlight_set_active(obj_buffer, spotlight_slot, false);
        }

        if (entered_gameplay) {
            tte_erase_screen(); // Hide menu
            pal_bg_mem[0] = is_night_mode() ? 0x0 : moon_far_finPal[0];
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

        if (entered_title) {
            REG_BG1CNT &= ~(BG_CBB(2) | BG_SBB(23) | BG_8BPP | BG_REG_32x32 | BG_PRIO(1));
            memcpy32(tile8_mem[2], logoTiles, logoTilesLen / 4);
            memcpy16(se_mem[29], logoMap, logoMapLen / 2);
            memcpy16(&pal_bg_mem[HUD_SPEED_RULER_PAL_IDX + 1], logoPal, logoPalLen / 2);
            for (int i = 0; i < MAX_SPRITES; i++) {
                obj_hide(&obj_buffer[i]);
            }
            REG_BG1CNT = BG_CBB(2) | BG_SBB(29) | BG_8BPP | BG_REG_32x32 | BG_PRIO(1);
        }
        if (left_title) {
            REG_BG1CNT &= ~(BG_CBB(2) | BG_SBB(29) | BG_8BPP | BG_REG_32x32 | BG_PRIO(1));
            // Load background tiles in CBB0
            memcpy32(tile8_mem[0], moon_far_finTiles, moon_far_finTilesLen / 4);
            // Load background tilemap in SBB 28
            memcpy16(se_mem[28], moon_far_finMap, moon_far_finMapLen / 2);
            // Load background palette
            memcpy16(pal_bg_mem, moon_far_finPal, moon_far_finPalLen / 2);
            // Load HUD tiles in CBB2
            memcpy32(tile8_mem[2], HUD_1Tiles, HUD_1TilesLen / 4);
            // Load HUD tilemap in SBB 23
            memcpy16(se_mem[23], HUD_1Map, HUD_1MapLen / 2);
            // Load HUD palette
            memcpy16(&pal_bg_mem[HUD_BACKGROUND_PAL_BASE], HUD_1Pal, HUD_1PalLen / 2);
            pal_bg_mem[HUD_SPEED_RULER_PAL_IDX + 1 + 26] = 0x0;
            pal_bg_mem[HUD_BACKGROUND_PAL_BASE + 1 + 27] = 0x0;
            REG_BG1CNT = BG_CBB(2) | BG_SBB(23) | BG_8BPP | BG_REG_32x32 | BG_PRIO(1);
            // Configure BG2 with wrap on and priority 3
            REG_BG2CNT = BG_CBB(0) | BG_SBB(28) | BG_AFF_64x64 | BG_WRAP | BG_PRIO(3);
        }
        
        switch (shell_state()) {
            case STATE_GAMEPLAY: {
                input = cpit_input();
                if (suppress_thrust_until_release) {
                    input.thrust_main = false;
                    if (!key_is_down(KEY_A)) {
                        suppress_thrust_until_release = false;
                    }
                }
                shell_feed_input(menu_input()); // To read the Start button
                EngineState engine = input.thrust_main ? ENGINE_MAIN
                                    : (input.rcs_x || input.rcs_y || input.rotate) ? ENGINE_RCS
                                    : ENGINE_OFF;
                if (lander.propellant > 0) {
                    sfx_engine_set(engine);
                }
                bool radar_on = lander.radar_on;
                if (radar_on && !radar_on_prev) {
                    sfx_play(SFX_RADAR);
                }
                radar_on_prev = radar_on;
                bool lander_light = lander.light_on;
                if (lander_light && !lander_light_prev) {
                    sfx_play(SFX_LIGHT);
                }
                else if (!lander_light && lander_light_prev) {
                    sfx_play(SFX_LIGHT);
                }
                lander_light_prev = lander_light;
                bool fast_mode = is_fast_mode();
                GameplayUpdate(&lander, &input, fast_mode, get_active_area_idx(), moon_sites);
                if (lander.state != LANDER_FLYING && !result_sent) {
                    result = GameScoreCreateResult(&lander);
                    shell_submit_result(&result);
                    result_sent = true;
                }
                main_states_management(); // Changes game state to "pause" when Start button is pressed
                shell_commit_input();
                if (shell_state() != STATE_GAMEPLAY) {
                    sfx_engine_set(ENGINE_OFF);
                    pal_bg_mem[0] = 0x0;
                    pal_bg_mem[HUD_SPEED_RULER_PAL_IDX] = 0x0;
                    for (int i = post_fuel_power_idx; i < MAX_SPRITES; i++) {
                        obj_hide(&obj_buffer[i]);
                    }
                    REG_DISPCNT &= ~(DCNT_BG2 | DCNT_WIN0 | DCNT_WIN1 | DCNT_WINOBJ);
                    REG_BLDCNT = 0;
                    REG_BLDY = 0;
                    spotlight_set_active(obj_buffer, spotlight_slot, false);
                    break;
                }
                hud_update(obj_buffer, 0, &lander, &input);
                bool radar_active = lander.radar_on && (lander.z < FIX_FROM_INT(200));
                if (is_night_mode()) {
                    pal_bg_mem[0] = 0x0;
                    if (radar_active) {
                        REG_DISPCNT &= ~(DCNT_WIN0 | DCNT_WIN1 | DCNT_WINOBJ);
                        REG_DISPCNT |= DCNT_BG2;
                        REG_BLDCNT = 0;
                        REG_BLDY = 0;
                        spotlight_set_active(obj_buffer, spotlight_slot, false);
                    } else if (lander.light_on) {
                        REG_DISPCNT &= ~(DCNT_WIN0 | DCNT_WIN1);
                        REG_DISPCNT |= DCNT_WINOBJ | DCNT_BG2;
                        REG_WINOUT = WINOUT_BUILD(WIN_BG0 | WIN_BG1 | WIN_OBJ, WIN_BG0 | WIN_BG1 | WIN_BG2 | WIN_OBJ);
                        REG_BLDCNT = 0;
                        REG_BLDY = 0;
                        spotlight_set_active(obj_buffer, spotlight_slot, true);
                    } else {
                        REG_DISPCNT &= ~(DCNT_WIN0 | DCNT_WIN1 | DCNT_WINOBJ | DCNT_BG2);
                        REG_BLDCNT = 0;
                        REG_BLDY = 0;
                        spotlight_set_active(obj_buffer, spotlight_slot, false);
                    }
                } else {
                    REG_DISPCNT &= ~(DCNT_WIN0 | DCNT_WIN1 | DCNT_WINOBJ);
                    REG_DISPCNT |= DCNT_BG2;
                    REG_BLDCNT = 0;
                    REG_BLDY = 0;
                    spotlight_set_active(obj_buffer, spotlight_slot, false);
                }
                // Configure BG Affine 2
                map_swap(&lander);
                lander_to_affine_src(&lander, &affine_src);
                bg_rotscale_ex(&affine_bg, &affine_src);
                if (is_night_mode()) {
                    pal_bg_mem[0] = 0x0;
                }
                break;
            }
            case STATE_PAUSE:
                shell_render_display();
                action = menu_input();
                shell_feed_input(action); // Reads input
                if (action & M_CONFIRM) {
                    sfx_play(SFX_SELECTION);
                }
                else if (action & M_RETURN) {
                    sfx_play(SFX_BACK);
                }
                else if ((action & M_DOWN) || (action & M_UP)
                        || (action & M_LEFT) || (action & M_RIGHT)) {
                            sfx_play(SFX_DPAD);
                }
                main_states_management(); // Changes game state (Title/Gameplay/Pause)
                sub_states_management(); // Changes game substate (Resume/Restart/Title/Credits)
                shell_commit_input();
                last_pause_choice = pause_state(); // Save current substate
                break;
            default:
                shell_render_display();
                shell_feed_input(menu_input());
                action = menu_input();
                if (action & M_CONFIRM) {
                    sfx_play(SFX_SELECTION);
                }
                else if (action & M_RETURN) {
                    sfx_play(SFX_BACK);
                }
                else if ((action & M_DOWN) || (action & M_UP)
                        || (action & M_LEFT) || (action & M_RIGHT)) {
                            sfx_play(SFX_DPAD);
                }
                main_states_management();
                sub_states_management();
                shell_commit_input();
                break;
        }
        prev_state = cur_state; // Update previous state
        VBlankIntrWait(); // Wait VBlank
        sfx_update();
        oam_copy(oam_mem, obj_buffer, total_obj); // Copy sprites in oam
        REG_BG_AFFINE[2] = affine_bg; // Update affine bg register
    }
}
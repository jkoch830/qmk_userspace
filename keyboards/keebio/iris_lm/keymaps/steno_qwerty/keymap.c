// Copyright 2023 Danny Nguyen (@nooges)
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Iris LM: QWERTY-first with toggled Lapwing/steno and Game modes.
// Steno toggle: hold Cmd, then chord A + S + D + F.
// Game toggle:  hold Cmd, then chord Q + W + E + R.
// Steno keys speak GeminiPR to Plover; some bottom-row keys stay HID or blank.
// Game mode is QWERTY with Space/Lower/Upper/Enter thumb order.

#include QMK_KEYBOARD_H

enum custom_layers {
    // Base modes must sit below Lower/Raise so momentary overlays win.
    _QWERTY = 0,
    _STENO,
    _GAME,
    _LOWER,
    _RAISE,
    _ADJUST,
    // Steno-only momentary overlays (above Adjust so they win over base steno).
    _COMPUTER, // hold Ctrl/Cmd/Alt → full QWERTY + that mod (OS shortcuts)
    _SYM_HOME, // hold Home ≈ Lower symbols
    _SYM_END,  // hold End ≈ Raise symbols
};

enum custom_keycodes {
    // Steno-mode keys that emit Lapwing strokes (spacing/attach via Plover).
    ST_LPRN = SAFE_RANGE, // 9 → #-L, or Shift+9 → PREPB  (
    ST_RPRN,               // 0 → #E,  or Shift+0 → PR*EPB )
    ST_BSPC,               // PW-FP  backspace
    ST_TAB,                // TA*B   tab
    // On QWERTY Lower: HID. On steno Home: Gemini (Emily / Lapwing).
    SYM_PIPE, // | ; Shift → \
    SYM_EQL,  // = ; Shift → +
    // Steno Home only — always Gemini strokes (not HID).
    SYM_QUOT, // ' / KR-RG; Shift → " / KW-GS; with * → KR*RG / KW*GS
    SYM_GRV,  // ` / KH-FG; Shift → ~ (Emily); with * → KH*FG
    SYM_STAR, // Flag only (*); does not send Gemini alone
    SYM_LBRC, // [ / PWR-BGT; Shift → { / TPR-BGT
    SYM_RBRC, // ] / PWR*BGT; Shift → } / TPR*BGT
    SYM_MINS, // - / H-PB; Shift → _ / Emily BG
};

enum combos {
    // Same physical chords; keycode variants per active base layer.
    COMBO_STENO_TOGGLE_QWERTY,
    COMBO_STENO_TOGGLE_STENO,
    COMBO_GAME_TOGGLE_QWERTY,
    COMBO_GAME_TOGGLE_STENO,
};

// Cmd is NOT in these arrays on purpose. QMK buffers every combo member for
// COMBO_TERM; putting A/S/D/F/Q/W/E/R in combos made WASD lag ~200ms in games.
// Hold Cmd first (registers as a normal mod), then chord the letters.
const uint16_t PROGMEM steno_toggle_qwerty[] = {KC_A, KC_S, KC_D, KC_F, COMBO_END};

// Exit while STN_* keycodes are active (Lapwing: A=S2, S=K, D=W, F=R).
const uint16_t PROGMEM steno_toggle_steno[] = {STN_S2, STN_KL, STN_WL, STN_RL, COMBO_END};

// Game chord works on QWERTY and Game (same letter keycodes).
const uint16_t PROGMEM game_toggle_qwerty[] = {KC_Q, KC_W, KC_E, KC_R, COMBO_END};

// From Steno: Q=#/S1, W=T-, E=P-, R=H- (all STN_* — mixed HID+STN combos are unreliable).
const uint16_t PROGMEM game_toggle_steno[] = {STN_S1, STN_TL, STN_PL, STN_HL, COMBO_END};

combo_t key_combos[] = {
    [COMBO_STENO_TOGGLE_QWERTY] = COMBO_ACTION(steno_toggle_qwerty),
    [COMBO_STENO_TOGGLE_STENO]  = COMBO_ACTION(steno_toggle_steno),
    [COMBO_GAME_TOGGLE_QWERTY]  = COMBO_ACTION(game_toggle_qwerty),
    [COMBO_GAME_TOGGLE_STENO]   = COMBO_ACTION(game_toggle_steno),
};

// Arm letter combos only while Cmd is held, and only for the active base's
// keycode set. Without Cmd, WASD/QWER are normal keys (no combo buffering).
// On steno, Cmd uses LM(_COMPUTER) so letters become KC_* — use the QWERTY
// combo variants while _COMPUTER is on.
bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
    if (!((get_mods() | get_oneshot_mods()) & MOD_BIT(KC_LGUI))) {
        return false;
    }
    const bool in_steno   = layer_state_cmp(default_layer_state, _STENO);
    const bool computer   = IS_LAYER_ON(_COMPUTER);
    switch (combo_index) {
        case COMBO_STENO_TOGGLE_STENO:
        case COMBO_GAME_TOGGLE_STENO:
            // Raw STN_* letter combos only when not mirrored to QWERTY.
            return in_steno && !computer;
        case COMBO_STENO_TOGGLE_QWERTY:
        case COMBO_GAME_TOGGLE_QWERTY:
            // QWERTY/Game bases, or steno-with-Cmd (computer layer).
            return !in_steno || computer;
    }
    return true;
}

// Each mode chord toggles that mode ↔ QWERTY. Crossing modes (e.g. Game→Steno)
// is just "not in target → enter target" on the destination chord.
static void toggle_steno_mode(void) {
    if (layer_state_cmp(default_layer_state, _STENO)) {
        default_layer_set((layer_state_t)1 << _QWERTY);
        layer_move(_QWERTY);
    } else {
        default_layer_set((layer_state_t)1 << _STENO);
        layer_move(_STENO);
    }
}

static void toggle_game_mode(void) {
    if (layer_state_cmp(default_layer_state, _GAME)) {
        default_layer_set((layer_state_t)1 << _QWERTY);
        layer_move(_QWERTY);
    } else {
        default_layer_set((layer_state_t)1 << _GAME);
        layer_move(_GAME);
    }
}

void process_combo_event(uint16_t combo_index, bool pressed) {
    if (!pressed) {
        return;
    }
    switch (combo_index) {
        case COMBO_STENO_TOGGLE_QWERTY:
        case COMBO_STENO_TOGGLE_STENO:
            toggle_steno_mode();
            break;
        case COMBO_GAME_TOGGLE_QWERTY:
        case COMBO_GAME_TOGGLE_STENO:
            toggle_game_mode();
            break;
    }
}

// Soft jump into STM32 DFU can fail if invoked directly from the key/USB
// path on the right half. Defer so the USB stack can settle first.
static uint32_t deferred_bootloader_jump(uint32_t trigger_time, void *cb_arg) {
    bootloader_jump();
    return 0;
}

// Mac/iOS: Opt+←/→ = word. Windows/Linux: Ctrl+←/→ = word.
// OS_UNSURE defaults to Opt (this userspace is Mac-first).
static uint8_t word_nav_mod(void) {
    switch (detected_host_os()) {
        case OS_WINDOWS:
        case OS_LINUX:
            return KC_LCTL;
        default:
            return KC_LALT;
    }
}

static uint8_t word_mod_held = 0;

// Build/send a GeminiPR packet without touching the live chord state
// (send_custom_steno_chord is not in this QMK tree yet).
extern void steno_add_key_to_chord_gemini(uint8_t chord[MAX_STROKE_SIZE], uint8_t key);
extern void steno_send_chord_gemini(uint8_t chord[MAX_STROKE_SIZE]);

static void send_gemini_stroke(const uint16_t *keys, uint8_t n) {
    uint8_t chord[MAX_STROKE_SIZE] = {0};
    for (uint8_t i = 0; i < n; i++) {
        steno_add_key_to_chord_gemini(chord, keys[i] - QK_STENO_FUNCTION);
    }
    steno_send_chord_gemini(chord);
}

// Drop Shift for the send so Plover's HID output isn't host-shifted; restore after.
static void send_gemini_stroke_unshifted(const uint16_t *keys, uint8_t n) {
    const uint8_t mods = get_mods();
    const uint8_t oneshot = get_oneshot_mods();
    if (mods & MOD_MASK_SHIFT) {
        del_mods(mods & MOD_MASK_SHIFT);
    }
    if (oneshot & MOD_MASK_SHIFT) {
        del_oneshot_mods(oneshot & MOD_MASK_SHIFT);
    }
    send_gemini_stroke(keys, n);
    set_mods(mods);
}

// Lapwing outlines (lapwing-base / lapwing-numbers / commands).
// # is Gemini S1 — Plover Lapwing remaps machine S1 → #.
static const uint16_t stroke_lprn[] = {STN_PL, STN_RL, STN_E, STN_PR, STN_BR};                   // PREPB
static const uint16_t stroke_rprn[] = {STN_PL, STN_RL, STN_ST1, STN_E, STN_PR, STN_BR};         // PR*EPB
static const uint16_t stroke_bspc[] = {STN_PL, STN_WL, STN_FR, STN_PR};                         // PW-FP
static const uint16_t stroke_tab[]  = {STN_TL, STN_A, STN_ST1, STN_BR};                         // TA*B
// RH numberpad (lapwing-numbers.json): glue digits → "with 123 with".
static const uint16_t stroke_1[]    = {STN_S1, STN_RR};                                         // #-R
static const uint16_t stroke_2[]    = {STN_S1, STN_BR};                                         // #-B
static const uint16_t stroke_3[]    = {STN_S1, STN_GR};                                         // #-G
static const uint16_t stroke_4[]    = {STN_S1, STN_FR, STN_RR};                                  // #-FR
static const uint16_t stroke_5[]    = {STN_S1, STN_PR, STN_BR};                                  // #-PB
static const uint16_t stroke_6[]    = {STN_S1, STN_LR, STN_GR};                                  // #-LG
static const uint16_t stroke_7[]    = {STN_S1, STN_FR};                                          // #-F
static const uint16_t stroke_8[]    = {STN_S1, STN_PR};                                          // #-P
static const uint16_t stroke_9[]    = {STN_S1, STN_LR};                                          // #-L
static const uint16_t stroke_0[]    = {STN_S1, STN_E};                                           // #E
// Punctuation (Lapwing ch. 20). Note: R-R is Enter, not period.
static const uint16_t stroke_comm[] = {STN_KL, STN_WL, STN_BR, STN_GR};                         // KW-BG
static const uint16_t stroke_dot[]  = {STN_TL, STN_PL, STN_PR, STN_LR};                          // TP-PL
static const uint16_t stroke_slsh[] = {STN_O, STN_E, STN_U};                                     // OEU
static const uint16_t stroke_ques[] = {STN_KL, STN_WL, STN_PR, STN_LR};                          // KW-PL
// Quote / backtick (Home symbol layer). Closing variants include STN_ST1 as *.
static const uint16_t stroke_quot[]      = {STN_KL, STN_RL, STN_RR, STN_GR};                     // KR-RG
static const uint16_t stroke_quot_cl[]   = {STN_KL, STN_RL, STN_ST1, STN_RR, STN_GR};            // KR*RG
static const uint16_t stroke_dquo[]      = {STN_KL, STN_WL, STN_GR, STN_SR};                     // KW-GS
static const uint16_t stroke_dquo_cl[]   = {STN_KL, STN_WL, STN_ST1, STN_GR, STN_SR};            // KW*GS
static const uint16_t stroke_grv[]       = {STN_KL, STN_HL, STN_FR, STN_GR};                     // KH-FG
static const uint16_t stroke_grv_cl[]    = {STN_KL, STN_HL, STN_ST1, STN_FR, STN_GR};            // KH*FG
// Brackets / hyphen (Lapwing).
static const uint16_t stroke_lbrc[]      = {STN_PL, STN_WL, STN_RL, STN_BR, STN_GR, STN_TR};     // PWR-BGT [
static const uint16_t stroke_lcbr[]      = {STN_TL, STN_PL, STN_RL, STN_BR, STN_GR, STN_TR};     // TPR-BGT {
static const uint16_t stroke_rbrc[]      = {STN_PL, STN_WL, STN_RL, STN_ST1, STN_BR, STN_GR, STN_TR}; // PWR*BGT ]
static const uint16_t stroke_rcbr[]      = {STN_TL, STN_PL, STN_RL, STN_ST1, STN_BR, STN_GR, STN_TR}; // TPR*BGT }
static const uint16_t stroke_hyphen[]    = {STN_HL, STN_PR, STN_BR};                             // H-PB -

// How many SYM_STAR keys (*1–*4) are held on the Home layer.
static uint8_t sym_star_held = 0;

// Emily attach flags: hold A/O thumbs while Shift is down (Shift first), then tap symbol.
static bool sym_attach_a = false;
static bool sym_attach_o = false;

// Emily starter SKWH + symbol shape (+ optional U for <> variants) + A/O attach.
static const uint16_t emily_skwh[] = {STN_S2, STN_KL, STN_WL, STN_HL};
static const uint16_t emily_at[]   = {STN_FR, STN_RR, STN_PR, STN_BR, STN_LR, STN_GR}; // FRPBLG
static const uint16_t emily_hash[] = {STN_FR, STN_RR, STN_LR, STN_GR};                 // FRLG
static const uint16_t emily_dlr[]  = {STN_RR, STN_PR, STN_BR, STN_LR};                 // RPBL
static const uint16_t emily_pct[]  = {STN_FR, STN_RR, STN_PR, STN_BR};                 // FRPB
static const uint16_t emily_circ[] = {STN_RR, STN_PR, STN_GR};                         // RPG
static const uint16_t emily_amp[]  = {STN_FR, STN_BR, STN_GR};                         // FBG
static const uint16_t emily_ast[]  = {STN_LR};                                         // L
static const uint16_t emily_lt[]   = {STN_FR, STN_PR, STN_LR};                         // FPL (+U)
static const uint16_t emily_gt[]   = {STN_RR, STN_BR, STN_GR};                         // RBG (+U)
static const uint16_t emily_eql[]  = {STN_PR, STN_BR, STN_LR, STN_GR};                 // PBLG =
static const uint16_t emily_plus[] = {STN_GR};                                         // G +
static const uint16_t emily_pipe[] = {STN_PR, STN_BR};                                 // PB |
static const uint16_t emily_bsls[] = {STN_FR, STN_BR};                                 // FB backslash
static const uint16_t emily_unds[] = {STN_BR, STN_GR};                                 // BG _
static const uint16_t emily_tild[] = {STN_FR, STN_PR, STN_BR, STN_GR};                 // FPBG ~
static const uint16_t stroke_excl[] = {STN_TL, STN_PL, STN_BR, STN_GR};                // TP-BG (!)

static bool sym_shifted(void) {
    return (get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT;
}

static bool steno_mode_active(void) {
    return layer_state_cmp(default_layer_state, _STENO);
}

// Gemini digit/punct helpers only on raw steno — not computer or Home/End symbol overlays.
static bool steno_typing(void) {
    return steno_mode_active() && !IS_LAYER_ON(_COMPUTER) && !IS_LAYER_ON(_SYM_HOME) && !IS_LAYER_ON(_SYM_END);
}

// While Shift held (or A/O already flagged), thumbs A/O are attach mods — not live vowels.
bool process_steno_user(uint16_t keycode, keyrecord_t *record) {
    if (!steno_typing()) {
        return true;
    }
    if (keycode != STN_A && keycode != STN_O) {
        return true;
    }
    const bool shift = (get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT;
    const bool intercept = shift || (keycode == STN_A && sym_attach_a) || (keycode == STN_O && sym_attach_o);
    if (!intercept) {
        return true;
    }
    if (keycode == STN_A) {
        sym_attach_a = record->event.pressed;
    } else {
        sym_attach_o = record->event.pressed;
    }
    return false;
}

static void send_emily_symbol(const uint16_t *shape, uint8_t n_shape, bool with_u) {
    uint16_t keys[20];
    uint8_t  n = 0;
    for (uint8_t i = 0; i < ARRAY_SIZE(emily_skwh); i++) {
        keys[n++] = emily_skwh[i];
    }
    for (uint8_t i = 0; i < n_shape; i++) {
        keys[n++] = shape[i];
    }
    if (with_u) {
        keys[n++] = STN_U;
    }
    if (sym_attach_a) {
        keys[n++] = STN_A;
    }
    if (sym_attach_o) {
        keys[n++] = STN_O;
    }
    send_gemini_stroke_unshifted(keys, n);
}

static void send_lapwing_digit(uint8_t digit) {
    switch (digit) {
        case 1:
            send_gemini_stroke_unshifted(stroke_1, ARRAY_SIZE(stroke_1));
            break;
        case 2:
            send_gemini_stroke_unshifted(stroke_2, ARRAY_SIZE(stroke_2));
            break;
        case 3:
            send_gemini_stroke_unshifted(stroke_3, ARRAY_SIZE(stroke_3));
            break;
        case 4:
            send_gemini_stroke_unshifted(stroke_4, ARRAY_SIZE(stroke_4));
            break;
        case 5:
            send_gemini_stroke_unshifted(stroke_5, ARRAY_SIZE(stroke_5));
            break;
        case 6:
            send_gemini_stroke_unshifted(stroke_6, ARRAY_SIZE(stroke_6));
            break;
        case 7:
            send_gemini_stroke_unshifted(stroke_7, ARRAY_SIZE(stroke_7));
            break;
        case 8:
            send_gemini_stroke_unshifted(stroke_8, ARRAY_SIZE(stroke_8));
            break;
        case 9:
            send_gemini_stroke_unshifted(stroke_9, ARRAY_SIZE(stroke_9));
            break;
        case 0:
            send_gemini_stroke_unshifted(stroke_0, ARRAY_SIZE(stroke_0));
            break;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        // Lower + Space → word-nav mod (Opt/Ctrl). Space is read from the active
        // base layer, so this follows Game's remapped thumbs automatically.
        case KC_SPC:
            if (IS_LAYER_ON(_LOWER)) {
                if (record->event.pressed) {
                    word_mod_held = word_nav_mod();
                    register_code(word_mod_held);
                } else {
                    unregister_code(word_mod_held);
                    word_mod_held = 0;
                }
                return false;
            }
            break;
        // Steno: digits, or Shift+digit → ! / Emily @#$%^&* (A/O thumbs = attach).
        case KC_1 ... KC_8:
            if (steno_typing()) {
                if (record->event.pressed) {
                    if ((get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT) {
                        switch (keycode) {
                            case KC_1:
                                send_gemini_stroke_unshifted(stroke_excl, ARRAY_SIZE(stroke_excl));
                                break;
                            case KC_2:
                                send_emily_symbol(emily_at, ARRAY_SIZE(emily_at), false);
                                break;
                            case KC_3:
                                send_emily_symbol(emily_hash, ARRAY_SIZE(emily_hash), false);
                                break;
                            case KC_4:
                                send_emily_symbol(emily_dlr, ARRAY_SIZE(emily_dlr), false);
                                break;
                            case KC_5:
                                send_emily_symbol(emily_pct, ARRAY_SIZE(emily_pct), false);
                                break;
                            case KC_6:
                                send_emily_symbol(emily_circ, ARRAY_SIZE(emily_circ), false);
                                break;
                            case KC_7:
                                send_emily_symbol(emily_amp, ARRAY_SIZE(emily_amp), false);
                                break;
                            case KC_8:
                                send_emily_symbol(emily_ast, ARRAY_SIZE(emily_ast), false);
                                break;
                        }
                    } else {
                        send_lapwing_digit((uint8_t)(keycode - KC_1 + 1));
                    }
                }
                return false;
            }
            break;
        case KC_COMM:
            if (steno_typing()) {
                if (record->event.pressed) {
                    if ((get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT) {
                        send_emily_symbol(emily_lt, ARRAY_SIZE(emily_lt), true); // <
                    } else {
                        send_gemini_stroke_unshifted(stroke_comm, ARRAY_SIZE(stroke_comm));
                    }
                }
                return false;
            }
            break;
        case KC_DOT:
            if (steno_typing()) {
                if (record->event.pressed) {
                    if ((get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT) {
                        send_emily_symbol(emily_gt, ARRAY_SIZE(emily_gt), true); // >
                    } else {
                        send_gemini_stroke_unshifted(stroke_dot, ARRAY_SIZE(stroke_dot));
                    }
                }
                return false;
            }
            break;
        case KC_SLSH:
            if (steno_typing()) {
                if (record->event.pressed) {
                    if ((get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT) {
                        send_gemini_stroke_unshifted(stroke_ques, ARRAY_SIZE(stroke_ques));
                    } else {
                        send_gemini_stroke(stroke_slsh, ARRAY_SIZE(stroke_slsh));
                    }
                }
                return false;
            }
            break;
        case ST_LPRN:
            if (record->event.pressed) {
                if ((get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT) {
                    send_gemini_stroke_unshifted(stroke_lprn, ARRAY_SIZE(stroke_lprn));
                } else {
                    send_lapwing_digit(9);
                }
            }
            return false;
        case ST_RPRN:
            if (record->event.pressed) {
                if ((get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT) {
                    send_gemini_stroke_unshifted(stroke_rprn, ARRAY_SIZE(stroke_rprn));
                } else {
                    send_lapwing_digit(0);
                }
            }
            return false;
        case ST_BSPC:
            if (record->event.pressed) {
                send_gemini_stroke(stroke_bspc, ARRAY_SIZE(stroke_bspc));
            }
            return false;
        case ST_TAB:
            if (record->event.pressed) {
                send_gemini_stroke(stroke_tab, ARRAY_SIZE(stroke_tab));
            }
            return false;
        case SYM_PIPE:
            // QWERTY Lower: HID |/\ . Steno Home: Emily PB / FB.
            if (record->event.pressed) {
                if (IS_LAYER_ON(_SYM_HOME)) {
                    if (sym_shifted()) {
                        send_emily_symbol(emily_bsls, ARRAY_SIZE(emily_bsls), false);
                    } else {
                        send_emily_symbol(emily_pipe, ARRAY_SIZE(emily_pipe), false);
                    }
                } else {
                    const uint8_t mods    = get_mods();
                    const uint8_t oneshot = get_oneshot_mods();
                    if (sym_shifted()) {
                        if (mods & MOD_MASK_SHIFT) {
                            del_mods(mods & MOD_MASK_SHIFT);
                        }
                        if (oneshot & MOD_MASK_SHIFT) {
                            del_oneshot_mods(oneshot & MOD_MASK_SHIFT);
                        }
                        tap_code(KC_BSLS);
                        set_mods(mods);
                    } else {
                        tap_code16(KC_PIPE);
                    }
                }
            }
            return false;
        case SYM_EQL:
            // QWERTY Lower: HID =/+ . Steno Home: Emily PBLG / G.
            if (record->event.pressed) {
                if (IS_LAYER_ON(_SYM_HOME)) {
                    if (sym_shifted()) {
                        send_emily_symbol(emily_plus, ARRAY_SIZE(emily_plus), false);
                    } else {
                        send_emily_symbol(emily_eql, ARRAY_SIZE(emily_eql), false);
                    }
                } else {
                    const uint8_t mods    = get_mods();
                    const uint8_t oneshot = get_oneshot_mods();
                    if (sym_shifted()) {
                        if (mods & MOD_MASK_SHIFT) {
                            del_mods(mods & MOD_MASK_SHIFT);
                        }
                        if (oneshot & MOD_MASK_SHIFT) {
                            del_oneshot_mods(oneshot & MOD_MASK_SHIFT);
                        }
                        tap_code16(KC_PLUS);
                        set_mods(mods);
                    } else {
                        tap_code(KC_EQL);
                    }
                }
            }
            return false;
        case SYM_STAR:
            // Close-modifier only; never emit a solo * stroke.
            if (record->event.pressed) {
                if (sym_star_held < 255) {
                    sym_star_held++;
                }
            } else if (sym_star_held > 0) {
                sym_star_held--;
            }
            return false;
        case SYM_QUOT:
            // ' / " open, or KR*RG / KW*GS when any Home-layer * is held.
            if (record->event.pressed) {
                const bool closing = sym_star_held > 0;
                if ((get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT) {
                    send_gemini_stroke_unshifted(closing ? stroke_dquo_cl : stroke_dquo,
                                                 closing ? ARRAY_SIZE(stroke_dquo_cl) : ARRAY_SIZE(stroke_dquo));
                } else if (closing) {
                    send_gemini_stroke(stroke_quot_cl, ARRAY_SIZE(stroke_quot_cl));
                } else {
                    send_gemini_stroke(stroke_quot, ARRAY_SIZE(stroke_quot));
                }
            }
            return false;
        case SYM_GRV:
            // ` open (KH-FG), KH*FG when * held; Shift → Emily ~.
            if (record->event.pressed) {
                if (sym_shifted()) {
                    send_emily_symbol(emily_tild, ARRAY_SIZE(emily_tild), false);
                } else if (sym_star_held > 0) {
                    send_gemini_stroke(stroke_grv_cl, ARRAY_SIZE(stroke_grv_cl));
                } else {
                    send_gemini_stroke(stroke_grv, ARRAY_SIZE(stroke_grv));
                }
            }
            return false;
        case SYM_LBRC:
            if (record->event.pressed) {
                if (sym_shifted()) {
                    send_gemini_stroke_unshifted(stroke_lcbr, ARRAY_SIZE(stroke_lcbr));
                } else {
                    send_gemini_stroke(stroke_lbrc, ARRAY_SIZE(stroke_lbrc));
                }
            }
            return false;
        case SYM_RBRC:
            if (record->event.pressed) {
                if (sym_shifted()) {
                    send_gemini_stroke_unshifted(stroke_rcbr, ARRAY_SIZE(stroke_rcbr));
                } else {
                    send_gemini_stroke(stroke_rbrc, ARRAY_SIZE(stroke_rbrc));
                }
            }
            return false;
        case SYM_MINS:
            // - (H-PB); Shift → _ (Emily BG). Home+Shift+Minus = underscore.
            if (record->event.pressed) {
                if (sym_shifted()) {
                    send_emily_symbol(emily_unds, ARRAY_SIZE(emily_unds), false);
                } else {
                    send_gemini_stroke(stroke_hyphen, ARRAY_SIZE(stroke_hyphen));
                }
            }
            return false;
        case QK_BOOT:
            if (record->event.pressed) {
                clear_keyboard();
#ifdef RGB_MATRIX_ENABLE
                rgb_matrix_set_suspend_state(true);
#endif
                defer_exec(500, deferred_bootloader_jump, NULL);
            }
            return false;
    }
    return true;
}

// Drop close-modifier state if Home layer turns off mid-hold (avoids sticky *).
layer_state_t layer_state_set_user(layer_state_t state) {
    if (!layer_state_cmp(state, _SYM_HOME)) {
        sym_star_held = 0;
    }
    return state;
}

// Force NKRO every boot on both halves. NKRO_DEFAULT_ON only applies when
// EEPROM is first initialized; factory/VIA EEPROM can leave nkro permanently off.
void keyboard_post_init_user(void) {
    if (!keymap_config.nkro) {
        keymap_config.nkro = true;
        eeconfig_update_keymap(&keymap_config);
    }
#ifdef RGB_MATRIX_ENABLE
    // Matrix must stay enabled for indicators; solid black = all LEDs off
    // except what indicators_advanced_user paints.
    rgb_matrix_enable_noeeprom();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(0, 0, 0);
#endif
}

#ifdef RGB_MATRIX_ENABLE
// QWERTY: top-right (Backspace) orange.
// Steno: Backspace + key to its left (0) orange.
// Game:  Backspace + 0 + 9 (three top-right) orange.
// Other LEDs forced dark each frame so leftover effects can't bleed through.
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // Honor RM_TOGG: when the matrix is disabled, leave LEDs fully off.
    if (!rgb_matrix_is_enabled()) {
        return false;
    }
    for (uint8_t i = led_min; i < led_max; i++) {
        rgb_matrix_set_color(i, RGB_OFF);
    }

    const uint8_t led_bspc = g_led_config.matrix_co[5][0];
    const uint8_t led_zero = g_led_config.matrix_co[5][1];
    const uint8_t led_nine = g_led_config.matrix_co[5][2];
    const bool    steno    = layer_state_cmp(default_layer_state, _STENO);
    const bool    game     = layer_state_cmp(default_layer_state, _GAME);

    if (led_bspc != NO_LED && led_bspc >= led_min && led_bspc < led_max) {
        rgb_matrix_set_color(led_bspc, RGB_ORANGE);
    }
    if ((steno || game) && led_zero != NO_LED && led_zero >= led_min && led_zero < led_max) {
        rgb_matrix_set_color(led_zero, RGB_ORANGE);
    }
    if (game && led_nine != NO_LED && led_nine >= led_min && led_nine < led_max) {
        rgb_matrix_set_color(led_nine, RGB_ORANGE);
    }
    return false;
}
#endif // RGB_MATRIX_ENABLE

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /* QWERTY (stock Iris LM default) */
    [_QWERTY] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
       QK_GESC, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                               KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                               KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_DEL,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                               KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
       KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_HOME,          KC_END,  KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
    //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                      KC_LGUI, TL_LOWR, KC_ENT,                    KC_SPC,  TL_UPPR, KC_RALT
                                  // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    /* STENO (GeminiPR → Plover / Lapwing)
     *
     * Lapwing puts # on the former top-S (Gemini S1). Plover's Lapwing setup
     * usually remaps machine key S1 → #. So:
     *   Q → STN_S1  (# via that remap)
     *   A → STN_S2  (the remaining left S)
     *
     * Q W E R T Y U I O P Del  →  # T P H * * F P L T D
     * A S D F G H J K L ; '    →  S K W R * * R B G S Z
     * Lower Enter | Space Raise →  A O | E U
     */
    [_STENO] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
    // 1–8 → #-R…#-P (via process_record); 9/0 → #-L/#E; Shift+9/0 → PREPB/PR*EPB; Bksp → PW-FP
       QK_GESC, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                               KC_6,    KC_7,    KC_8,    ST_LPRN, ST_RPRN, ST_BSPC,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
    // Tab → TA*B
    //         # (S1)   T-       P-       H-       *1       *3       -F       -P       -L       -T       -D
       ST_TAB,  STN_S1,  STN_TL,  STN_PL,  STN_HL,  STN_ST1,                            STN_ST3, STN_FR,  STN_PR,  STN_LR,  STN_TR,  STN_DR,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
    //         S (S2)   K-       W-       R-       *2       *4       -R       -B       -G       -S       -Z
    // Ctrl/Cmd/Alt → LM(_COMPUTER) so hold = QWERTY + mod (OS shortcuts).
       LM(_COMPUTER, MOD_LCTL), STN_S2, STN_KL, STN_WL, STN_RL, STN_ST2,               STN_ST4, STN_RR,  STN_BR,  STN_GR,  STN_SR,  STN_ZR,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
    //         (blank Z–B)              Home≈Lower                           End≈Raise  (blank N M)  , . /
       KC_LSFT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, MO(_SYM_HOME),    MO(_SYM_END), XXXXXXX, XXXXXXX, KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
    //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
    //                               Cmd      A        O                          E        U        Alt
                                      LM(_COMPUTER, MOD_LGUI), STN_A, STN_O,       STN_E,   STN_U,   LM(_COMPUTER, MOD_RALT)
                                  // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    /* GAME — QWERTY with thumbs Space, Lower, Upper, Enter */
    [_GAME] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
       QK_GESC, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                               KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                               KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_DEL,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                               KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
       KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_HOME,          KC_END,  KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
    //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
    //                               Cmd      Space    Lower                      Upper    Enter    Alt
                                      KC_LGUI, KC_SPC,  TL_LOWR,                   TL_UPPR, KC_ENT,  KC_RALT
                                  // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    [_LOWER] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
    //                                                                         [ ] — Shift held → { }     Bksp: = (Shift → +)
       KC_GRV,  KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                            KC_CIRC, KC_AMPR, KC_ASTR, KC_LBRC, KC_RBRC, SYM_EQL,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
    //                                                         Del position: |  (Shift → \)
       KC_TILD, _______, KC_UP,   _______, QK_BOOT, _______,                            _______, KC_P7,   KC_P8,   KC_P9,   KC_P0,   SYM_PIPE,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
    //         Ctrl + ←↓→ on ASD; vim hjkl arrows on right home row  (+ stays; | moved to Del)
       KC_LCTL, KC_LEFT, KC_DOWN, KC_RGHT, _______, KC_LBRC,                            KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_PLUS, _______,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
       KC_LSFT, EE_CLR,  _______, _______, _______, KC_LCBR, KC_LPRN,          KC_RPRN, KC_RCBR, KC_P1,   KC_P2,   KC_P3,   KC_MINS, KC_RSFT,
    //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
    // Space stays transparent so base-layer KC_SPC (QWERTY or Game) is seen;
    // process_record_user turns Lower+Space into the word-nav mod.
                                      _______, _______, _______,                   _______, _______, KC_P0
                                  // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    [_RAISE] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
       KC_F12,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                              KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
    // Upper+Tab = RGB toggle (indicators on/off); other RGB keys removed
       RM_TOGG, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                            KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, QK_BOOT,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       _______, KC_MPRV, KC_MNXT, KC_VOLU, KC_PGUP, KC_UNDS,                            KC_EQL,  KC_HOME, _______, _______, _______, KC_BSLS,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
       KC_MUTE, KC_MSTP, KC_MPLY, KC_VOLD, KC_PGDN, KC_MINS, KC_LPRN,          _______, KC_PLUS, KC_END,  _______, _______, _______, EE_CLR,
    //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                      _______, _______, _______,                   _______, _______, _______
                                  // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    /* Both Lower+Upper: transparent — union of Lower/Raise only. */
    [_ADJUST] = LAYOUT(
       _______, _______, _______, _______, _______, _______,                            _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______, _______,                            _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______, _______,                            _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______, _______, _______, _______, _______,
                                           _______, _______, _______,                   _______, _______, _______
    ),

    /* COMPUTER — momentary QWERTY while Ctrl/Cmd/Alt held in steno (LM). */
    [_COMPUTER] = LAYOUT(
       QK_GESC, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                               KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
       KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                               KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_DEL,
       _______, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                               KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
       KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_HOME,          KC_END,  KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
                                           _______, KC_SPC,  KC_ENT,                    KC_SPC,  KC_ENT,  _______
    ),

    /* SYM_HOME — hold Home in steno. Symbols are Gemini (Lapwing/Emily), not HID.
     * !@#$%^&* unset (use Shift+digit on base). No dedicated ( ) — use Shift+9/0. */
    [_SYM_HOME] = LAYOUT(
    // Esc: ` / ~ ; 9/0: [ ] / { } ; Bksp: = / +
       SYM_GRV, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                            XXXXXXX, XXXXXXX, XXXXXXX, SYM_LBRC, SYM_RBRC, SYM_EQL,
    // * on T/Y ; Del: | / backslash
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, SYM_STAR,                           SYM_STAR, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, SYM_PIPE,
    // * on G/H ; Quote: ' / "
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, SYM_STAR,                           SYM_STAR, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, SYM_QUOT,
    //                                                         Minus: - / Shift _
       KC_LSFT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, SYM_MINS, KC_RSFT,
                                           XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX
    ),

    /* SYM_END — F-keys only (other Raise symbols live on base Shift or Home). */
    [_SYM_END] = LAYOUT(
       KC_F12,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                              KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                            XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                            XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                           XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX
    )
};

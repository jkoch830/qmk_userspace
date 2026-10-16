// Copyright 2023 Danny Nguyen (@nooges)
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Iris LM: QWERTY-first with a toggled Lapwing/steno layer.
// Toggle: chord Left GUI (Cmd) + A + S + D + F simultaneously.
// Steno keys speak GeminiPR to Plover; unlisted keys stay QWERTY.

#include QMK_KEYBOARD_H

enum custom_layers {
    _QWERTY = 0,
    _LOWER,
    _RAISE,
    _STENO,
};

enum custom_keycodes {
    // Lower+Space: Opt on Mac/iOS, Ctrl elsewhere — word-nav with arrows.
    WORD_MOD = SAFE_RANGE,
};

enum combos {
    // Same physical chord; two keycode sets for QWERTY vs STENO layers.
    COMBO_STENO_TOGGLE_QWERTY,
    COMBO_STENO_TOGGLE_STENO,
};

const uint16_t PROGMEM steno_toggle_qwerty[] = {KC_LGUI, KC_A, KC_S, KC_D, KC_F, COMBO_END};

// Exit while STN_* keycodes are active (Lapwing: A=S2, S=K, D=W, F=R).
const uint16_t PROGMEM steno_toggle_steno[] = {KC_LGUI, STN_S2, STN_KL, STN_WL, STN_RL, COMBO_END};

combo_t key_combos[] = {
    [COMBO_STENO_TOGGLE_QWERTY] = COMBO_ACTION(steno_toggle_qwerty),
    [COMBO_STENO_TOGGLE_STENO]  = COMBO_ACTION(steno_toggle_steno),
};

static void toggle_steno_mode(void) {
    if (layer_state_cmp(default_layer_state, _STENO)) {
        default_layer_set((layer_state_t)1 << _QWERTY);
        layer_move(_QWERTY);
    } else {
        default_layer_set((layer_state_t)1 << _STENO);
        layer_move(_STENO);
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

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case WORD_MOD:
            if (record->event.pressed) {
                word_mod_held = word_nav_mod();
                register_code(word_mod_held);
            } else {
                unregister_code(word_mod_held);
                word_mod_held = 0;
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
// Other LEDs forced dark each frame so leftover effects can't bleed through.
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    for (uint8_t i = led_min; i < led_max; i++) {
        rgb_matrix_set_color(i, RGB_OFF);
    }

    const uint8_t led_bspc = g_led_config.matrix_co[5][0];
    const uint8_t led_zero = g_led_config.matrix_co[5][1];
    const bool    steno    = layer_state_cmp(default_layer_state, _STENO);

    if (led_bspc != NO_LED && led_bspc >= led_min && led_bspc < led_max) {
        rgb_matrix_set_color(led_bspc, RGB_ORANGE);
    }
    if (steno && led_zero != NO_LED && led_zero >= led_min && led_zero < led_max) {
        rgb_matrix_set_color(led_zero, RGB_ORANGE);
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

    [_LOWER] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
    //                                                                         [ ] — Shift held → { }
       KC_GRV,  KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                            KC_CIRC, KC_AMPR, KC_ASTR, KC_LBRC, KC_RBRC, KC_PGUP,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       KC_TILD, _______, KC_UP,   _______, QK_BOOT, _______,                            _______, KC_P7,   KC_P8,   KC_P9,   KC_P0,   KC_PGDN,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
    //         Ctrl + ←↓→ on ASD; vim hjkl arrows on right home row
       KC_LCTL, KC_LEFT, KC_DOWN, KC_RGHT, _______, KC_LBRC,                            KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_PLUS, KC_PIPE,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
       KC_LSFT, EE_CLR,  _______, _______, _______, KC_LCBR, KC_LPRN,          KC_RPRN, KC_RCBR, KC_P1,   KC_P2,   KC_P3,   KC_MINS, KC_RSFT,
    //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
    //                                                               Space → WORD_MOD (Opt/Ctrl by OS)
                                      _______, _______, _______,                   WORD_MOD, _______, KC_P0
                                  // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    [_RAISE] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
       KC_F12,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                              KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
    // RGB toggle/mode/hue keys removed — indicators-only lighting
       _______, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                            KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, QK_BOOT,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       _______, KC_MPRV, KC_MNXT, KC_VOLU, KC_PGUP, KC_UNDS,                            KC_EQL,  KC_HOME, _______, _______, _______, KC_BSLS,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
       KC_MUTE, KC_MSTP, KC_MPLY, KC_VOLD, KC_PGDN, KC_MINS, KC_LPRN,          _______, KC_PLUS, KC_END,  _______, _______, _______, EE_CLR,
    //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                      _______, _______, _______,                   _______, _______, _______
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
       QK_GESC, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                               KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
    //         # (S1)   T-       P-       H-       *1       *3       -F       -P       -L       -T       -D
       KC_TAB,  STN_S1,  STN_TL,  STN_PL,  STN_HL,  STN_ST1,                            STN_ST3, STN_FR,  STN_PR,  STN_LR,  STN_TR,  STN_DR,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
    //         S (S2)   K-       W-       R-       *2       *4       -R       -B       -G       -S       -Z
       KC_LCTL, STN_S2,  STN_KL,  STN_WL,  STN_RL,  STN_ST2,                            STN_ST4, STN_RR,  STN_BR,  STN_GR,  STN_SR,  STN_ZR,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
       KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_HOME,          KC_END,  KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
    //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
    //                               Cmd      A        O                          E        U        Alt
                                      KC_LGUI, STN_A,   STN_O,                     STN_E,   STN_U,   KC_RALT
                                  // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    )
};

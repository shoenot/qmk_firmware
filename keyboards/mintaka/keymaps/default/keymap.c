// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdint.h>
#include QMK_KEYBOARD_H
#include "raw_hid.h"
#include "screen.h"

enum custom_keycodes {
    KC_P00 = SAFE_RANGE,
    ENC_INC,
    ENC_DEC,
    LAY_TOG
};

enum layer_names {
    _BASE_LAYER,
    _NUM_LAYER
};

enum layer_names current_layer = 0;

void layer_toggle(void) {
    if (current_layer == 0) {
        layer_move(1);
        current_layer = 1;
    } else {
        layer_move(0);
        current_layer = 0;
    }
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE_LAYER] = LAYOUT(
        LAY_TOG, HYPR(KC_B), HYPR(KC_C), KC_FIND,
        HYPR(KC_E), HYPR(KC_F), HYPR(KC_G), KC_CUT,
        HYPR(KC_I), HYPR(KC_J), KC_AGAIN, KC_COPY,
        HYPR(KC_M), HYPR(KC_N), KC_UNDO, KC_PASTE,           QK_BOOT,
        KC_A, KC_MPRV, KC_MPLY, KC_MNXT,              KC_MUTE
    ),
    [_NUM_LAYER] = LAYOUT(
        LAY_TOG,    _______,    _______,         _______,
        KC_KP_7,        KC_KP_8,    KC_KP_9,     KC_KP_SLASH,
        KC_KP_4,        KC_KP_5,    KC_KP_6,     KC_KP_ASTERISK,
        KC_KP_1,        KC_KP_2,    KC_KP_3,     KC_KP_MINUS,        QK_BOOT,
        _______,    KC_KP_0,    KC_KP_DOT,   KC_KP_PLUS,         _______
    )
};

const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_BASE_LAYER] = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(ENC_INC, ENC_DEC) },
    [_NUM_LAYER] = { ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(KC_RIGHT, KC_LEFT) }
};

// HID Communication

void raw_hid_receive(uint8_t *data, uint8_t length) {
    process_hid_data(data, length);
}

void keyboard_post_init_kb(void) {
    initialize_screen();
}

void housekeeping_task_user(void) {
    check_hid_timeout();
    check_display_timeout();
}

bool shutdown_user(bool jump_to_bootloader) {
    shutdown_screen(jump_to_bootloader);
    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        wake_from_timeout();
        start_display_timer();
        switch(keycode) {
            case ENC_INC:
                enc_counter(false);
                update_screen();
                return false;
            case ENC_DEC:
                enc_counter(true);
                update_screen();
                return false;
            case LAY_TOG:
                layer_toggle();
                update_screen();
                return false;
        }
    }
    return true;
}


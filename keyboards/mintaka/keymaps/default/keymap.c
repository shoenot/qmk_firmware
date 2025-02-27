// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdint.h>
#include QMK_KEYBOARD_H
#include "raw_hid.h"
#include "screen.h"

enum custom_keycodes {
    KC_P00 = SAFE_RANGE,
    ENC_INC,
    ENC_DEC
};


enum layer_names {
    _BASE_LAYER
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE_LAYER] = LAYOUT(
        KC_A,   KC_B,   KC_C,   KC_D,
        KC_E,   KC_F,   KC_G,   KC_H,
        KC_I,   KC_J,   KC_K,   KC_L,
        QK_BOOT,   KC_N,   KC_O,   KC_P,   KC_Q,
        KC_R,   KC_S,   KC_T,   KC_U,   KC_V
    )
};

const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_BASE_LAYER] = { ENCODER_CCW_CW(ENC_INC, ENC_DEC), ENCODER_CCW_CW(MS_WHLU, MS_WHLD) }
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
}

bool shutdown_user(bool jump_to_bootloader) {
    shutdown_screen(jump_to_bootloader);
    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch(keycode) {
            case ENC_INC:
                enc_counter(false);
                return false;
            case ENC_DEC:
                enc_counter(true);
                return false;
        }
        wake_from_screen_timeout();
    }
    return true;
}

void post_encoder_update_user(uint8_t index, bool clockwise) {
    wake_from_screen_timeout();
}

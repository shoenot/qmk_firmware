// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include QMK_KEYBOARD_H
#include <qp.h>
#include "rsz_experience.qgf.h"

#define OLED_HEIGHT 64
#define OLED_WIDTH 128
#define OLED_I2C_ADDRESS 0x3c

enum custom_keycodes {
    KC_P00 = SAFE_RANGE
};

// Headers for raw hid
#include "raw_hid.h"

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
    [_BASE_LAYER] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(MS_WHLU, MS_WHLD) }
};

painter_device_t oled_display;

void keyboard_post_init_kb(void) {
    painter_image_handle_t splash = qp_load_image_mem(gfx_rsz_experience);
    qp_power(oled_display, 1);
    oled_display = qp_sh1106_make_i2c_device(OLED_WIDTH, OLED_HEIGHT, OLED_I2C_ADDRESS);
    qp_init(oled_display, QP_ROTATION_0);
    qp_drawimage(oled_display, 0, 0, splash);
}


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch(keycode) {
            case KC_P00:
                tap_code(KC_P0);
                tap_code(KC_P0);
                return false;
        }
    }
    return true;
}

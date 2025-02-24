// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdint.h>
#include <stdio.h>
#include QMK_KEYBOARD_H
#define SERIAL_SCREEN_BUFFER_LENGTH (/*SSD1306_MatrixCols*/21 * /*SSD1306_MatrixRows*/8 + /*Extra IsEnabledBit*/1)
#define SCREEN_NUM_LINES 8

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

uint8_t screen_data_buffer[SERIAL_SCREEN_BUFFER_LENGTH - 1] = {0};
int oled_line_number = 0;

void raw_hid_receive(uint8_t *data, uint8_t length) {
    memcpy((char *)&screen_data_buffer[oled_line_number * 21], data, 20);
    screen_data_buffer[oled_line_number * 21 - 1] = '\n';
    oled_line_number = (oled_line_number + 1) % SCREEN_NUM_LINES;
    oled_on();
}

void render_text(void) {
    oled_write((char *)&screen_data_buffer, false);
}

bool oled_task_user(void) {
    render_text();
    return false;
}

void oled_render_boot(bool bootloader) {
    oled_clear();
    for (int i = 0; i < 16; i++) {
        oled_set_cursor(0, i);
        if (bootloader) {
            oled_write_P(PSTR("Awaiting New Firmware "), false);
        } else {
            oled_write_P(PSTR("Rebooting "), false);
        }
    }

    oled_render_dirty(true);
}

bool shutdown_user(bool jump_to_bootloader) {
    oled_render_boot(jump_to_bootloader);
    return false;
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

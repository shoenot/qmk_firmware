#include "screen.h"
#include "string.h"
#include <stdint.h>
#include <qp.h>
#include "rsz_experience.qgf.h"
#include "jetbrains.qff.h"
#include "jetbrains_big.qff.h"
#include QMK_KEYBOARD_H

char screen_data_buffer[SCREEN_BUFFER_LENGTH - 1] = {0};
char temp_data_buffer[SCREEN_BUFFER_LENGTH - 1] = {0};
int current_line_number = 0;
uint8_t current_data_screen = 0;
int encoder_index = 0;

painter_device_t oled_display;
painter_font_handle_t display_font;
painter_font_handle_t display_font_big;
painter_image_handle_t boot_splash;
uint32_t hid_timer;
uint32_t screen_timer;
bool display_updated = false;
bool is_screen_timeout = false;

void text_to_buffer(char *text, char *buffer, int length, int line_number) {
    memset(&buffer[line_number * SCREEN_NUM_CHARS], 0, SCREEN_NUM_CHARS);
    memcpy(&buffer[line_number * SCREEN_NUM_CHARS], text, length);
}

void clear_buffer(char *buffer) {
    memset(&buffer, 0, sizeof(buffer));
}

void initialize_screen(void) {
    boot_splash = qp_load_image_mem(gfx_rsz_experience);
    oled_display = qp_sh1106_make_i2c_device(OLED_WIDTH, OLED_HEIGHT, OLED_I2C_ADDRESS);
    display_font = qp_load_font_mem(font_jetbrains);
    display_font_big = qp_load_font_mem(font_jetbrains_big);
    qp_power(oled_display, 1);
    qp_init(oled_display, QP_ROTATION_0);
    qp_drawimage(oled_display, 0, 0, boot_splash);
    screen_timer = timer_read32();
}

void display_write(int start) {
    hid_timer = timer_read32();
    if (display_updated) {
        qp_clear(oled_display);
        for (int i = start, j = 0; i < start + SCREEN_NUM_LINES; i++, j++) {
            qp_drawtext(oled_display, 0, j * 16, display_font, (char *)&screen_data_buffer[i * SCREEN_NUM_CHARS]);
        }
    }
}

void display_write_big(void) {
    // Only does the first two lines, in big font.
    qp_clear(oled_display);
    for (int i = 0; i < 2; i++) {
        qp_drawtext(oled_display, 0, i * 32, display_font_big, (char *)&screen_data_buffer[i * SCREEN_NUM_CHARS]);
    }
}


void enc_counter(bool reverse) {
    // Range from 0 to NUM_SCREENS - 1
    if (!reverse) {
        if (encoder_index < (NUM_SCREENS - 1)) {
            encoder_index++;
        }
    } else {
        if (encoder_index != 0) {
            encoder_index--;
        }
    }
    current_data_screen = encoder_index;
    display_write(current_data_screen * 4);
}

void check_screen_timeout(void) {
    if (!is_screen_timeout && timer_elapsed32(screen_timer) > SCREEN_TIMEOUT) {
        is_screen_timeout = true;
        qp_power(oled_display, 0);
    }
}

void check_hid_timeout(void) {
    if (display_updated && timer_elapsed32(hid_timer) > HID_TIMEOUT) {
        qp_clear(oled_display);
        qp_drawimage(oled_display, 0, 0, boot_splash);
        clear_buffer(screen_data_buffer);
        clear_buffer(temp_data_buffer);
        display_updated = false;
    }
}

void process_timeout(void) {
    screen_timer = timer_read32();
    is_screen_timeout = false;
    qp_power(oled_display, 1);
}


void shutdown_screen(bool jump_to_bootloader) {
    if (jump_to_bootloader) {
        text_to_buffer("Awaiting", screen_data_buffer, 8, 0);
        text_to_buffer("Firmware", screen_data_buffer, 8, 1);
        display_write_big();
    }
    qp_flush(oled_display);
}


void process_hid_data(uint8_t *data, uint8_t length) {
    /********************************************************************************************************
      HID report:

      | - | - - - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - - - |

      First byte is either a 0x01 or a 0x02. 1 indicates that it is the first of the reports, or the first line.
      When the first byte is a 1, the temp_data_buffer is cleared and the line number is reset.
      This is to account for times when the communication is interrupted mid-screen, so after comms are
      reestablished, we want to clear the temp buffer even if its not fully filled.

      Second portion is SCREEN_NUM_CHARS long and is the only data that is being copied to a buffer.

      Third portion is just extra padding, because the exact report rize has to be exactly 32 bytes long.
      This portion is discarded.
     ********************************************************************************************************/

    if (data[0] == '\01') {
        clear_buffer(temp_data_buffer);
        current_line_number = 0;
    }

    text_to_buffer((char *)&data[1], temp_data_buffer, SCREEN_NUM_CHARS, current_line_number);
    current_line_number++;

    if (current_line_number >= SCREEN_NUM_LINES * NUM_SCREENS) {
        clear_buffer(screen_data_buffer);
        memcpy(screen_data_buffer, temp_data_buffer, sizeof(temp_data_buffer));
        current_line_number = 0;
        clear_buffer(temp_data_buffer);
        display_write(current_data_screen * 4);
        display_updated = true;
    }
}



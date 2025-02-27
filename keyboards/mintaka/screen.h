#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

#define OLED_HEIGHT 64
#define OLED_WIDTH 128
#define OLED_I2C_ADDRESS 0x3c
#define SCREEN_NUM_LINES 4
#define SCREEN_NUM_CHARS 19
#define NUM_SCREENS 4
#define SCREEN_BUFFER_LENGTH (SCREEN_NUM_CHARS * SCREEN_NUM_LINES * NUM_SCREENS + 1)
#define HID_TIMEOUT 5000

void clear_buffer(char *buffer);
void text_to_buffer(char *text, char *buffer, int length, int line_number);

void initialize_screen(void);

void show_splash(void);
void display_write(int start);
void display_write_big(void);
void enc_counter(bool reverse);

void process_hid_data(uint8_t *data, uint8_t length);

void check_hid_timeout(void);
void wake_from_screen_timeout(void);
void shutdown_screen(bool jump_to_bootloader);

#endif

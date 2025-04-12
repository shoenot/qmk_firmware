#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

void initialize_screen(void);
void enc_counter(bool reverse);
void process_hid_data(uint8_t *data, uint8_t length);
void check_hid_timeout(void);
void start_display_timer(void);
void check_display_timeout(void);
void wake_from_timeout(void);
void shutdown_screen(bool jump_to_bootloader);
void update_screen(void);

#endif

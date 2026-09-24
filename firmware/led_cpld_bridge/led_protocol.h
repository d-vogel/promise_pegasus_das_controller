#pragma once

#include <stdbool.h>
#include <stdint.h>

void led_init(void);
void led_set_frame(uint16_t frame);
void led_set_markers(uint8_t strobe_a, uint8_t strobe_b);
void led_start(void);
void led_stop(void);
void led_set_sweep(bool enabled);
bool led_is_running(void);
uint16_t led_get_frame(void);
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "led_protocol.h"
#include "pico_pinout.h"

uint16_t press_count = 0;


static void print_help(void) {
    puts("Commands:");
    puts("  set <value>        set 12-bit frame (hex or decimal)");
    puts("  help               show this help text");
}

static uint16_t parse_value(const char *text) {
    if (text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) {
        return (uint16_t)strtoul(text, NULL, 0);
    }
    return (uint16_t)strtoul(text, NULL, 10);
}

uint32_t btn_debounce_time = 0;
void gpio_callback(uint gpio, uint32_t events) {
    if (time_reached(btn_debounce_time)) {
        btn_debounce_time = time_us_32() + 200000;
        if(gpio==FP_SW0) press_count++;
        if(gpio==FP_SW1) press_count++;
    } else {
        return;
    }
    
}

int main(void) {
    stdio_init_all();
    sleep_ms(1500);
    gpio_init(PSU_ON);
    gpio_set_dir(PSU_ON, GPIO_OUT);
    gpio_put(PSU_ON, 0);
    
    gpio_init(HDD12_EN);
    gpio_set_dir(HDD12_EN, GPIO_OUT);
    gpio_put(HDD12_EN, 1);

    gpio_init(HDD34_EN);
    gpio_set_dir(HDD34_EN, GPIO_OUT);
    gpio_put(HDD34_EN, 1);

    gpio_init(PICO_LED);
    gpio_set_dir(PICO_LED, GPIO_OUT);
    gpio_put(PICO_LED, 0);

    gpio_init(FP_LEDB0);
    gpio_set_dir(FP_LEDB0, GPIO_OUT);
    gpio_put(FP_LEDB0, 1);
    gpio_init(FP_LEDB1);
    gpio_set_dir(FP_LEDB1, GPIO_OUT);
    gpio_put(FP_LEDB1, 1);
    gpio_init(FP_LEDB2);
    gpio_set_dir(FP_LEDB2, GPIO_OUT);
    gpio_put(FP_LEDB2, 1);
    gpio_init(FP_RGB_ORANGE);
    gpio_set_dir(FP_RGB_ORANGE, GPIO_OUT);
    gpio_put(FP_RGB_ORANGE, 1);
    gpio_init(FP_RGB_BLUE);
    gpio_set_dir(FP_RGB_BLUE, GPIO_OUT);
    gpio_put(FP_RGB_BLUE, 1);
    gpio_init(FP_RGB_RED);
    gpio_set_dir(FP_RGB_RED, GPIO_OUT);
    gpio_put(FP_RGB_RED, 1);
    gpio_init(FP_SW0);
    gpio_set_dir(FP_SW0, GPIO_IN);
    gpio_init(FP_SW1);
    gpio_set_dir(FP_SW1, GPIO_IN);

    gpio_set_irq_enabled_with_callback(FP_SW0, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(FP_SW1, GPIO_IRQ_EDGE_FALL, true);

    led_init();
    led_start();
    led_set_frame(0x0249);

    print_help();

    char line[64];
    size_t len = 0;
    uint16_t sweep_frame = 1;
    uint32_t pico_led_lastToggle = 0;

    while (true) {
        int ch = getchar_timeout_us(1000);
        if (ch != PICO_ERROR_TIMEOUT) {
            if (ch == '\r' || ch == '\n') {
                line[len] = '\0';
                len = 0;

                char cmd[16];
                char arg1[16];
                char arg2[16];
                if (sscanf(line, "%15s %15s %15s", cmd, arg1, arg2) >= 1) {
                    if (strcmp(cmd, "set") == 0 && sscanf(line, "%*s %15s", arg1) == 1) {
                        uint16_t value = parse_value(arg1) & 0x0FFF;
                        led_set_frame(value);
                        printf("frame set to 0x%03X\n", value);
                    } else if (strcmp(cmd, "help") == 0) {
                        print_help();
                    } else {
                        puts("unknown command");
                    }
                }
            } else if (len < sizeof(line) - 1) {
                line[len++] = (char)ch;
            }
        }

        switch (press_count) {
            case 1:
                gpio_put(FP_LEDB0, 0);
                break;
            case 2:
                gpio_put(FP_LEDB1, 0);
                break;
            case 3:
                gpio_put(FP_LEDB2, 0);
                break;
            case 4:
                gpio_put(FP_RGB_ORANGE, 0);
                gpio_put(FP_RGB_BLUE, 1);
                gpio_put(FP_RGB_RED, 1);
                break;
            case 5:
                gpio_put(FP_RGB_ORANGE, 1);
                gpio_put(FP_RGB_BLUE, 0);
                gpio_put(FP_RGB_RED, 1);
                break;
            case 6:
                gpio_put(FP_RGB_ORANGE, 1);
                gpio_put(FP_RGB_BLUE, 1);
                gpio_put(FP_RGB_RED, 0);
                break;
            case 7:
                gpio_put(FP_RGB_ORANGE, 0);
                gpio_put(FP_RGB_BLUE, 0);
                gpio_put(FP_RGB_RED, 1);
                break;
            case 8:
                gpio_put(FP_RGB_ORANGE, 1);
                gpio_put(FP_RGB_BLUE, 0);
                gpio_put(FP_RGB_RED, 0);
                break;
            case 9:
                gpio_put(FP_RGB_ORANGE, 0);
                gpio_put(FP_RGB_BLUE, 1);
                gpio_put(FP_RGB_RED, 0);
            case 10:
                gpio_put(FP_LEDB0, 1);
                gpio_put(FP_LEDB1, 1);
                gpio_put(FP_LEDB2, 1);
                gpio_put(FP_RGB_ORANGE, 1);
                gpio_put(FP_RGB_BLUE, 1);
                gpio_put(FP_RGB_RED, 1);
                press_count = 0;
                break;

            default:
                break;
        }
        
        if (time_reached(pico_led_lastToggle + 500000)) {
            pico_led_lastToggle = time_us_32();
            gpio_put(PICO_LED, !gpio_get(PICO_LED));
            // printf("Press count: %d\n", press_count);
        }
    }

    return 0;
}

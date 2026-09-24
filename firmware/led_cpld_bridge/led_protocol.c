#include "pico/stdlib.h"
#include "led_protocol.h"
#include "pico_pinout.h"

static volatile uint16_t frame = 0x0249;
static volatile bool running = true;

static void led_bitbang_frame(uint16_t value) {


    for (int bit = 11; bit >= 0; --bit) {
        gpio_put(LED_LATCH, 0);
        gpio_put(LED_DATA, (value >> bit) & 1u);

        if (bit == 0) gpio_put(LED_LATCH, 1);

        gpio_put(LED_CLK, 1);
        sleep_us(8);
        gpio_put(LED_CLK, 0);
        sleep_us(8);

    }
}
// static void led_bitbang_frame(uint16_t value)
// {
//     for (int bit = 11; bit >= 0; --bit) {

//         // LATCH normally low
//         gpio_put(LED_LATCH, 0);

//         // Put data on the bus while CLK is low
//         gpio_put(LED_DATA, (value >> bit) & 1u);
//         if (bit == 0) {
//             // LATCH is asserted immediately before the
//             // falling edge that samples the final bit.
//             gpio_put(LED_LATCH, 1);
//         }
//         // Rising edge
//         gpio_put(LED_CLK, 1);
//         sleep_us(8);

        

//         // Falling edge — CPLD samples DATA here
//         gpio_put(LED_CLK, 0);
//         sleep_us(8);

//         if (bit == 0) {
//             // End the latch pulse
//             gpio_put(LED_LATCH, 0);
//         }
//     }
// }


void led_init(void) {
    gpio_init(LED_CLK);
    gpio_init(LED_DATA);
    gpio_init(LED_LATCH);

    gpio_set_dir(LED_CLK, GPIO_OUT);
    gpio_set_dir(LED_DATA, GPIO_OUT);
    gpio_set_dir(LED_LATCH, GPIO_OUT);

    gpio_put(LED_CLK, 0);
    gpio_put(LED_DATA, 0);
    gpio_put(LED_LATCH, 0);
}

void led_set_frame(uint16_t f) {
    frame = f & 0x0FFF;
    led_bitbang_frame(frame);
}

void led_start(void) {
    running = true;
}

void led_stop(void) {
    running = false;
    gpio_put(LED_LATCH, 0);
}

void led_set_sweep(bool enabled) {
    (void)enabled;
}

bool led_is_running(void) {
    return running;
}

uint16_t led_get_frame(void) {
    return frame;
}
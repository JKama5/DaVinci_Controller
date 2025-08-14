#include <cstdio>
#include "pico/stdlib.h"
#include "servo.hpp"
#include "pot.hpp"

static constexpr uint16_t min_raw = 10;
static constexpr uint16_t max_raw = 4085;
static constexpr uint16_t servo_min = 0;
static constexpr uint16_t servo_max = 180;
static constexpr bool invert_pot = false;

int main() {
    stdio_init_all();
    sleep_ms(300);

    PotConfig cfg;
    cfg.pin = 26;
    pot_init(cfg);

    const uint servo_pin = 15;
    Servo servo;
    servo.attach(servo_pin);

    const uint16_t raw_range = max_raw - min_raw;
    const uint16_t deg_range = servo_max - servo_min;
    if (raw_range == 0) {
        printf("ERROR: max_raw must be > min_raw.\n");
        while (true) tight_loop_contents();
    }

    printf("Linear map raw %u..%u -> %u..%u deg (invert=%s)\n",
           min_raw, max_raw, servo_min, servo_max, invert_pot ? "yes" : "no");

    while (true) {
        uint16_t raw = pot_read_raw12();
        uint16_t value = raw;
        if (value < min_raw) value = min_raw;
        if (value > max_raw) value = max_raw;
        uint16_t delta = value - min_raw;
        if (invert_pot) delta = raw_range - delta;

        uint16_t angle = servo_min + ((uint32_t)delta * deg_range) / raw_range;
        servo.writeAngle(angle);

        static uint32_t last_print = 0;
        uint32_t now_ms = to_ms_since_boot(get_absolute_time());
        if (now_ms - last_print > 250) {
            uint16_t percent = ((uint32_t)delta * 100) / raw_range;
            printf("raw=%4u  angle=%3u (%3u%%)\n", raw, angle, percent);
            last_print = now_ms;
        }

        sleep_us(1000);
    }
}


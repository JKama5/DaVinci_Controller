#include <cstdio>
#include "pico/stdlib.h"
#include "servo.hpp"
#include "pot.hpp"

static constexpr uint16_t min_raw = 10;
static constexpr uint16_t max_raw = 4085;
static constexpr float servo_min = 0.0f;
static constexpr float servo_max = 180.0f;
static constexpr bool invert_pot = false;

static inline float clamp(float x) { return x < 0.f ? 0.f : (x > 1.f ? 1.f : x); }

int main() {
    stdio_init_all();
    sleep_ms(300);

    PotConfig cfg;
    cfg.pin = 26;
    pot_init(cfg);

    const uint servo_pin = 15;
    Servo servo;
    servo.attach(servo_pin);

    const float raw_range = (float)max_raw - (float)min_raw;
    const float deg_range = servo_max - servo_min;
    if (raw_range <= 0.0f) {
        printf("ERROR: max_raw must be > min_raw.\n");
        while (true) tight_loop_contents();
    }

    printf("Linear map raw %u..%u -> %.1f..%.1f deg (invert=%s)\n",
           min_raw, max_raw, servo_min, servo_max, invert_pot ? "yes" : "no");

    while (true) {
        uint16_t raw = pot_read_raw12();
        float x = ((float)raw - (float)min_raw) / raw_range;
        x = clamp(x);
        if (invert_pot) x = 1.0f - x;

        float angle = servo_min + x * deg_range;
        servo.writeAngle(angle);

        static uint32_t last_print = 0;
        uint32_t now_ms = to_ms_since_boot(get_absolute_time());
        if (now_ms - last_print > 250) {
            printf("raw=%4u  x=%.3f  angle=%.1f\n", raw, x, angle);
            last_print = now_ms;
        }

        sleep_us(1000);
    }
}


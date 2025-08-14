#include <cstdio>
#include "pico/stdlib.h"
#include "servo.hpp"
#include "pot.hpp"

// ---------- USER SETTINGS ----------
// Set these to the ACTUAL raw values at each end (see serial prints)
static constexpr uint16_t RAW_MIN = 10;   // <-- replace after measuring
static constexpr uint16_t RAW_MAX = 4085;  // <-- replace after measuring

// Servo travel (use full 0..180 unless you need to limit it)
static constexpr float SERVO_MIN_DEG = 0.0f;
static constexpr float SERVO_MAX_DEG = 180.0f;

// Invert slider direction if needed
static constexpr bool INVERT_POT = false;
// -----------------------------------

static inline float clamp01(float x) { return x < 0.f ? 0.f : (x > 1.f ? 1.f : x); }

int main() {
    stdio_init_all();
    sleep_ms(300);

    // Pot on GPIO26 (ADC0) by default
    PotConfig pc;
    pc.adc_gpio = 26;
    pot_init(pc);

    // Servo on GP15
    const uint SERVO_GPIO = 15;
    Servo s;
    s.attach(SERVO_GPIO);

    const float raw_span = (float)RAW_MAX - (float)RAW_MIN;
    const float deg_span = SERVO_MAX_DEG - SERVO_MIN_DEG;
    if (raw_span <= 0.0f) {
        printf("ERROR: RAW_MAX must be > RAW_MIN.\n");
        while (true) tight_loop_contents();
    }

    printf("Linear map raw %u..%u -> %.1f..%.1f deg (invert=%s)\n",
           RAW_MIN, RAW_MAX, SERVO_MIN_DEG, SERVO_MAX_DEG, INVERT_POT ? "yes" : "no");

    while (true) {
        uint16_t raw = pot_read_raw12();           // 0..4095
        float x = ((float)raw - (float)RAW_MIN) / raw_span;
        x = clamp01(x);
        if (INVERT_POT) x = 1.0f - x;

        float angle = SERVO_MIN_DEG + x * deg_span; // strict linear
        s.writeAngle(angle);

        // Debug ~4 Hz
        static uint32_t tprint = 0;
        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (now - tprint > 250) {
            printf("raw=%4u  x=%.3f  angle=%.1f\n", raw, x, angle);
            tprint = now;
        }

        sleep_us(1000); // fast loop, no filtering
    }
}


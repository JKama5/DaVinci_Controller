#include "pico/stdlib.h"
#include "encoder.hpp"
#include "servo.hpp"
#include <stdio.h>

// ---------------- Pins ----------------
static const uint LED_PIN   = 25;
static const uint ENC_A_PIN = 2;
static const uint ENC_B_PIN = 3;
static const uint SERVO_PIN = 0;

// ---------------- Encoder / Servo state ----------------
static volatile int32_t encoder_count = 0;
static volatile bool encoder_updated = false;

// Servo state
static int servo_angle = 90;   // start centered

// EC11 characteristics
static constexpr int COUNTS_PER_DETENT = 4;   // typical EC11
static constexpr int DEGREES_PER_DETENT = 2;  // tune this

// Encoder ISR callback
void encoder_cb(int32_t count) {
    encoder_count = count;
    encoder_updated = true;
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

    printf("\n=== EC11 → Servo Control ===\n");

    // LED heartbeat
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Encoder
    Encoder encoder(ENC_A_PIN, ENC_B_PIN);
    encoder.init();               // internal pull-ups enabled
    encoder.setCallback(encoder_cb);

    // Servo
    Servo servo;
    servo.attach(SERVO_PIN, 50, 64);  // 50 Hz, safe divider
    servo.center();

    printf("Servo attached and centered\n");

    int32_t last_step = 0;

    while (true) {
        gpio_put(LED_PIN, 1);
        sleep_ms(50);
        gpio_put(LED_PIN, 0);
        sleep_ms(50);

        if (encoder_updated) {
            encoder_updated = false;

            // Convert raw counts → detents
            int32_t step = encoder_count / COUNTS_PER_DETENT;

            // Only act on step changes
            if (step != last_step) {
                int delta = step - last_step;
                last_step = step;

                servo_angle += delta * DEGREES_PER_DETENT;

                // Clamp angle
                if (servo_angle < 0)   servo_angle = 0;
                if (servo_angle > 180) servo_angle = 180;

                servo.writeAngle(servo_angle);

                printf("Counts: %ld | Step: %ld | Angle: %d\n",
                       encoder_count, step, servo_angle);
            }
        }

        tight_loop_contents();
    }
}


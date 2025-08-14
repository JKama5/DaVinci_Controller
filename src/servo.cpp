#include "servo.hpp"

bool Servo::attach(uint gpio_pin, float freq_hz, float clkdiv) {
    if (attached) return true;

    pin = gpio_pin;
    gpio_set_function(pin, GPIO_FUNC_PWM);

    slice = pwm_gpio_to_slice_num(pin);

    float sys_freq = 125000000.0f;
    top = (uint32_t)((sys_freq / (clkdiv * freq_hz)) - 1.0f);

    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv(&cfg, clkdiv);
    pwm_config_set_wrap(&cfg, top);
    pwm_init(slice, &cfg, true);

    period = 1000000.0f / freq_hz;
    attached = true;
    center();
    return true;
}

void Servo::detach() {
    if (!attached) return;
    pwm_set_enabled(slice, false);
    attached = false;
}

void Servo::writeMicroseconds(uint16_t us) {
    if (!attached) return;
    if (us < min_us) us = min_us;
    if (us > max_us) us = max_us;

    uint32_t level = (uint32_t)(((uint64_t)us * (top + 1)) / (uint32_t)period);
    pwm_set_gpio_level(pin, level);
}

void Servo::writeAngle(float degrees) {
    if (degrees < 0.0f) degrees = 0.0f;
    if (degrees > 180.0f) degrees = 180.0f;

    float span = (float)(max_us - min_us);
    uint16_t us = (uint16_t)(min_us + (degrees / 180.0f) * span);
    writeMicroseconds(us);
}

void Servo::center() {
    writeMicroseconds((min_us + max_us) / 2);
}


#include "servo.hpp"

bool Servo::attach(uint gpio_pin, uint32_t freq_hz, uint8_t clkdiv) {
    if (attached) return true;

    pin = gpio_pin;
    gpio_set_function(pin, GPIO_FUNC_PWM);

    slice = pwm_gpio_to_slice_num(pin);

    uint32_t sys_freq = 125000000;
    top = (sys_freq / (clkdiv * freq_hz)) - 1;

    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv_int_frac(&cfg, clkdiv, 0);
    pwm_config_set_wrap(&cfg, top);
    pwm_init(slice, &cfg, true);

    period = 1000000 / freq_hz;
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

void Servo::writeAngle(uint16_t degrees) {
    if (degrees > 180) degrees = 180;

    uint16_t span = max_us - min_us;
    uint16_t us = min_us + (span * degrees) / 180;
    writeMicroseconds(us);
}

void Servo::center() {
    writeMicroseconds((min_us + max_us) / 2);
}


#include "servo.hpp"

bool Servo::attach(uint gpio_pin, float freq_hz, float clkdiv) {
    if (attached_) return true;

    gpio_pin_ = gpio_pin;
    gpio_set_function(gpio_pin_, GPIO_FUNC_PWM);

    slice_num_ = pwm_gpio_to_slice_num(gpio_pin_);

    // PWM freq = 125e6 / (clkdiv * (TOP + 1))
    float f_sys = 125000000.0f;
    top_ = (uint32_t)((f_sys / (clkdiv * freq_hz)) - 1.0f);

    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv(&cfg, clkdiv);
    pwm_config_set_wrap(&cfg, top_);
    pwm_init(slice_num_, &cfg, true);

    period_us_ = 1000000.0f / freq_hz; // e.g., 50 Hz -> 20000 µs
    attached_ = true;
    center();
    return true;
}

void Servo::detach() {
    if (!attached_) return;
    pwm_set_enabled(slice_num_, false);
    attached_ = false;
}

void Servo::writeMicroseconds(uint16_t us) {
    if (!attached_) return;
    if (us < kMinUs) us = kMinUs;
    if (us > kMaxUs) us = kMaxUs;

    uint32_t level = (uint32_t)(((uint64_t)us * (top_ + 1)) / (uint32_t)period_us_);
    pwm_set_gpio_level(gpio_pin_, level);
}

void Servo::writeAngle(float degrees) {
    if (degrees < 0.0f) degrees = 0.0f;
    if (degrees > 180.0f) degrees = 180.0f;

    float span = (float)(kMaxUs - kMinUs);
    uint16_t us = (uint16_t)(kMinUs + (degrees / 180.0f) * span);
    writeMicroseconds(us);
}

void Servo::center() {
    writeMicroseconds((kMinUs + kMaxUs) / 2);
}


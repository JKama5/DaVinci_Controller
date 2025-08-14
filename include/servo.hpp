#pragma once
#include "pico/stdlib.h"
#include "hardware/pwm.h"

class Servo {
public:
    Servo() : pin(255), slice(0), top(0), attached(false), period(20000.0f) {}

    bool attach(uint gpio_pin, float freq_hz = 50.0f, float clkdiv = 64.0f);
    void detach();

    void writeMicroseconds(uint16_t us);
    void writeAngle(float degrees);
    void center();

private:
    uint pin;
    uint slice;
    uint32_t top;
    bool attached;
    float period;

    static constexpr uint16_t min_us = 500;
    static constexpr uint16_t max_us = 2500;
};


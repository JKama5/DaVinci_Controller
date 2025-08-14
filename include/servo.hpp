#pragma once
#include "pico/stdlib.h"
#include "hardware/pwm.h"

class Servo {
public:
    Servo() : pin(255), slice(0), top(0), attached(false), period(20000) {}

    bool attach(uint gpio_pin, uint32_t freq_hz = 50, uint8_t clkdiv = 64);
    void detach();

    void writeMicroseconds(uint16_t us);
    void writeAngle(uint16_t degrees);
    void center();

private:
    uint pin;
    uint slice;
    uint32_t top;
    bool attached;
    uint32_t period; // microseconds

    static constexpr uint16_t min_us = 500;
    static constexpr uint16_t max_us = 2500;
};


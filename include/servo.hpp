#pragma once
#include "pico/stdlib.h"
#include "hardware/pwm.h"

class Servo {
public:
    Servo() : gpio_pin_(255), slice_num_(0), top_(0), attached_(false), period_us_(20000.0f) {}

    bool attach(uint gpio_pin, float freq_hz = 50.0f, float clkdiv = 64.0f);
    void detach();

    void writeMicroseconds(uint16_t us);
    void writeAngle(float degrees);
    void center();

private:
    uint gpio_pin_;
    uint slice_num_;
    uint32_t top_;
    bool attached_;
    float period_us_;

    static constexpr uint16_t kMinUs = 500;   // MG996R safe range
    static constexpr uint16_t kMaxUs = 2500;
};


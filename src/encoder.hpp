#pragma once
#include "pico/stdlib.h"
#include <stdint.h>
#include <atomic>

class Encoder {
public:
    using MoveCallback = void(*)(int32_t);

    Encoder(uint pinA, uint pinB);
    void init();                 // configure GPIOs and enable IRQs
    void setCallback(MoveCallback cb);
    int32_t getCount() const;
    void reset();

    // C-style callback used by the SDK; forwards to the instance
    static void gpio_callback(uint gpio, uint32_t events);

private:
    void handle_irq(uint gpio);

    uint pinA_;
    uint pinB_;
    uint8_t prev_;
    std::atomic<int32_t> count_;
    MoveCallback cb_;

    static Encoder* instance_; // single instance helper
};


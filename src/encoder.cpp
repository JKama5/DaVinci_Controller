#include "encoder.hpp"
#include "hardware/gpio.h"
#include "hardware/irq.h"

// Single-instance pointer for C-style callback
Encoder* Encoder::instance_ = nullptr;

Encoder::Encoder(uint pinA, uint pinB)
: pinA_(pinA),
  pinB_(pinB),
  prev_(0),
  count_(0),
  cb_(nullptr) {
}

void Encoder::init() {
    // Save instance for static callback
    instance_ = this;

    // --- Configure GPIOs ---
    gpio_init(pinA_);
    gpio_init(pinB_);

    gpio_set_dir(pinA_, GPIO_IN);
    gpio_set_dir(pinB_, GPIO_IN);

    // INTERNAL pull-ups (EC11 requires this)
    gpio_pull_up(pinA_);
    gpio_pull_up(pinB_);

    // Initialize previous state AFTER pull-ups are active
    uint8_t a = gpio_get(pinA_);
    uint8_t b = gpio_get(pinB_);
    prev_ = (a << 1) | b;

    // Enable interrupts on BOTH edges for BOTH pins
    gpio_set_irq_enabled_with_callback(
        pinA_,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true,
        &Encoder::gpio_callback
    );

    gpio_set_irq_enabled(
        pinB_,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true
    );
}

void Encoder::setCallback(MoveCallback cb) {
    cb_ = cb;
}

int32_t Encoder::getCount() const {
    return count_.load();
}

void Encoder::reset() {
    count_.store(0);
}

void Encoder::gpio_callback(uint gpio, uint32_t events) {
    if (instance_) {
        instance_->handle_irq(gpio);
    }
}

void Encoder::handle_irq(uint /*gpio*/) {
    // TEMP DEBUG: toggle LED on every interrupt
    gpio_put(25, !gpio_get(25));

    uint8_t a = gpio_get(pinA_);
    uint8_t b = gpio_get(pinB_);
    uint8_t curr = (a << 1) | b;

    static const int8_t table[16] = {
         0, -1, +1,  0,
        +1,  0,  0, -1,
        -1,  0,  0, +1,
         0, +1, -1,  0
    };

    uint8_t idx = (prev_ << 2) | curr;
    int8_t delta = table[idx];

    if (delta != 0) {
        count_.fetch_add(delta);
        if (cb_) cb_(count_.load());
    }

    prev_ = curr;
}


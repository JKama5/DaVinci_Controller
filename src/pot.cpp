#include "pot.hpp"
#include "hardware/adc.h"

static bool initialized = false;

void pot_init(const PotConfig& cfg) {
    if (initialized) return;
    uint channel = (cfg.pin == 26) ? 0 : (cfg.pin == 27) ? 1 : 2;
    adc_init();
    adc_gpio_init(cfg.pin);
    adc_select_input(channel);
    initialized = true;
}

uint16_t pot_read_raw12() { return (uint16_t)adc_read(); }


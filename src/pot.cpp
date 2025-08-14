#include "pot.hpp"
#include "hardware/adc.h"

static bool s_inited = false;

void pot_init(const PotConfig& cfg) {
    if (s_inited) return;
    uint input = (cfg.adc_gpio == 26) ? 0 : (cfg.adc_gpio == 27) ? 1 : 2;
    adc_init();
    adc_gpio_init(cfg.adc_gpio);
    adc_select_input(input);
    s_inited = true;
}

uint16_t pot_read_raw12() { return (uint16_t)adc_read(); }


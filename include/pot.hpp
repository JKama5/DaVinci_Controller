#pragma once
#include "pico/stdlib.h"

struct PotConfig { uint adc_gpio = 26; };
void     pot_init(const PotConfig& cfg);
uint16_t pot_read_raw12(); // 0..4095


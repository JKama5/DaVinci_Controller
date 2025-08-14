#pragma once
#include "pico/stdlib.h"

struct PotConfig { uint pin = 26; };
void pot_init(const PotConfig& cfg);
uint16_t pot_read_raw12();


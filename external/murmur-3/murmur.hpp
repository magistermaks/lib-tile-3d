#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint32_t murmur3_32(const uint8_t* key, size_t len, uint32_t seed);

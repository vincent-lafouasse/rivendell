#pragma once

#include <stddef.h>
#include <stdint.h>

#include "./rivendell/error.h"

struct RvdError rvd_elf32_load(
    const uint8_t* file,
    size_t file_size,
    uint8_t* ram,
    size_t ram_size,
    uint32_t ram_base,
    uint32_t* entry
);

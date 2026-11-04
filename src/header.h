#pragma once

#include "types.h"

struct Header32 {
    enum Endianness endian;
    u32 entry;
    u32 ph_offset;
    u32 ph_num;
    /* enum Machine machine, later when i implement ARM */
};

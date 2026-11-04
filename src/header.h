#pragma once

#include <stddef.h>

#include "rivendell/error.h"
#include "types.h"

struct Header32 {
    enum Endianness endian;
    u32 entry;
    u32 ph_offset;
    u32 ph_num;
    /* enum Machine machine, later when i implement ARM */
};

enum RvdError rvd_parse_header(
    const u8* file,
    size_t file_size,
    struct Header32 out[static 1]
);

#pragma once

#include "types.h"

struct Header {
    /* enum Machine machine */
    u32 entry;
    u32 ph_offset;
    u32 ph_num;
};

struct ProgramHeader {
    /* enum PhType type */
    u32 offset;
    u32 v_addr;
    u32 p_addr;
};

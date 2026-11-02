#pragma once

#include "types.h"

struct Header32 {
    u32 entry;
    u32 ph_offset;
    u32 ph_num;
    /* enum Machine machine, later when i implement ARM */
};

struct ProgramHeader32 {
    /* enum PhType type */
    u32 offset;
    u32 v_addr;
    u32 p_addr;
};

#pragma once

#include "types.h"

struct Header32 {
    enum Endianness endian;
    u32 entry;
    u32 ph_offset;
    u32 ph_num;
    /* enum Machine machine, later when i implement ARM */
};

struct Segment32 {
    /* enum SegmentType type */
    u32 offset;
    u32 v_addr;
    u32 p_addr;
};

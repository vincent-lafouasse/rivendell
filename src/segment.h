#pragma once

#include "types.h"

struct Segment32 {
    /* enum SegmentType type */
    u32 offset;
    u32 v_addr;
    u32 p_addr;
    u32 filesz;
    u32 memsz;
};

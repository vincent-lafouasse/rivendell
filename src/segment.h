#pragma once

#include <stddef.h>

#include "header.h"
#include "raw.h"
#include "types.h"

struct Elf32_Phdr rvd_read_phdr_at(
    const u8* file,
    const struct Header32 hdr[static 1],
    size_t i
);

struct Segment32 {
    u32 offset;
    u32 addr;
    u32 filesz;
    u32 memsz;
};

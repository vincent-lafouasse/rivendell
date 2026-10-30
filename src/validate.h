#pragma once

#include "raw.h"

struct Elf32Header {
    u32 not_sure_yet;
};

enum ElfError validate_ehdr(const struct Elf32_Ehdr raw[static 1],
                            struct Elf32Header out[static 1]);

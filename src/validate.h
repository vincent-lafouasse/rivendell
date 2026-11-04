#pragma once

#include <stddef.h>

#include "raw.h"

enum RvdError rvd_validate_ehdr(
    const struct Elf32_Ehdr hdr[static 1],
    size_t file_size
);

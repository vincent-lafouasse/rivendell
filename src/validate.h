#pragma once

#include "raw.h"

enum RvdError rvd_validate_ehdr(const struct Elf32_Ehdr hdr[static 1]);

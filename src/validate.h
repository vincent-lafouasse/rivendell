#pragma once

#include "raw.h"

enum RvdError validate_ehdr(const struct Elf32_Ehdr hdr[static 1]);

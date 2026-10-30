#pragma once

#include "raw.h"

enum ElfError validate_ehdr(const struct Elf32_Ehdr hdr[static 1]);

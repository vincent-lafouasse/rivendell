#pragma once

#include "raw.h"

// run on the raw bytes before read_ehdr, which needs the endianness
enum ElfError validate_ident(const u8 ident[static EI_NIDENT],
                             enum Endianness endian[static 1]);

// assumes e_ident already passed validate_ident
enum ElfError validate_ehdr(const struct Elf32_Ehdr hdr[static 1]);

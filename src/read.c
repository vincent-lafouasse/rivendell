#include "raw.h"

#include <string.h>

struct Elf32_Ehdr read_ehdr(const u8 data[static 52], enum Endianness endian)
{
    struct Elf32_Ehdr out = {0};

    memcpy(out.e_ident, data, EI_NIDENT);
    return out;
}

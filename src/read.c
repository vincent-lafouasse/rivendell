#include "raw.h"

#include <string.h>

enum ElfError parse_endianness(uint8_t ei_data, enum Endianness out[static 1])
{
    switch (ei_data) {
        case 1:
            *out = ENDIAN_LITTLE;
            return ELF_OK;
        case 2:
            *out = ENDIAN_BIG;
            return ELF_OK;
        default:
            return ELF_BAD_ENDIAN;
    }
}

struct Elf32_Ehdr read_ehdr(const u8 data[static 52], enum Endianness endian)
{
    struct Elf32_Ehdr out = {0};

    memcpy(out.e_ident, data, EI_NIDENT);
    return out;
}

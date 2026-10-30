#include "raw.h"

#include <string.h>

#include <bitcast.h>

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

typedef u16 (*U16_reader)(const u8 bytes[2]);
typedef u32 (*U32_reader)(const u8 bytes[4]);

struct Elf32_Ehdr read_ehdr(const u8 data[static 52], enum Endianness endian)
{
    U16_reader read_u16 =
        (endian == ENDIAN_LITTLE) ? bitcastU16_LE : bitcastU16_BE;
    U32_reader read_u32 =
        (endian == ENDIAN_LITTLE) ? bitcastU32_LE : bitcastU32_BE;

    struct Elf32_Ehdr out = {0};

    memcpy(out.e_ident, data, EI_NIDENT);
    return out;
}

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

struct Elf32_Ehdr read_ehdr(const u8 data[static 52], enum Endianness endian)
{
    u16 (*read_u16)(const u8 bytes[2]);
    u32 (*read_u32)(const u8 bytes[4]);

    switch (endian) {
        case ENDIAN_LITTLE:
            read_u16 = bitcastU16_LE;
            read_u32 = bitcastU32_LE;
            break;
        case ENDIAN_BIG:
            read_u16 = bitcastU16_BE;
            read_u32 = bitcastU32_BE;
            break;
        default:
            __builtin_trap();
            __builtin_unreachable();
    }

    struct Elf32_Ehdr out = {0};

    memcpy(out.e_ident, data, EI_NIDENT);
    return out;
}

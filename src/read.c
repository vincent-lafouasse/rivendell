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
    out.e_type = read_u16(data + EI_NIDENT);
    out.e_machine = read_u16(data + EI_NIDENT + 2);
    out.e_version = read_u32(data + EI_NIDENT + 4);
    out.e_entry = read_u32(data + EI_NIDENT + 8);
    out.e_phoff = read_u32(data + EI_NIDENT + 12);
    out.e_shoff = read_u32(data + EI_NIDENT + 16);
    out.e_flags = read_u32(data + EI_NIDENT + 20);
    out.e_ehsize = read_u16(data + EI_NIDENT + 24);
    out.e_phentsize = read_u16(data + EI_NIDENT + 26);
    out.e_phnum = read_u16(data + EI_NIDENT + 28);
    out.e_shentsize = read_u16(data + EI_NIDENT + 30);
    out.e_shnum = read_u16(data + EI_NIDENT + 32);
    out.e_shstrndx = read_u16(data + EI_NIDENT + 34);
    return out;
}

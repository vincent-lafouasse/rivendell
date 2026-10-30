#include "raw.h"

#include <string.h>

#include <bitcast.h>

#include "debug.h"

typedef u16 (*ReadU16)(const u8 bytes[2]);
typedef u32 (*ReadU32)(const u8 bytes[4]);

struct IntReader {
    ReadU16 read_u16;
    ReadU32 read_u32;
};

static struct IntReader int_reader(enum Endianness endian)
{
    switch (endian) {
        case ENDIAN_LITTLE:
            return (struct IntReader){
                .read_u16 = bitcastU16_LE,
                .read_u32 = bitcastU32_LE,
            };
        case ENDIAN_BIG:
            return (struct IntReader){
                .read_u16 = bitcastU16_BE,
                .read_u32 = bitcastU32_BE,
            };
        default:
            UNREACHABLE();
    }
}

enum Endianness parse_endianness(uint8_t ei_data)
{
    switch (ei_data) {
        case 1:
            return ENDIAN_LITTLE;
        case 2:
            return ENDIAN_BIG;
        default:
            // placeholder so the header can be read, validate_ehdr checks
            // e_ident first and rejects this file with RVD_BAD_ENDIAN
            return ENDIAN_LITTLE;
    }
}

struct Elf32_Ehdr read_ehdr(const u8 data[static 52], enum Endianness endian)
{
    const struct IntReader reader = int_reader(endian);
    struct Elf32_Ehdr out = {0};

    memcpy(out.e_ident, data, EI_NIDENT);
    out.e_type = reader.read_u16(data + EI_NIDENT);
    out.e_machine = reader.read_u16(data + EI_NIDENT + 2);
    out.e_version = reader.read_u32(data + EI_NIDENT + 4);
    out.e_entry = reader.read_u32(data + EI_NIDENT + 8);
    out.e_phoff = reader.read_u32(data + EI_NIDENT + 12);
    out.e_shoff = reader.read_u32(data + EI_NIDENT + 16);
    out.e_flags = reader.read_u32(data + EI_NIDENT + 20);
    out.e_ehsize = reader.read_u16(data + EI_NIDENT + 24);
    out.e_phentsize = reader.read_u16(data + EI_NIDENT + 26);
    out.e_phnum = reader.read_u16(data + EI_NIDENT + 28);
    out.e_shentsize = reader.read_u16(data + EI_NIDENT + 30);
    out.e_shnum = reader.read_u16(data + EI_NIDENT + 32);
    out.e_shstrndx = reader.read_u16(data + EI_NIDENT + 34);
    return out;
}

struct Elf32_Phdr read_phdr(const u8 data[static 32], enum Endianness endian)
{
    const struct IntReader reader = int_reader(endian);
    struct Elf32_Phdr out = {0};

    out.p_type = reader.read_u32(data);
    out.p_offset = reader.read_u32(data + 4);
    out.p_vaddr = reader.read_u32(data + 8);
    out.p_paddr = reader.read_u32(data + 12);
    out.p_filesz = reader.read_u32(data + 16);
    out.p_memsz = reader.read_u32(data + 20);
    out.p_flags = reader.read_u32(data + 24);
    out.p_align = reader.read_u32(data + 28);
    return out;
}

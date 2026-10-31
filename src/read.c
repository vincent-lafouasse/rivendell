#include "raw.h"

#include <assert.h>
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

static u16 take_u16(const u8* data, size_t i[static 1], struct IntReader reader)
{
    // straight to UB, have fun
    const u16 out = reader.read_u16(data + *i);
    *i += 2;
    return out;
}

static u32 take_u32(const u8* data, size_t i[static 1], struct IntReader reader)
{
    // straight to UB, have fun
    const u32 out = reader.read_u32(data + *i);
    *i += 4;
    return out;
}

enum Endianness rvd_parse_endianness(uint8_t ei_data)
{
    switch (ei_data) {
        case 1:
            return ENDIAN_LITTLE;
        case 2:
            return ENDIAN_BIG;
        default:
            // placeholder so the header can be read, rvd_validate_ehdr checks
            // e_ident first and rejects this file with RVD_BAD_ENDIAN
            return ENDIAN_LITTLE;
    }
}

struct Elf32_Ehdr rvd_read_ehdr(const u8 data[static 52],
                                enum Endianness endian)
{
    const struct IntReader reader = int_reader(endian);
    struct Elf32_Ehdr out = {0};

    memcpy(out.e_ident, data, EI_NIDENT);

    size_t i = EI_NIDENT;
    out.e_type = take_u16(data, &i, reader);
    out.e_machine = take_u16(data, &i, reader);
    out.e_version = take_u32(data, &i, reader);
    out.e_entry = take_u32(data, &i, reader);
    out.e_phoff = take_u32(data, &i, reader);
    out.e_shoff = take_u32(data, &i, reader);
    out.e_flags = take_u32(data, &i, reader);
    out.e_ehsize = take_u16(data, &i, reader);
    out.e_phentsize = take_u16(data, &i, reader);
    out.e_phnum = take_u16(data, &i, reader);
    out.e_shentsize = take_u16(data, &i, reader);
    out.e_shnum = take_u16(data, &i, reader);
    out.e_shstrndx = take_u16(data, &i, reader);
    assert(i == 52);
    return out;
}

struct Elf32_Phdr rvd_read_phdr(const u8 data[static 32],
                                enum Endianness endian)
{
    const struct IntReader reader = int_reader(endian);
    struct Elf32_Phdr out = {0};

    size_t i = 0;
    out.p_type = take_u32(data, &i, reader);
    out.p_offset = take_u32(data, &i, reader);
    out.p_vaddr = take_u32(data, &i, reader);
    out.p_paddr = take_u32(data, &i, reader);
    out.p_filesz = take_u32(data, &i, reader);
    out.p_memsz = take_u32(data, &i, reader);
    out.p_flags = take_u32(data, &i, reader);
    out.p_align = take_u32(data, &i, reader);
    assert(i == 32);
    return out;
}

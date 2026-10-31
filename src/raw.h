#pragma once

#include <rivendell/elf.h>

#include "types.h"

#define EI_NIDENT 16

enum Endianness { ENDIAN_LITTLE, ENDIAN_BIG };

struct Elf32_Ehdr {
    u8 e_ident[EI_NIDENT];
    u16 e_type;
    u16 e_machine;
    u32 e_version;
    u32 e_entry;
    u32 e_phoff;
    u32 e_shoff;
    u32 e_flags;
    u16 e_ehsize;
    u16 e_phentsize;
    u16 e_phnum;
    u16 e_shentsize;
    u16 e_shnum;
    u16 e_shstrndx;
};

enum Endianness rvd_parse_endianness(uint8_t ei_data);
struct Elf32_Ehdr rvd_read_ehdr(const u8 data[static 52],
                                enum Endianness endian);

struct Elf32_Phdr {
    u32 p_type;
    u32 p_offset;
    u32 p_vaddr;
    u32 p_paddr;
    u32 p_filesz;
    u32 p_memsz;
    u32 p_flags;
    u32 p_align;
};

struct Elf32_Phdr rvd_read_phdr(const u8 data[static 32],
                                enum Endianness endian);

struct Elf32_Shdr {
    u32 sh_name;
    u32 sh_type;
    u32 sh_flags;
    u32 sh_addr;
    u32 sh_offset;
    u32 sh_size;
    u32 sh_link;
    u32 sh_info;
    u32 sh_addralign;
    u32 sh_entsize;
};

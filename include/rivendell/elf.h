#pragma once

#include <stddef.h>
#include <stdint.h>

enum ElfError {
    ELF_OK,
    ELF_TRUNCATED,

    // e_ident
    ELF_BAD_MAGIC,
    ELF_BAD_CLASS,
    ELF_UNSUPPORTED_CLASS,
    ELF_BAD_ENDIAN,
    ELF_BAD_IDENT_VERSION,
    ELF_BAD_OSABI,

    // file header
    ELF_BAD_TYPE,
    ELF_UNSUPPORTED_TYPE,
    ELF_BAD_MACHINE,
    ELF_UNSUPPORTED_MACHINE,
    ELF_BAD_VERSION,
    ELF_BAD_FLAGS,
    ELF_UNSUPPORTED_FLAGS,
    ELF_BAD_EHSIZE,
    ELF_BAD_PHENTSIZE,
    ELF_NO_SEGMENTS,
    ELF_PHDRS_OUT_OF_FILE,
    ELF_BAD_ENTRY,

    // program headers
    ELF_SEGMENT_OUT_OF_FILE,
    ELF_SEGMENT_OUT_OF_RAM,
    ELF_SEGMENT_FILESZ_GT_MEMSZ,
    ELF_SEGMENT_BAD_ALIGN,
};
const char* elf_error_str(enum ElfError e);

enum ElfError elf32_load(const uint8_t* file,
                         size_t file_size,
                         uint8_t* ram,
                         size_t ram_size,
                         uint32_t ram_base,
                         uint32_t* entry);

#pragma once

#include <stddef.h>
#include <stdint.h>

enum ElfError {
    ELF_OK,
    ELF_TRUNCATED,
    ELF_BAD_MAGIC,
    ELF_BAD_CLASS,
    ELF_BAD_ENDIAN,
    ELF_BAD_MACHINE,
    ELF_BAD_TYPE,
    ELF_SEGMENT_OUT_OF_FILE,
    ELF_SEGMENT_OUT_OF_RAM,
};

enum ElfError elf32_load(const uint8_t* file,
                         size_t file_size,
                         uint8_t* ram,
                         size_t ram_size,
                         uint32_t ram_base,
                         uint32_t* entry);

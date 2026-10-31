#pragma once

#include <stddef.h>
#include <stdint.h>

// pretty much only during validation are errors raised
// so these error variants are mostly validation errors
enum RvdError {
    RVD_OK,
    RVD_TRUNCATED,

    // e_ident
    RVD_BAD_MAGIC,
    RVD_BAD_CLASS,
    RVD_UNSUPPORTED_CLASS,
    RVD_BAD_ENDIAN,
    RVD_BAD_IDENT_VERSION,
    RVD_BAD_OSABI,

    // file header
    RVD_BAD_TYPE,
    RVD_UNSUPPORTED_TYPE,
    RVD_BAD_MACHINE,
    RVD_UNSUPPORTED_MACHINE,
    RVD_BAD_VERSION,
    RVD_BAD_FLAGS,
    RVD_UNSUPPORTED_FLAGS,
    RVD_BAD_EHSIZE,
    RVD_BAD_PHENTSIZE,
    RVD_NO_SEGMENTS,
    RVD_PHDRS_OUT_OF_FILE,
    RVD_BAD_ENTRY,

    // program headers
    RVD_SEGMENT_OUT_OF_FILE,
    RVD_SEGMENT_OUT_OF_RAM,
    RVD_SEGMENT_FILESZ_GT_MEMSZ,
    RVD_SEGMENT_BAD_ALIGN,
};
const char* rvd_error_str(enum RvdError e);

enum RvdError rvd_elf32_load(const uint8_t* file,
                             size_t file_size,
                             uint8_t* ram,
                             size_t ram_size,
                             uint32_t ram_base,
                             uint32_t* entry);

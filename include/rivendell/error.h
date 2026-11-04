#pragma once

#include <stdint.h>

enum RvdErrorKind {
    RVD_OK,
    RVD_TRUNCATED,

    // e_ident
    RVD_BAD_MAGIC,
    RVD_BAD_CLASS,
    RVD_UNSUPPORTED_CLASS,
    RVD_BAD_ENDIAN,
    RVD_BAD_IDENT_VERSION,
    RVD_BAD_OSABI,
    RVD_UNSUPPORTED_OSABI,
    RVD_UNSUPPORTED_ABI_VERSION,

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
    RVD_UNSUPPORTED_PHNUM,
    RVD_PHDRS_OUT_OF_FILE,
    RVD_MISALIGNED_ENTRY,
    RVD_BAD_ENTRY,

    // program headers
    RVD_SEGMENT_OUT_OF_FILE,
    RVD_SEGMENT_OUT_OF_RAM,
    RVD_SEGMENT_FILESZ_GT_MEMSZ,
    RVD_SEGMENT_BAD_ALIGN,
};

// value is informational, a raw number whose meaning depends on kind:
//   RVD_TRUNCATED                     file size
//   RVD_BAD_MAGIC                     e_ident[0..3], first byte highest
//   RVD_*_CLASS                       e_ident[EI_CLASS]
//   RVD_BAD_ENDIAN                    e_ident[EI_DATA]
//   RVD_BAD_IDENT_VERSION             e_ident[EI_VERSION]
//   RVD_*_OSABI                       e_ident[EI_OSABI]
//   RVD_UNSUPPORTED_ABI_VERSION       e_ident[EI_ABIVERSION]
//   RVD_*_TYPE                        e_type
//   RVD_*_MACHINE                     e_machine
//   RVD_BAD_VERSION                   e_version
//   RVD_*_FLAGS                       e_flags
//   RVD_BAD_EHSIZE                    e_ehsize
//   RVD_BAD_PHENTSIZE                 e_phentsize
//   RVD_NO_SEGMENTS                   unused, 0
//   RVD_UNSUPPORTED_PHNUM             e_phnum
//   RVD_PHDRS_OUT_OF_FILE             end offset of the program header table
//   RVD_*_ENTRY                       e_entry
//   RVD_SEGMENT_*                     program header index

struct RvdError {
    enum RvdErrorKind kind;
    uint64_t value;
};

const char* rvd_error_kind_str(enum RvdErrorKind kind);

// longest formatted error is under 100 chars, see rvd_error_format
#define RVD_ERROR_FORMAT_MAX 128

// e.g. "unsupported machine (e_machine 0x3e)". always NUL-terminated,
// returns buffer so it can be passed straight to printf
char* rvd_error_format(
    struct RvdError e,
    char buffer[static RVD_ERROR_FORMAT_MAX]
);

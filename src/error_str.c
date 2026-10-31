#include <rivendell/elf.h>

const char* rvd_error_str(enum RvdError e)
{
    switch (e) {
        case RVD_OK:
            return "ok";
        case RVD_TRUNCATED:
            return "file is truncated";
        case RVD_BAD_MAGIC:
            return "bad ELF magic";
        case RVD_BAD_CLASS:
            return "invalid ELF class";
        case RVD_UNSUPPORTED_CLASS:
            return "unsupported ELF class (expected 32-bit)";
        case RVD_BAD_ENDIAN:
            return "invalid ELF data encoding";
        case RVD_BAD_IDENT_VERSION:
            return "unsupported e_ident version";
        case RVD_BAD_OSABI:
            return "unsupported OS/ABI";
        case RVD_BAD_TYPE:
            return "invalid ELF type";
        case RVD_UNSUPPORTED_TYPE:
            return "unsupported ELF type (expected executable)";
        case RVD_BAD_MACHINE:
            return "invalid machine";
        case RVD_UNSUPPORTED_MACHINE:
            return "unsupported machine";
        case RVD_BAD_VERSION:
            return "unsupported ELF version";
        case RVD_BAD_FLAGS:
            return "invalid processor flags";
        case RVD_UNSUPPORTED_FLAGS:
            return "unsupported processor flags";
        case RVD_BAD_EHSIZE:
            return "bad ELF header size";
        case RVD_BAD_PHENTSIZE:
            return "bad program header entry size";
        case RVD_NO_SEGMENTS:
            return "no program headers";
        case RVD_PHDRS_OUT_OF_FILE:
            return "program header table extends past end of file";
        case RVD_BAD_ENTRY:
            return "entry point is not in a loaded segment";
        case RVD_SEGMENT_OUT_OF_FILE:
            return "segment extends past end of file";
        case RVD_SEGMENT_OUT_OF_RAM:
            return "segment does not fit in RAM";
        case RVD_SEGMENT_FILESZ_GT_MEMSZ:
            return "segment file size exceeds its memory size";
        case RVD_SEGMENT_BAD_ALIGN:
            return "segment alignment is invalid";
    }
    return "unknown error";
}

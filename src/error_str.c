#include <rivendell/elf.h>

const char* elf_error_str(enum ElfError e)
{
    switch (e) {
        case ELF_OK:
            return "ok";
        case ELF_TRUNCATED:
            return "file is truncated";
        case ELF_BAD_MAGIC:
            return "bad ELF magic";
        case ELF_BAD_CLASS:
            return "unsupported ELF class (expected 32-bit)";
        case ELF_BAD_ENDIAN:
            return "invalid ELF data encoding";
        case ELF_BAD_MACHINE:
            return "unsupported machine";
        case ELF_BAD_TYPE:
            return "unsupported ELF type";
        case ELF_SEGMENT_OUT_OF_FILE:
            return "segment extends past end of file";
        case ELF_SEGMENT_OUT_OF_RAM:
            return "segment does not fit in RAM";
    }
    return "unknown error";
}

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
            return "invalid ELF class";
        case ELF_UNSUPPORTED_CLASS:
            return "unsupported ELF class (expected 32-bit)";
        case ELF_BAD_ENDIAN:
            return "invalid ELF data encoding";
        case ELF_BAD_IDENT_VERSION:
            return "unsupported e_ident version";
        case ELF_BAD_OSABI:
            return "unsupported OS/ABI";
        case ELF_BAD_TYPE:
            return "invalid ELF type";
        case ELF_UNSUPPORTED_TYPE:
            return "unsupported ELF type (expected executable)";
        case ELF_BAD_MACHINE:
            return "invalid machine";
        case ELF_UNSUPPORTED_MACHINE:
            return "unsupported machine";
        case ELF_BAD_VERSION:
            return "unsupported ELF version";
        case ELF_BAD_FLAGS:
            return "unsupported processor flags";
        case ELF_BAD_EHSIZE:
            return "bad ELF header size";
        case ELF_BAD_PHENTSIZE:
            return "bad program header entry size";
        case ELF_NO_SEGMENTS:
            return "no program headers";
        case ELF_PHDRS_OUT_OF_FILE:
            return "program header table extends past end of file";
        case ELF_BAD_ENTRY:
            return "entry point is not in a loaded segment";
        case ELF_SEGMENT_OUT_OF_FILE:
            return "segment extends past end of file";
        case ELF_SEGMENT_OUT_OF_RAM:
            return "segment does not fit in RAM";
        case ELF_SEGMENT_FILESZ_GT_MEMSZ:
            return "segment file size exceeds its memory size";
        case ELF_SEGMENT_BAD_ALIGN:
            return "segment alignment is invalid";
    }
    return "unknown error";
}

#include "validate.h"

#include <string.h>

#include "debug.h"

#define MAGIC "\x7f\x45\x4c\x46"

enum ElfError validate_ehdr(const struct Elf32_Ehdr hdr[static 1])
{
    if (memcmp(hdr->e_ident, MAGIC, 4) != 0) {
        return ELF_BAD_MAGIC;
    }

    const u8 ei_class = hdr->e_ident[0x04];
    switch (ei_class) {
        case 1:
        case 2:
            break;
        default:
            return ELF_BAD_CLASS;
    }

    // already checked
    const u8 ei_data = hdr->e_ident[0x05];
    switch (ei_data) {
        case 1:
        case 2:
            break;
        default:
            UNREACHABLE();
    }

    return ELF_OK;
}
